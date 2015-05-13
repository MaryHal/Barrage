-- BulletLua Test Script 6

local function bind(f,...)
   local args = {...}
   return function(...)
      return f(unpack(args),...)
   end
end

local shotCount = 0
local initialDirections = {0, 40, 80, 120, 160}

local test06
test06 = {
   main = function ()
      frame = getFrameCount()

      if (math.fmod(frame, 120) == 0) then
         -- Cycle between directions
         direction = initialDirections[math.fmod(shotCount, #initialDirections) + 1]
         shotCount = shotCount + 1
         launch(1, direction, 1.5, test06.shoot)
      end
   end,

   shoot = function ()
      local frame = getFrameCount()

      if (frame < 60) then
         setDirectionRelative(3)
         setSpeedRelative(0.05)

         -- vx, vy = getVelocity()
         -- s = getSpeed()
         -- d = getDirection()
         -- print (frame, vx, vy, s, d)
      elseif (frame == 60) then
         test06.burst()
      end
   end,

   burst = function ()
      local twirlcw  = bind(test06.twirl, 1)
      local twirlccw = bind(test06.twirl, -1)

      local divs = 15
      for initialDirection = 1, 360/divs do
         launch(1, initialDirection * divs, 3.7, twirlcw)
         launch(1, initialDirection * divs, 3.7, twirlccw)

         launch(1, initialDirection * divs, 3.0, twirlcw)
         launch(1, initialDirection * divs, 3.0, twirlccw)

         launch(1, initialDirection * divs, 2.3, twirlcw)
         launch(1, initialDirection * divs, 2.3, twirlccw)
      end
      kill()
   end,

   twirl = function (dir)
      local frame = getFrameCount()

      setDirectionRelative(dir * 2)
      setSpeedRelative(0.08)

      if (frame == 90) then
         vanish()
      end
   end
}

return test06
