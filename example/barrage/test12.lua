
local test12
test12 = {
   main = function ()
      turn = getTurn()
      rank = getRank()
      if (math.fmod(turn, math.floor(120 * (1.2 - rank))) == 0) then
         launch(180, 4, test12.shoot)
         -- launch(160, 3.5, shoot)
         -- launch(200, 4.5, shoot)
      end
   end,

   shoot = function ()
      aimAtTarget()
      if (getTurn() == 40) then
         launchCircle(40, 8, test12.blaze)
         kill()
      end
   end,

   blaze = function ()
      setDirectionRelative(8)

      if (getTurn() == 20) then
         vanish()
      end
   end
}
return test12
