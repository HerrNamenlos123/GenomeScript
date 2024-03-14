# GenomeScript
A modkit for Gothic 3, implementing Hooks and Lua scripting

This is just an experiment and not for public use yet!

Build:  
```bash
cmake . -B build
cmake --build build
```

Once the project is built and installed into the Gothic installation directory (hard-coded in CMakeLists.txt),
you must inject the DLL:  
Copy `setdll.exe`, `GenomeScriptInstall.bat` and `GenomeScriptUninstall.bat` to the Gothic directory 
and double-click `GenomeScriptInstall.bat`. It injects the GenomeScript DLL to `NxCooking.dll`, because it is
sufficient and probably nobody else does this, hence making it compatible with any other mod.

When installed, GenomeScript will load all mods installed in `./GenomeScript/*/`.

Take a look at the examples folder. The folders in `examples` must be copied to `./GenomeScript/`. Then they are automatically loaded on startup, in alphabetical order.