local theta = 0

local test08
test08 = {
   main = function ()
      turn = getTurn()

      launch(1, theta, 10, test08.shoot)
      theta = theta + 27

      if (turn >= 600) then
         vanish()
      end
   end,

   shoot = function ()
      turn = getTurn()
      rank = getRank()

      dir = getDirection()
      for i = 0, 10 do
         launch(1, dir + i * 30, 3.0 + 0.4 * rank, test08.turnaround)
      end

      -- launch(1, dir + 180, 6, nullfunc)

      kill()
   end,

   turnaround = function ()
      turn = getTurn()
      if (turn == 30) then
         setDirection(getDirection() + 180)
      elseif (turn > 60) then
         vanish()
      end
   end
}
return test08
