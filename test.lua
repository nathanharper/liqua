-- liqua.c:212:8: warning: return type defaults to ‘int’ [-Wreturn-type]
-- liqua.c:220:8: warning: return type defaults to ‘int’ [-Wreturn-type]
-- liqua.c: In function ‘liqua_get_palette’:
-- liqua.c:257:9: warning: assignment discards ‘const’ qualifier from pointer target type [enabled by default]
liq = require "liqua"

local pixels = {
  {255, 255, 255, 255},
  {  0,   0,   0, 255},
  {255, 255, 255, 255},
  {  0,   0,   0, 255},
  {255, 255, 255, 255}
}

attr,err = liq.attr_create()
print(attr,err)

image,err = attr:image_create_rgba(pixels, 5, 1, 0.5)
print(image,err)

result,err = attr:quantize_image(image)
print(result,err)

data,err = result:write_remapped_image(image, bitmap)
print(data,err)

palette,err = result:get_palette()
print(palette,err)

attr:destroy()
image:destroy()
result:destroy()
