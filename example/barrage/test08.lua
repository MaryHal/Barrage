local theta = 0

local test08
test08 = {
   main = function ()
      frame = getFrameCount()

      launch(1, theta, 10, test08.shoot)
      theta = theta + 27

      if (frame >= 600) then
         vanish()
      end
   end,

   shoot = function ()
      frame = getFrameCount()
      rank = getRank()

      dir = getDirection()
      for i = 0, 10 do
         launch(1, dir + i * 30, 3.0 + 0.4 * rank, test08.turnaround)
      end

      -- launch(1, dir + 180, 6, nullfunc)

      kill()
   end,

   turnaround = function ()
      frame = getFrameCount()
      if (frame == 30) then
         setDirection(getDirection() + 180)
      elseif (frame > 60) then
         vanish()
      end
   end
}
return test08
