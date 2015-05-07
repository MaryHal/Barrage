-- BulletLua Test Script 5

local function bind(f,...)
   local args = {...}
   return function(...)
      return f(unpack(args),...)
   end
end

local cycles = 0
local gwait = 20

local test05
test05 = {
   main = function ()
      circle = bind(launchCircle, 40)
      setPosition(320, 240)
      setFunction(bind(test05.go, 10))
   end,

   go = function (wait)
      local turn = getTurn()
      if (math.fmod(turn, wait) == 0) then
         circle(21 - gwait, test05.curve)

         setFunction(bind(test05.go, gwait))
         gwait = gwait - 1
         if gwait < 18 then
            gwait = 20
         end

         cycles = cycles + 1
      end
      if (cycles == 15) then
         vanish()
      end
   end,

   curve = function ()
      local turn = getTurn()
      setDirectionRelative(5)
      setSpeedRelative(0.1)

      if (turn > 180) then
         vanish()
      end
   end
}

return test05
