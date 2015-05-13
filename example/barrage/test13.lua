local function bind(f,...)
   local args = {...}
   return function(...)
      return f(unpack(args),...)
   end
end

local theta = 0

local test13
test13 = {
   main = function()
      setPosition(320, 240)

      launch(1,  theta + 00, 3, test13.doSomething)
      launch(2, -theta + 10, 6, test13.doSomething)

      launch(1,  theta + 120, 3, test13.doSomething)
      launch(2, -theta + 120, 6, test13.doSomething)

      launch(1,  theta + 240, 3, test13.doSomething)
      launch(2, -theta + 240, 6, test13.doSomething)

      theta = theta + 6
   end,

   doSomething = function()
      local frame = getFrameCount()
      setSpeedRelative(0.02)

      if (frame == 10) then
         vanish()
      end
   end
}

return test13
