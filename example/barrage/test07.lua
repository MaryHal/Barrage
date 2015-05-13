local theta = 0

local test07
test07 = {
   main = function ()
      setPosition(320, 240)
      frame = getFrameCount()

      if (math.fmod(frame, 6) == 0) then
         for i = 0, 11 do
            launch(1, theta + i * 30, 2, test07.blue)
            theta = theta + 0.1
         end
      end
   end,

   blue = function ()
      frame = getFrameCount()
      if (frame == 30) then
         currentDirection = getDirection()
         launch(1, currentDirection - 110, 2, test07.final)

         setFunction(test07.final)
      end
   end,

   final = function ()
      if (getFrameCount() == 60) then
         vanish()
      end
   end
}

return test07
