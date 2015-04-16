barrage = require("libbarrage")

bulletImg = nil

b = nil

function love.load(arg)
   bulletImg = love.graphics.newImage('assets/bullet.png')
   print (bulletImg:getWidth(), bulletImg:getHeight())
   b = barrage.new('barrage/test3.lua', 320.0, 120.0)
end

function love.update(dt)
   b:tick()
   -- count = b:getActiveCount()
   -- print(count)
end

function love.draw(dt)
   while b:nextAvailable() do
      x, y = b:yield()
      -- print (x, y)
      love.graphics.draw(bulletImg, x, y, 0, 0.5, 0.5)
   end
end
