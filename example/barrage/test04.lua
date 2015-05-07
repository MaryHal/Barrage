-- BulletLua Test Script

local test04
test04 = {
   onLoad = function ()
      math.randomseed(os.time())
   end,

   main = function ()
      turn = getTurn()
      setDirection(0)

      if (math.fmod(turn, 30) == 0) then
         setPosition(math.random(200, 440), 488)
         launch(math.random(-20, 20), math.random() + 6.5, test04.shoot)
      end
   end,

   shoot = function ()
      turn = getTurn()
      setSpeedRelative(-0.07)
      if (turn == 60) then
         for d = 0, 360, 360/30 do
            launch(d, 2 + math.random() * 3, test04.fade)
         end
         kill()
      end
   end,

   fade = function ()
      if (getTurn() == 30) then
         vanish()
      end
   end
}

return test04
