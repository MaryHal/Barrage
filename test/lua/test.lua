barrage = require("libbarrage")

b = barrage.new("script.lua", 320.0, 120.0)

print("Hello World")

for i = 1, 10 do
   b:tick()
   x, y = b:yield()
   print(x, y)
end
