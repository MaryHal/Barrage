local barrage = require "barrageC"

local bulletImg = nil
local b = nil

function love.load(arg)
   bulletImg = love.graphics.newImage('assets/bullet.png')
   barrage.setRngSeed()
   b = barrage.new(arg[2], 320.0, 120.0)
end

function love.update(dt)
   local x, y = love.mouse.getPosition()
   b:setPlayerPosition(x, y)

   print("update begin")
   b:tick()
   print("update end")
end

function love.draw(dt)
   print("draw begin")
   while b:hasNext() do
      local x, y = b:yield()
      love.graphics.draw(bulletImg, x, y, 0, 0.5, 0.5)
   end
   print("draw end")
end
