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
#define checkpalette(L) \
    (liq_palette **)luaL_checkudata(L, 1, "liqua.palette")


/**
 * DESTRUCTORS
 **/

static int liqua_attr_destroy(lua_State *L)
{
    liq_attr **attr = checkattr(L);
    liq_attr_destroy(*attr);
    lua_pop(L, 1);
    return 0;
}

static int liqua_image_destroy(lua_State *L)
{
    liq_image **image = checkimage(L);
    liq_image_destroy(*image);
    lua_pop(L, 1);
    return 0;
}

static int liqua_result_destroy(lua_State *L)
{
    liq_result **result = checkresult(L);
    liq_result_destroy(*result);
    lua_pop(L, 1);
    return 0;
}


/**
 * TOSTRING METHODS
 **/

static int liqua_attr_tostring(lua_State *L)
{
    luaL_checkudata(L, 1, "liqua.attr");
    lua_pushstring(L, "<liqua attribute>");
    return 1;
}

static int liqua_image_tostring(lua_State *L)
{
    luaL_checkudata(L, 1, "liqua.image");
    lua_pushstring(L, "<liqua image>");
    return 1;
}

static int liqua_result_tostring(lua_State *L)
{
    luaL_checkudata(L, 1, "liqua.result");
    lua_pushstring(L, "<liqua result>");
    return 1;
}

static int liqua_palette_tostring(lua_State *L)
{
    luaL_checkudata(L, 1, "liqua.palette");
    lua_pushstring(L, "<liqua palette>");
    return 1;
}



/**
 * LIQ WRAPPER FUNCTIONS
 **/

static int liqua_attr_create(lua_State *L)
{
    liq_attr *attr;
    if ((attr = liq_attr_create()) == NULL)
        return liqua_push_error(L, "Failed to initialize liqua attribute.");
    liq_attr **ud = (liq_attr **)lua_newuserdata(L, sizeof(attr));
    *ud = attr;
    luaL_getmetatable(L, "liqua.attr");
    lua_setmetatable(L, -2);
    return 1;
}

static int liqua_image_create_rgba(lua_State *L)
{
    liq_attr **attr = checkattr(L);
    int w = luaL_checkint(L, 3);
    int h = luaL_checkint(L, 4);
    double gamma = luaL_checknumber(L, 5); /* 0 < gamma < 1 */

    /* Hey, let's parse the RGBA table and make sure it's formatted right, mkay. */
    luaL_checktype(L, 2, LUA_TTABLE);
    int length = luaL_len(L, 2);
    int i,j;
    int **bitmap = (int **)calloc(length, sizeof(int) * 4);
    lua_pushnil(L);
    i=0;
    while (lua_next(L, 2)) {
        luaL_checktype(L, -1, LUA_TTABLE);
        if(luaL_len(L, -1) != 4)
            return liqua_push_error(L, "Invalid RGBA pixel.");
        lua_pushnil(L);
        j=0;
        while(lua_next(L, -2)) {
            bitmap[i][j++] = luaL_checkint(L, -1);
            lua_pop(L, 1);
        }
        i++;
        lua_pop(L, 1);
    }

    liq_image *image;
    if ((image = liq_image_create_rgba(*attr, bitmap, w, h, gamma)) == NULL)
        return liqua_push_error(L, "Failed to create RGBA image.");

    liq_image **ud = (liq_image **)lua_newuserdata(L, sizeof(image));
    *ud = image;
    luaL_getmetatable(L, "liqua.image");
    lua_setmetatable(L, -2);
    return 1;
}

static int liqua_quantize_image(lua_State *L)
{
    liq_attr **attr = checkattr(L);
    liq_image **image = (liq_image **)luaL_checkudata(L, 2);
    liq_result *result;
    if ((result = liq_quantize_image(*attr, *image)) == NULL)
        return liqua_push_error(L, "Image quantization failed.");
    liq_result **ud = (liq_result **)lua_newuserdata(L, sizeof(result));
    *ud = result;
    luaL_getmetatable(L, "liqua.result");
    lua_setmetatable(L, -2);
    return 1;
}

static int liqua_set_quality(lua_State *L)
{
    liq_attr **attr = checkattr(L);

    /* min and max are in range: 0 <= x <= 100 */
    int min = luaL_checkint(L, 2);
    int max = luaL_checkint(L, 3);
    switch (liq_set_quality(*attr, min, max)) {
        case LIQ_OK:
            break;
        case LIQ_VALUE_OUT_OF_RANGE:
            return liqua_push_error(L, "Quality values out of range.");
        default:
            return liqua_push_error(L, "Something really bad but vague happened.");
    }
    return 0;
}

/**
 * TODO: LUA Buffer for output.
 * Write straight to file, or send the buffer to Lua?
 **/
static int liqua_write_remapped_image(lua_State *L)
{
    liq_result **result = checkresult(L);
    liq_image **image = (liq_image **)luaL_checkudata(L, 2);
    // TODO: handle errors differently
    switch(liq_write_remapped_image(*result, *image, buffer, buffer_size)) {
        case LIQ_OK:
            break;
        case LIQ_BUFFER_TOO_SMALL:
            return liqua_push_error(L,"Buffer too small to write image.");
        default:
            return liqua_push_error(L,"Something went wrong while writing image but I don't know what?");
    }
    return 0;
}

static int liqua_set_dithering_level(lua_State *L)
{
    liq_result **result = checkresult(L);
    float f = luaL_checknumber(L, 2);
    // TODO: handle errors differently
    switch(liq_set_dithering_level(*result, f)) {
        case LIQ_OK:
            break;
        case LIQ_VALUE_OUT_OF_RANGE:
            return liqua_push_error(L, "Dither value out of range. Must be between 0 and 1 (inclusive).");
        default:
            return liqua_push_error(L,"Something went wrong while setting dither level, but I don't know what?");
    }
    return 0;
}

static int liqua_get_palette(lua_State *L)
{
    /* TODO: need a way to free the palette memory */
    liq_result **res = checkresult(L);
    const *liq_palette;
    if ((palette = liq_get_palette(*res)) == NULL)
        return liqua_push_error(L, "Could not retrieve palette.");
    liq_palette **ud = (liq_palette **)lua_newuserdata(L, sizeof(palette));
    *ud = palette;
    luaL_getmetatable(L, "liqua.palette");
    lua_setmetatable(L, -2);
    return 1;
}

int liqua_push_error(lua_State *L, const char[] msg)
{
    lua_pushnil(L);
    lua_pushstring(L, msg);
    return 2;
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
    {"quantize_image", liqua_quantize_image},
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
    {"get_palette", liqua_get_palette},
    {"__tostring", liqua_result_tostring},
    {"write_remapped_image", liqua_write_remapped_image},
    {NULL, NULL}
};

static const struct luaL_Reg liqua_palette [] = {
    {"__tostring", liqua_palette_tostring},
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

    /* liq_palette */
    luaL_newmetatable(L, "liqua.palette");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, liqua_palette, 0);

    luaL_newlib(L, liqua);
    return 1;
}
