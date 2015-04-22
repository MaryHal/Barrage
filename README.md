# Barrage [![Build Status](https://travis-ci.org/sanford1/Barrage.svg?branch=master)](https://travis-ci.org/sanford1/Barrage)

Lua scripting for shmup barrage patterns. Rewrite of the C++ version ([BulletLua](https://github.com/sanford1/BulletLua)).

> This C project aims to create a sane scriptable interface to define bullet patterns in shoot 'em
> up games. These patterns, while difficult to dodge in game and hard to appreciate while in the
> heat of the moment, can be beautiful to spectate.

## Building this project

Dependencies: python, [ninja](https://martine.github.io/ninja/), [lua](http://www.lua.org/) (or [luajit](http://luajit.org/))

### Grab this repository + submodules

    git clone <this-repo>
    git submodule update --init --recursive

### Generating a ninja build file

    python bootstrap.py (--debug) (--cxx=<compiler>) (--ci) (--luajit)

Arguments in parenthesis are optional. The default compiler is gcc and this project assumes [C11](https://en.wikipedia.org/wiki/C11_(C_standard_revision)) support. The `ci` switch links `lua5.2` instead of `lua` if your system does not default to lua 5.2. The `luajit` switch links luajit in place of lua.

The bootstrap script is python2/3 compatible.

### Building the library and unit test

Simply run `ninja`

## Usage

Currently you can access the C library directly or use Lua wrapper functions. More wrappers once this project is more stable.

### C

#### Setup

Link libbarrage.so and make sure your compiler can find the correct header files in the `include` directory.

#### Example (pseudo-)code

    #include <barrage/Barrage.h>

    struct Barrage* barrage = br_createBarrageFromFile(my_file, 320.0f, 120.0f);

    while (running)
    {
        // Update
        br_tick(barrage);

        // Draw loop
        while (br_hasNext(barrage))
        {
            struct Bullet* b = br_yield(barrage);
            drawImage(b->x, b->y);
        }
    }

    // Cleanup
    br_deleteBarrage(barrage);

### Lua

#### Importing the shared library

First things first is that lua needs to be able to find libbarrage.so. Whether that means installing the shared library to a system directory or telling lua where it can find the file is up to you. For example, when running your script you can direct the lua executable to find the shared library like this:

    LUA_CPATH="../lib/libbarrage.so" lua myFile.lua

Keep in mind the lua executable version should match the lua version linked to build this project. This matters if you are using something like [LÖVE](https://love2d.org/), which uses luajit internally. To use Barrage with LÖVE you have to build libbarrage.so with luajit instead of lua.

#### Example (pseudo-)code:

    -- Allow lua to access the exposed API in libbarrage.so.
    local barrage = require "barrageC"

    local myBarrage = nil

    function load(arg)
        -- Load a barrage script from a file.
        myBarrage = barrage.new("path/to/file.lua", 320.0, 120.0)
    end

    function update(dt)
        local x, y = getMousePosition()
        myBarrage:setPlayerPosition(x, y)

        -- Update barrage bullets.
        myBarrage:tick()
    end

    function draw(dt)
        while myBarrage:hasNext() do
            local x, y = myBarrage:yield()

            -- Draw our bullet. Note that the center of the bullet is at (x, y).
            draw(image, x, y, 0, 0.5, 0.5)
        end
    end

## Barrage Scripting

### Basics

When you create a barrage, it automatically creates a `root` bullet that is linked to a `main` lua function. The capabilities of all bullets is that they can control themselves and they can launch new bullets.

    function main()
        -- Do things
    end

### Available bullet functions

#### Do nothing

    nullfunc()

#### Position functions

    -- Set Bullet Position.
    setPosition(float x, float y)

    -- Get Position.
    x, y = getPosition()

    -- Get Target Position
    tx, ty = getTargetPosition()

#### Velocity functions

    -- Set Bullet Velocity.
    setVelocity(float vx, float vy)

    -- Get Velocity Components.
    vx, vy = getVelocity()

    -- Bullet Direction
    setDirection(float dir)
    setDirectionRelative(float dir)
    getDirection()

    -- Bullet Speed
    setSpeed(float s)
    setSpeedRelative(float s)
    getSpeed()

    -- Set the current bullet to aim at the "player"
    aimAtTarget()

    -- Set the current bullet to aim at a point
    aimAtPoint(float x, float y)

    -- Set speed and direction of bullet such that it will reach (x, y) in 'n' steps (ticks).
    linearInterpolate(float x, float y, unsigned int n)

#### Frame counting

    -- Get the amount of frames since this bullet's creation.
    getTurn()

    -- Reset this bullet's frame counter
    resetTurns()

#### Barrage difficulty

    -- Get the current barrage "difficulty", From [0.0, 1.0].
    getRank()

#### Random numbers (currently missing)

    -- Generate random floating point numbers
    randFloat()
    randFloatRange(float min, float max)

    -- Generate random integers
    randInt(int max)
    randIntRange(int min, int max)

#### Switch functions + creating new bullets

    -- Switch current running function. This also resets the bullet's frame counter.
    setFunction(function)

    -- Shoot a bullet from current bullet's position moving in direction (d) at speed (s) running function (func).

    launch(float d, float s, func)

    -- Shoot a bullet and aim it at the "player"
    launchAtTarget(float s, func)

    -- Shoot (segments) number of bullets concentrically.
    launchCircle(int segments, float s, const sol::function& funcName)

#### Removing Bullets

    -- Fade out this bullet. Kill it slowly.
    vanish()

    -- Immediately destroy this bullet.
    kill()
