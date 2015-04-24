-- BulletLua Test Script

function onLoad()
   math.randomseed(os.time())
end

function main()
   turn = getTurn()
   setDirection(0)

   if (math.fmod(turn, 30) == 0) then
      setPosition(math.random(200, 440), 488)
      launch(math.random(-20, 20), math.random() + 6.5, shoot)
   end
end

function shoot()
   turn = getTurn()
   setSpeedRelative(-0.07)
   if (turn == 60) then
      for d = 0, 360, 360/30 do
         launch(d, 2 + math.random() * 3, fade)
      end
      kill()
   end
end

function fade()
   if (getTurn() == 30) then
      vanish()
   end
end
