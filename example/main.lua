local barrage = require "barrageC"

local bulletImg = love.graphics.newImage('assets/bullet.png')
local bulletQuad = love.graphics.newQuad(0, 0, 32, 32, bulletImg:getDimensions())
local barrageBatch = love.graphics.newSpriteBatch(bulletImg, 4096)

local myBarrage = nil

local barrageIndex = 1
local barrageFileList = love.filesystem.getDirectoryItems('barrage')

local font = love.graphics.newFont(12)

function love.load(arg)
   myBarrage = barrage.new('barrage/' .. barrageFileList[barrageIndex], 320.0, 120.0)
end

function love.keypressed(key)
   if key == ' ' or key == 'return' then
      myBarrage = barrage.new('barrage/' .. barrageFileList[barrageIndex], 320.0, 120.0)
   elseif key == 'left' or key == 'a' then
      barrageIndex = barrageIndex - 1
      if barrageIndex < 1 then
         barrageIndex = #barrageFileList
      end
   elseif key == 'right' or key == 'd' then
      barrageIndex = barrageIndex + 1
      if barrageIndex > #barrageFileList then
         barrageIndex = 1
      end
   end
end

function love.update(dt)
   local x, y = love.mouse.getPosition()
   myBarrage:setPlayerPosition(x, y)

   myBarrage:tick()

   barrageBatch:clear()
   barrageBatch:bind()
   do
      while myBarrage:hasNext() do
         local x, y, vx, vy, frame = myBarrage:yield()
         if frame < 0 then
            barrageBatch:setColor(255, 255, 255, 255 - (31 + frame) * 255 / 30)
         else
            barrageBatch:setColor(255, 255, 255)
         end
         barrageBatch:add(bulletQuad, x - 8, y - 8, 0, 0.5, 0.5)
      end
   end
   barrageBatch:unbind()
end

function love.draw(dt)
   love.graphics.setFont(font)
   love.graphics.setColor(255, 255, 255)
   love.graphics.print(barrageFileList[barrageIndex], 8, 8)

   love.graphics.draw(barrageBatch)
end
