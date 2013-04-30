#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "lib/libimagequant.h"
#include "lua5.2/lua.h"
#include "lua5.2/lauxlib.h"
#include "lua5.2/lualib.h"

/* typecheck macros */
#define checkattr(L) \
    (liq_attr **)luaL_checkudata(L, 1, "liqua.attr")
#define checkimage(L) \
    (liq_image **)luaL_checkudata(L, 1, "liqua.image")
#define checkresult(L) \
    (liq_result **)luaL_checkudata(L, 1, "liqua.result")


/**
 * DESTRUCTORS
 **/

static int liqua_attr_destroy(lua_State *L)
{
    liq_attr **attr = checkattr(L);
    liq_attr_destroy(*attr);
    return 0;
}

static int liqua_image_destroy(lua_State *L)
{
    liq_image **image = checkimage(L);
    liq_image_destroy(*image);
    return 0;
}

static int liqua_result_destroy(lua_State *L)
{
    liq_result **result = checkresult(L);
    liq_result_destroy(*result);
    return 0;
}


/**
 * TOSTRING METHODS
 **/

static int liqua_attr_tostring(lua_State *L)
{
    /* checkattr(L); */
    lua_pushstring(L, "<liqua attribute>");
    return 1;
}

static int liqua_image_tostring(lua_State *L)
{
    /* checkimage(L); */
    lua_pushstring(L, "<liqua image>");
    return 1;
}

static int liqua_result_tostring(lua_State *L)
{
    /* checkresult(L); */
    lua_pushstring(L, "<liqua result>");
    return 1;
}



/**
 * LIQ WRAPPER FUNCTIONS
 **/

static int liqua_attr_create(lua_State *L)
{
    liq_attr *attr = liq_attr_create();

    if (attr == NULL) {
        lua_pushnil(L);
        lua_pushstring(L, "Failed to initialize liq attribute.");
        return 2;
    }

    liq_attr **ud = (liq_attr *)lua_newuserdata(L, sizeof(attr));
    *ud = attr;
    luaL_getmetatable(L, "liqua.attr");
    lua_setmetatable(L, -2);
    return 1;
}

static int liqua_image_create_rgba(lua_State *L)
{
    liq_attr **attr = checkattr(L);
    const char *bmp = luaL_checkstring(L, 2); /* TODO: this should be a table. */
    char *bmp_copy = strdup(bmp);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    double gamma = luaL_checknumber(L, 5); /* 0 < gamma < 1 */

    // FOR TATSTINGK
    
    int list[3][4] = {
        {255,255,255,255},
        {255,255,255,255},
        {255,255,255,255}
    };

    liq_image *image = liq_image_create_rgba(*attr, list, w, h, gamma);

    if (image == NULL) {
        lua_pushnil(L);
        lua_pushstring(L, "Failed to create RGBA image.");
        return 2;
    }

    liq_image **ud = (liq_image *)lua_newuserdata(L, sizeof(image));
    *ud = image;
    luaL_getmetatable(L, "liqua.image");
    lua_setmetatable(L, -2);
    return 1;
}



/**
 * FUNCTION MAPPINGS
 **/

static const struct luaL_Reg liqua [] = {
    {"attr_create", liqua_attr_create},
    {NULL, NULL}
};

static const struct luaL_Reg liqua_attr [] = {
    {"destroy", liqua_attr_destroy},
    {"image_create_rgba", liqua_image_create_rgba},
    {"__tostring", liqua_attr_tostring},
    {NULL, NULL}
};

static const struct luaL_Reg liqua_image [] = {
    {"destroy", liqua_image_destroy},
    {"__tostring", liqua_image_tostring},
    {NULL, NULL}
};

static const struct luaL_Reg liqua_result [] = {
    {"destroy", liqua_result_destroy},
    {"__tostring", liqua_result_tostring},
    {NULL, NULL}
};



/**
 * INITIALIZE BINDINGS
 **/

int luaopen_liqua(lua_State *L)
{
    /* liq_attr */
    luaL_newmetatable(L, "liqua.attr");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, liqua_attr, 0);

    /* liq_image */
    luaL_newmetatable(L, "liqua.image");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, liqua_image, 0);

    /* liq_result */
    luaL_newmetatable(L, "liqua.result");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, liqua_result, 0);

    luaL_newlib(L, liqua);
    return 1;
}
