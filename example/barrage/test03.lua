-- BulletLua Test Script 3

function main()
   local turn = getTurn()

   setPosition(320, 200)

   d = math.fmod(turn, 6)
   if (d == 0) then
      launch(0, 1, curve)
      launch(90, 1, curve)
      launch(180, 1, curve)
      launch(270, 1, curve)

      launch(0, 3, curve2)
      launch(90, 3, curve2)
      launch(180, 3, curve2)
      launch(270, 3, curve2)
   end

   if (turn >= 120) then
      vanish()
   end
end

function curve()
   if (getSpeed() <= 1.5) then
      setSpeedRelative(0.06)
   end

   setDirectionRelative(3)

   if (getTurn() > 200) then
      vanish()
   end
end

function curve2()
   setDirectionRelative(3)

   if (getTurn() > 200) then
      vanish()
   end
end
