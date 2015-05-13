local function bind(f,...)
   local args = {...}
   return function(...)
      return f(unpack(args),...)
   end
end

local centerX = 320
local centerY = 240

local radius = 200
local theta = 0

local test09
test09 = {
   onLoad = function ()
      math.randomseed(os.time())
   end,

   main = function ()
      setPosition(320, 240)

      for i = 0, 360, 4 do
         trapi = bind(test09.trap, i)
         launch(1, 0, 0, trapi)
      end

      setFunction(test09.increment)
   end,

   increment = function ()
      theta = theta + 0.015
   end,

   trap = function (offset)
      local x = centerX + radius * math.cos(theta + math.rad(offset))
      local y = centerY + radius * math.sin(theta + math.rad(offset))

      setPosition(x, y)
      setDirection(math.deg(theta + math.rad(offset) + math.pi))

      if (math.random(0, 600) == 0) then
         launchAtTarget(1, 4, test09.shoot)
      end
   end,

   shoot = function ()
      thisX, thisY = getPosition()

      dx = thisX - centerX
      dy = thisY - centerY

      -- Vanish when bullet leaves trapping circle. Thank you, Pythagoras.
      -- We should test a tiny bit past the bounds of our circle because on some frames,
      -- on launch, this bullet will immediately be within this boundary.
      radiusError = 8
      if (dx*dx + dy*dy > radius*radius+radiusError) then
         vanish()
      end
   end
}

return test09
