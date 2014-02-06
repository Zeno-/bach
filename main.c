#include <stdio.h>
#include <string.h>
#include "config_master.h"

#include "vpu/config.h"
#include "vpu/vpu_tests.h"

#include "lua/lua_iface.h"

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
static void printusage(const char *progname, FILE *fp);

static void run_tests(void);

int main(int argc, char **argv)
{
    int r = 1;
    struct setup setup;

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
    const char *s, *p;

    if (argc == 0 || *argv[0] != '\0')
        return DEFAULT_PROG_NAME;

    s = argv[0];

    /* Skip over path */
    while ((p = strpbrk(s, "\\/")))
        s = p + 1;

    if (*s == '\0')
        s = DEFAULT_PROG_NAME;

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
#   ifdef VPU_RUNTEST_TERRAIN
        vputest_genterrain();
#   endif
}

