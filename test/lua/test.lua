local barrage = require("barrageC")

local b = barrage.newBarrage("script.lua", 320.0, 120.0)
local sp = barrage.newSpacialPartition()

print("Hello World")

for i = 1, 10 do
   b:tick(sp)
   x, y = b:yield()
   print(x, y)
end
