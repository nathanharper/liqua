liq = require"liqua"
attr,err = liq.attr_create()
print(attr,err)
image,err = attr:image_create_rgba("jjjj",1,3,0.5)
print(image,err)
