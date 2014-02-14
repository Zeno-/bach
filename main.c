#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "config_master.h"

#include "vpu/config.h"
#include "vpu/vpu_tests.h"

#include "lua/lua_iface.h"


#include "cpu/instructions.h"

enum {
    RUNMODE_ERR_PARSEARGS,
    RUNMODE_SHOWUSAGE,
    RUNMODE_LUA,
    RUNMODE_NORMAL
};

struct setup {
    int runmode;
    const char *luafilename;

};

static int run_normal(struct setup *setup);
static int run_lua(struct setup *setup);
static int parseargs(int argc, char **argv, struct setup *setup);
static const char *progname(int argc, char **argv);
static const char *skipleadingpath(const char *s);
static void printusage(const char *progname, FILE *fp);

static void run_tests(void);


void
testcpuinstructions(void)
{
    uint32_t i;

    i = INSTR_MAKEMASK(INSTR_OPCODE_BITS, INSTR_OPCODE_MSBPOS);
    printf("%x\n", i);

    i = INSTR_MAKEMASK(INSTR_SETCFLAGS_BITS, INSTR_SETCFLAGS_MSBPOS);
    printf("%x\n", i);

    i = 0;
    i = INSTR_SETPART(i, 0x3f, INSTR_OPCODE_BITS, INSTR_OPCODE_MSBPOS);
    printf("%x\n", i);

    i = INSTR_GETPART(i, INSTR_OPCODE_BITS, INSTR_OPCODE_MSBPOS);
    printf("%x\n", i);

    i = 0;
    i = INSTR_SETPART(i, 0xc0c0c0c0, 32, 31);
    printf("%x\n", i);

    i = INSTR_GETPART(i, 32, 31);
    printf("%x\n", i);

    i = 0;
    i = INSTR_OPCODE_SET(i, 0x12);
    printf("%x\n", i);

    i = INSTR_OPCODE_GET(i);
    printf("%x\n", i);
}

int main(int argc, char **argv)
{
    int r = 1;
    struct setup setup;

    testcpuinstructions();

    switch (parseargs(argc, argv, &setup)) {
    case RUNMODE_SHOWUSAGE:
    case RUNMODE_ERR_PARSEARGS:
        printusage(progname(argc, argv), stderr);
        break;
    case RUNMODE_LUA:
        r = run_lua(&setup);
        break;
    case RUNMODE_NORMAL:
        r = run_normal(&setup);
        break;
    }
    return r;
}

static int
run_normal(struct setup *setup)
{
    (void)setup;  /*UNUSED */

#ifdef VPU_RUNTESTS
    run_tests();
    return 0;
#endif
}

static int
run_lua(struct setup *setup)
{
    setup->luafilename = "/home/crobbins/code/bach/lua/examples/test.lua";
    /* ^^^^ FIXME: test only */
    return zvm_lua_runscript(setup->luafilename);
}

static int
parseargs(int argc, char **argv, struct setup *setup)
{
    int mode = DEFAULT_RUNMODE;
    int i;

    for (i = 1; i < argc; i++) {
        /* FIXME: Implement */
    }

    setup->runmode = mode;
    return setup->runmode;
}

static const char *
progname(int argc, char **argv)
{
    const char *s;

    if (argc == 0 || *argv[0] == '\0')
        return DEFAULT_PROG_NAME;

    s = skipleadingpath(argv[0]);

    return *s == '\0' ? DEFAULT_PROG_NAME : s;
}

static const char *
skipleadingpath(const char *s)
{
    const char *p;

    /* Skip over path */
    while ((p = strpbrk(s, "\\/"))) {
        while (isspace(*(p + 1)))
            p++;
        if (*(p + 1) == '\0')
            break;
        s = p + 1;
    }
    return s;
}

static void
printusage(const char *progname, FILE *fp)
{
    fprintf(fp, "Usage: %s [options]\n", progname);
    fprintf(fp, "Options:\n");
    fprintf(fp, "  %-25s %s\n", "--lua luafilename",
                "Runs the Lua program given by 'luafilename'.\n");
}

static void
run_tests(void)
{
#   if (VPU_RUNTEST_TERRAIN == 1)
        vputest_genterrain();
#   endif
}

