local barrage = require "barrageC"

local bulletImg = nil
local b = nil

local barrageIndex = 1
local barrageFilelist = nil

local font = nil

function love.load(arg)
   bulletImg = love.graphics.newImage('assets/bullet.png')

   font = love.graphics.newFont(12)
   barrageFileList = love.filesystem.getDirectoryItems('barrage')

   b = barrage.new('barrage/' .. barrageFileList[barrageIndex], 320.0, 120.0)
end

function love.keypressed(key)
   if key == ' ' or key == 'return' then
      b = barrage.new('barrage/' .. barrageFileList[barrageIndex], 320.0, 120.0)
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
   b:setPlayerPosition(x, y)

   b:tick()
end

function love.draw(dt)
   love.graphics.setFont(font)
   love.graphics.setColor(255, 255, 255)
   love.graphics.print(barrageFileList[barrageIndex], 8, 8)

   while b:hasNext() do
      local x, y, vx, vy, frame = b:yield()
      if frame < 0 then
         love.graphics.setColor(255, 255, 255, 255 - (31 + frame) * 255 / 30)
      else
         love.graphics.setColor(255, 255, 255)
      end
      love.graphics.draw(bulletImg, x - 8, y - 8, 0, 0.5, 0.5)
   end
end
