#include "lua_iface.h"

#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "machine.h"

#include "lua/lua_config.h"

#include "vpu/vpu_tests.h"

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
initexports(lua_State *L)
{
    luaL_newmetatable(L, "bach.machinehandle");

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luaL_register(L, NULL, lib_methods);
    luaL_register(L, BACH_LUA_LIBNAME, lib_funcs);

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
    struct machine *M;
    M = luaL_checkudata(L, 1, "bach.machinehandle");
    vputest_genterrain();
    return 0;   /* no return values */
}

static int
l_newmachine(lua_State *L)
{
    struct machine *M;

    M = machine_new();
    if (!M) {
        /* FIXME: Report error */
        return 0;
    }

    lua_newuserdata(L, sizeof M);

    return 1;
}

/* Exported (to Lua) functions
 */
static const struct luaL_reg lib_funcs[] = {
    { "newmachine",          l_newmachine    },
    { NULL, NULL }
};

static const struct luaL_reg lib_methods[] = {
    { "runterraintest",     l_runterraintest    },
    { NULL, NULL }
};
