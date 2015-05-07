-- BulletLua Test Script
local dir = 0

local test01
test01 = {
   main = function ()
      local turn = getTurn()
      local rank = getRank()

      if (math.fmod(turn, 15) == 0) then
         launch(dir, 1.5, test01.explode)
         dir = dir + 34
      end
   end,

   explode = function ()
      local turn = getTurn()
      if (turn == 60) then
         launchCircle(40, 4.0, test01.homeIn)

         kill()
      end
   end,

   homeIn = function ()
      local turn = getTurn()
      if (turn == 20) then
         setSpeed(1)
      elseif (turn == 25) then
         aimAtTarget()
         setSpeed(10)
      elseif (turn == 50) then
         vanish()
      end
   end
}

return test01
