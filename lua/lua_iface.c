#include "lua_iface.h"

#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "machine.h"
#include "vpu/fonts/bmfonts.h"
#include "lua/lua_config.h"


#include "vpu/vpu_tests.h"

struct mhdl {
    struct machine *M;
};

/* Lua standard libraries to load
 */
static const luaL_reg lualibs[] = {
    { "base",       luaopen_base },
    //{ "os",         luaopen_os },
    { NULL,         NULL }
};

/* Fwd decl (defined at end of this file) */
static const struct luaL_reg lib_funcs[];
static const struct luaL_reg lib_methods[];

/* --------------------------------------------------------------------
   Functions not for use within Lua (PRIVATE)
   -------------------------------------------------------------------*/
static int
errorh(lua_State *L)
{
    fputs(lua_tostring(L, 1), stderr);
    fputc('\n', stderr);

    return 0;
}

static void
openlualibs(lua_State *L)
{
    const luaL_reg *lib;

    for (lib = lualibs; lib->func != NULL; lib++) {
        lib->func(L);
        lua_settop(L, 0);
    }
}

static int
garbagecollect(lua_State *L)
{
    struct mhdl *m = luaL_checkudata(L, 1, "bach.machinehandle");
    machine_poweroff(&m->M);
    return 0;
}

static int
initexports(lua_State *L)
{
    static const struct luaL_reg *f;

    luaL_newmetatable(L, "bach.machinehandle");

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, garbagecollect);
    lua_setfield(L, -2, "__gc");

    /* install methods into metatable */
    for (f = lib_methods; f->func != NULL && f->name != NULL; f++) {
        lua_pushcfunction(L, f->func);
        lua_setfield(L, -2, f->name);
    }

    lua_createtable(L, 1, 0);
    for (f = lib_funcs; f->func != NULL && f->name != NULL; f++) {
        lua_pushcfunction(L, f->func);
        lua_setfield(L, -2, f->name);
    }

    luaL_getmetatable(L, "bach.machinehandle");
    lua_setmetatable(L, -2);

    lua_setglobal(L, "bm");

    return 1;
}

static lua_State *
newluainstance(int *errhandler)
{
    lua_State *L;

    /* Initialise interface */
    if ((L = lua_open()) == NULL) {
        fputs("Could not initialise Lua (memory error)\n", stderr);
        return NULL;
    }

    openlualibs(L);
    initexports(L);

    lua_pushcfunction(L, errorh);  /* push traceback function */
    *errhandler = lua_gettop(L);

    return L;
}

static int
loadscript(lua_State *L, const char *scriptname)
{
    int lr;

    if ((lr = luaL_loadfile(L, scriptname)) != 0) {
        fputs("Error loading ", stderr);
        fputs(scriptname, stderr);
        fputs(": ", stderr);
        switch (lr) {
        case LUA_ERRFILE:
            fputs("could not open file\n", stderr);
            break;
        case LUA_ERRSYNTAX:
            fputs("pre-compilation syntax error\n", stderr);
            break;
        case LUA_ERRMEM:
            fputs("memory allocation error\n", stderr);
            break;
        default:
            fputs("unknown error\n", stderr);
            break;
        }
        return 1;
     }
    return 0;   /* No error */
}

/* --------------------------------------------------------------------
    C Public
   -------------------------------------------------------------------*/

int
zvm_lua_runscript(const char *filename)
{
    lua_State *L;
    int errhandler;

    if (filename == NULL || *filename == '\0') {
        fputs("Run Lua: error invalid filename\n", stderr);
        return 1;   /* invalid filename */
    }

    if ((L = newluainstance(&errhandler)) == 0)
        return 1;   /* fail */

    if (loadscript(L, filename) == 0) {
        lua_pcall(L, 0, 0, errhandler);
    }

    lua_close(L);

    return 0;
}

/* --------------------------------------------------------------------
   Functions exported to Lua
   -------------------------------------------------------------------*/

static int
l_runterraintest(lua_State *L)
{
    (void)L;    /* UNUSED */
    vputest_genterrain();
    return 0;   /* no return values */
}

/* --------------------------------------------------------------------*/

static int
l_newmachine(lua_State *L)
{
    struct mhdl *m;

    m = lua_newuserdata(L, sizeof *m);
    luaL_getmetatable(L, "bach.machinehandle");
    lua_setmetatable(L, -2);

    /* FIXME: Error checking */
    m->M = machine_new();

    return 1;
}

static int
l_machine_poweron(lua_State *L)
{
    /* FIXME: Error checking */

    struct mhdl *m;
    struct machine_config cfg = {
       800, 600, 0, &vidfont8x8
    };

    m = luaL_checkudata(L, 1, "bach.machinehandle");
    machine_poweron(m->M, &cfg);
    return 0;
}

static int
l_esys_waitforquit(lua_State *L)
{
    struct mhdl *m;

    m = luaL_checkudata(L, 1, "bach.machinehandle");
    evsys_waitforquit(m->M->esys);
    return 0;
}

/* --------------------------------------------------------------------
   Lua export definitions
   -------------------------------------------------------------------*/

static const struct luaL_reg lib_methods[] = {

    /* ---- Machine  -------------------------------------------------*/

    { "poweron",            l_machine_poweron   },

    /* ---- Event system ---------------------------------------------*/

    { "waitforquit",        l_esys_waitforquit  },

    /* ---- Tests ----------------------------------------------------*/

    { "runterraintest",     l_runterraintest    },

    { NULL, NULL }
};

static const struct luaL_reg lib_funcs[] = {
    { "newmachine",          l_newmachine       },
    { NULL, NULL }
};
