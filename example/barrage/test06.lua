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
      turn = getTurn()

      if (math.fmod(turn, 120) == 0) then
         -- Cycle between directions
         direction = initialDirections[math.fmod(shotCount, #initialDirections) + 1]
         shotCount = shotCount + 1
         launch(direction, 1.5, test06.shoot)
      end
   end,

   shoot = function ()
      local turn = getTurn()

      if (turn < 60) then
         setDirectionRelative(3)
         setSpeedRelative(0.05)

         -- vx, vy = getVelocity()
         -- s = getSpeed()
         -- d = getDirection()
         -- print (turn, vx, vy, s, d)
      elseif (turn == 60) then
         test06.burst()
      end
   end,

   burst = function ()
      local twirlcw  = bind(test06.twirl, 1)
      local twirlccw = bind(test06.twirl, -1)

      local divs = 15
      for initialDirection = 1, 360/divs do
         launch(initialDirection * divs, 3.7, twirlcw)
         launch(initialDirection * divs, 3.7, twirlccw)

         launch(initialDirection * divs, 3.0, twirlcw)
         launch(initialDirection * divs, 3.0, twirlccw)

         launch(initialDirection * divs, 2.3, twirlcw)
         launch(initialDirection * divs, 2.3, twirlccw)
      end
      kill()
   end,

   twirl = function (dir)
      local turn = getTurn()

      setDirectionRelative(dir * 2)
      setSpeedRelative(0.08)

      if (turn == 90) then
         vanish()
      end
   end
}

return test06
