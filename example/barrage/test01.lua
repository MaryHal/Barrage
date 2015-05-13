-- BulletLua Test Script
local dir = 0

local test01
test01 = {
   main = function ()
      local frame = getFrameCount()
      local rank = getRank()

      if (math.fmod(frame, 15) == 0) then
         launch(1, dir, 1.5, test01.explode)
         dir = dir + 34
      end
   end,

   explode = function ()
      local frame = getFrameCount()
      if (frame == 60) then
         launchCircle(1, 40, 4.0, test01.homeIn)

         kill()
      end
   end,

   homeIn = function ()
      local frame = getFrameCount()
      if (frame == 20) then
         setSpeed(1)
      elseif (frame == 25) then
         aimAtTarget()
         setSpeed(10)
      elseif (frame == 50) then
         vanish()
      end
   end
}

return test01
