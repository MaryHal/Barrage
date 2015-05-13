local theta = 0
local prevTheta = 0
local radius = 100

-- Time in frames
local reactivateTime = 50
local travelTime = 4
local freezeTime = 60

function randFloatRange(a, b)
   return a + math.random() * (b - a)
end

local test11
test11 = {
   onLoad = function ()
      math.randomseed(os.time())
   end,

   main = function ()
      if (math.fmod(getTurn() + 10, reactivateTime) == 0) then
         theta = randFloatRange(prevTheta + 1.6, prevTheta + 6.28 - 1.6)
         prevTheta = theta
         for i = 0, 40 do
            launch(1, 0, 0, test11.surround)
         end
      end
   end,

   surround = function ()
      -- setCollision(false)
      tx, ty = getTargetPosition()

      nx = tx + radius * math.sin(theta)
      ny = ty + radius * math.cos(theta)
      theta = theta + 0.04

      linearInterpolate(nx, ny, travelTime)
      setFunction(test11.freeze)
   end,

   freeze = function ()
      if (getTurn() == travelTime) then
         -- setCollision(true)
         setSpeed(0)
         aimAtTarget()
         setFunction(test11.blast)
      end
   end,

   blast = function ()
      local turn = getTurn()
      if (turn == freezeTime) then
         setSpeed(8)
      elseif (turn == 120) then
         vanish()
      end
   end
}
return test11
