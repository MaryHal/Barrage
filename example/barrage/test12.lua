
local test12
test12 = {
   main = function ()
      frame = getFrameCount()
      rank = getRank()
      if (math.fmod(frame, math.floor(120 * (1.2 - rank))) == 0) then
         launch(1, 180, 4, test12.shoot)
         -- launch(1, 160, 3.5, shoot)
         -- launch(1, 200, 4.5, shoot)
      end
   end,

   shoot = function ()
      aimAtTarget()
      if (getFrameCount() == 40) then
         launchCircle(1, 40, 8, test12.blaze)
         kill()
      end
   end,

   blaze = function ()
      setDirectionRelative(8)

      if (getFrameCount() == 20) then
         vanish()
      end
   end
}
return test12
