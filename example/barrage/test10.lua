
-- BulletLua Test Script 10

local function bind(f,...)
   local args = {...}
   return function(...)
      return f(unpack(args),...)
   end
end

local test10
test10 = {
   main = function ()
      local commands = {}
      commands[30] = bind(setSpeed, 2)
      commands[60] = vanish

      setFunction(bind(test10.runTable, commands))
   end,

   runTable = function (commands)
      local turn = getTurn()
      local cmd = commands[turn]

      if cmd ~= nil then
         cmd()
      end
   end
}
return test10
