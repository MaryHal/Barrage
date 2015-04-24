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
   if key == ' ' then
      b = barrage.new('barrage/' .. barrageFileList[barrageIndex], 320.0, 120.0)
   elseif key == 'left' then
      barrageIndex = barrageIndex - 1
      if barrageIndex < 1 then
         barrageIndex = #barrageFileList
      end
   elseif key == 'right' then
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
   love.graphics.print(barrageFileList[barrageIndex], 8, 8)
   while b:hasNext() do
      local x, y = b:yield()
      love.graphics.draw(bulletImg, x, y, 0, 0.5, 0.5)
   end
end
