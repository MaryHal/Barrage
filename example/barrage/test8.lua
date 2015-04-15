theta = 0

function main()
   turn = getTurn()

   launch(theta, 10, shoot)
   theta = theta + 27

   if (turn >= 600) then
      vanish()
   end
end

function shoot()
   turn = getTurn()
   rank = getRank()

   dir = getDirection()
   for i = 0, 10 do
      launch(dir + i * 30, 3.0 + 0.4 * rank, turnaround)
   end

   -- launch(dir + 180, 6, nullfunc)

   kill()
end

function turnaround()
   turn = getTurn()
   if (turn == 30) then
      setDirection(getDirection() + 180)
   elseif (turn > 60) then
      vanish()
   end
end
