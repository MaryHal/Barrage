local theta = 0

local test07
test07 = {
   main = function ()
      setPosition(320, 240)
      turn = getTurn()

      if (math.fmod(turn, 6) == 0) then
         for i = 0, 11 do
            launch(theta + i * 30, 2, test07.blue)
            theta = theta + 0.1
         end
      end
   end,

   blue = function ()
      turn = getTurn()
      if (turn == 30) then
         currentDirection = getDirection()
         launch(currentDirection - 110, 2, test07.final)

         setFunction(test07.final)
      end
   end,

   final = function ()
      if (getTurn() == 60) then
         vanish()
      end
   end
}

return test07
