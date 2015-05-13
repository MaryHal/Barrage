-- BulletLua Test Script

local test04
test04 = {
   onLoad = function ()
      math.randomseed(os.time())
   end,

   main = function ()
      frame = getFrameCount()
      setDirection(0)

      if (math.fmod(frame, 30) == 0) then
         setPosition(math.random(200, 440), 488)
         launch(1, math.random(-20, 20), math.random() + 6.5, test04.shoot)
      end
   end,

   shoot = function ()
      frame = getFrameCount()
      setSpeedRelative(-0.07)
      if (frame == 60) then
         for d = 0, 360, 360/30 do
            launch(1, d, 2 + math.random() * 3, test04.fade)
         end
         kill()
      end
   end,

   fade = function ()
      if (getFrameCount() == 30) then
         vanish()
      end
   end
}

return test04
