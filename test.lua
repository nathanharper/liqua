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

print(result:write_remapped_image(image, bitmap))

palette,err = result:get_palette()
print(palette,err)

attr:destroy()
image:destroy()
result:destroy()
