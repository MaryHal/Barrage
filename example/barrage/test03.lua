-- BulletLua Test Script 3

local test03
test03 = {
   main = function ()
      local turn = getTurn()

      setPosition(320, 200)

      d = math.fmod(turn, 6)
      if (d == 0) then
         launch(0, 1, test03.curve)
         launch(90, 1, test03.curve)
         launch(180, 1, test03.curve)
         launch(270, 1, test03.curve)

         launch(0, 3, test03.curve2)
         launch(90, 3, test03.curve2)
         launch(180, 3, test03.curve2)
         launch(270, 3, test03.curve2)
      end

      if (turn >= 120) then
         vanish()
      end
   end,

   curve = function ()
      if (getSpeed() <= 1.5) then
         setSpeedRelative(0.06)
      end

      setDirectionRelative(3)

      if (getTurn() > 200) then
         vanish()
      end
   end,

   curve2 = function ()
      setDirectionRelative(3)

      if (getTurn() > 200) then
         vanish()
      end
   end
}

return test03
