
function onLoad()
   math.randomseed(os.time())
end

function bind(f,...)
   local args = {...}
   return function(...)
      return f(unpack(args),...)
   end
end

centerX = 320
centerY = 240

radius = 200
theta = 0

function trap(offset)
   local x = centerX + radius * math.cos(theta + math.rad(offset))
   local y = centerY + radius * math.sin(theta + math.rad(offset))

   setPosition(x, y)

   if (math.random(0, 600) == 0) then
      launchAtTarget(4, shoot)
   end
end

function shoot()
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

function main()
   setPosition(320, 240)

   for i = 1, 360, 4 do
      trapi = bind(trap, i)
      launch(0, 0, trapi)
   end

   setFunction(increment)
end

function increment()
   theta = theta + 0.015
end
