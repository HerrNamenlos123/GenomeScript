# GenomeScript
A modkit for Gothic 3, implementing Hooks and Lua scripting

This is just an experiment and not for public use yet!

Build:  
```bash
cmake . -B build
cmake --build build --config=release --target=GenomeScript
```

Once the project is built and installed into the Gothic installation directory (hard-coded in CMakeLists.txt),
you must inject the DLL:  
Copy `setdll.exe`, `GenomeScriptInstall.bat` and `GenomeScriptUninstall.bat` to the Gothic directory 
and double-click `GenomeScriptInstall.bat`. It injects the GenomeScript DLL to `NxCooking.dll`, because it is
sufficient and probably nobody else does this, hence making it compatible with any other mod.

When installed, GenomeScript will load all mods installed in `./GenomeScript/*/`.

Take a look at the examples folder. The folders in `examples` must be copied to `./GenomeScript/`. Then they are automatically loaded on startup, in alphabetical order.

There are following conventions:  
 - Hooked functions contain the class and function name. 
 E.g. to override `Music::TriggerFight()`, define 
 ```lua
function Music_TriggerFight(this)
    log.info("hi")
end
 ```
 - If it is a member function, add `this` as the first parameter.
 - For static functions, omit it.
 - You can call original member functions with `this:TriggerFight()`. `:` is equivalent to C++ `.`
 - You can call static member functions everywhere with `Class.StaticMember()`. `.` is equivalent to C++ `::`
 - Use `PreventDefault*()` to change the default behaviour of a hook (see examples)
 - Loops and arrays start at index 1 (!!!)

Example:
The function is `Music::TriggerFight()`.
```lua  
function Music_TriggerFight(this)   -- Simply define this to get the call ('this' is the 'Music' object)
    PreventDefaultSuccess()         -- Call this to return early after the script, without calling the original
    PreventDefaultFailure()         -- The last call across all scripts defines what is done
                                    -- To keep mods as compatible as possible, prevent as little defaults as possible and only add hooks
    PreventDefaultWithValue()       -- Return early, but with the return value from this hook
    return this:TriggerVictory()    -- We call another function instead of the default, and return the return value instead
end
```
This function would replace the combat music with the victory music. 

You can add more hooks by modifying `hooks.cpp`. The goal is to eventually support all hooks someone would possibly want, so that any mod could at least in theory be written in Lua. Do not forget to call `::detour()` AND `::defineLuaTypes()` at the bottom of the file!!!

To add a hook you will need the full function signature. You can get them directly from Ghidra or another decompiler, or alternatively here: (https://github.com/HerrNamenlos123/G3SDK/tree/main/extra-resources)