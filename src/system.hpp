#pragma once

#include "common.hpp"

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}
#include <LuaBridge/LuaBridge.h>

namespace GenomeScript {

std::wstring widen(const std::string& str);
std::string narrow(const std::wstring& str);

class ScriptMaster {
public:
    static ScriptMaster& get() {
        static ScriptMaster master;
        return master;
    }

    struct Script {
        std::string filepath;
        std::string moduleName;
        lua_State* luaState {};
        std::unordered_map<std::string, luabridge::LuaRef> functionPool;
    };

    void loadAllScripts();
    void unloadAllScripts();
    void loadScript(const std::string& filename, const std::string& moduleName);
    bool unloadScript(const std::string& moduleName);

    bool callFunctionInAllScripts(const std::string& function);

    void ExecLuaCode(const Script& script, const std::string& code);
    bool callLuaFunction(Script& script, const std::string& function);
    void defineLuaTypes(const Script& script);

    ScriptMaster(const ScriptMaster&) = delete;
    ScriptMaster(ScriptMaster&&) = delete;
    ScriptMaster& operator=(const ScriptMaster&) = delete;
    ScriptMaster& operator=(ScriptMaster&&) = delete;

private:
    ScriptMaster() = default;
    ~ScriptMaster();

    inline static bool m_devModeEnabled = false;
    inline static const std::string m_luaOnAttachFunction = "OnAttach";
    inline static const std::string m_luaOnDetachFunction = "OnDetach";
    std::vector<Script> m_scripts;
};

} // namespace GenomeScript
