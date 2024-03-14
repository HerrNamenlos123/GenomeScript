#pragma once

#include "pch.hpp"

// Lua
extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}
#include <LuaBridge/LuaBridge.h>

namespace GenomeScript {

constexpr auto HOTRELOAD_INTERVAL_S = 1;

std::wstring widen(const std::string& str);
std::string narrow(const std::wstring& str);
double time();

enum class HookAction { None, PreventDefaultAsSuccess, PreventDefaultAsFailure, PreventDefaultWithValue };

class ScriptMaster {
public:
    using HookResult = std::pair<HookAction, std::optional<luabridge::LuaRef>>;

    static ScriptMaster& get() {
        static ScriptMaster master;
        return master;
    }

    struct Script {
        std::filesystem::path filepath;
        std::string moduleName;
        lua_State* luaState {};
        std::filesystem::file_time_type lastWriteTime;
        std::unordered_map<std::string, luabridge::LuaRef> functionPool;
    };

    void loadAllScripts();
    void unloadAllScripts();
    void loadScript(const std::string& filename, const std::string& moduleName);
    void loadLuaFile(Script& script);
    bool unloadScript(const std::string& moduleName);

    template<typename... TArgs> 
    HookResult callFunctionInAllScripts(const std::string& function, TArgs... args) {
        // log::trace("Calling lua function {}", function);
        m_hookActionStack.emplace_back();
        for (auto& script : m_scripts) {
            callLuaFunction(script, function, args...);
        }
        // log::trace("Calling lua function {} ... Done", function);
        auto result = m_hookActionStack.back();
        m_hookActionStack.pop_back();
        return result;
    }

    template <typename... TArgs> 
    std::optional<luabridge::LuaRef> callLuaFunction(Script& script, const std::string& function, TArgs... args) {

        luabridge::LuaRef const func = luabridge::getGlobal(script.luaState, function.c_str());
        if (!func.isFunction()) {
            return {};
        }

        luabridge::LuaResult const result = func(args...);
        if (!result) {
            log::error("{}: {}() returned an error: {}", script.filepath.u8string(), function, result.errorMessage());
        }

        if (result.size() > 0) {
            return result[0];
        }
        return {};
    }

    void ExecLuaCode(const Script& script, const std::string& code);
    void defineLuaTypes(const Script& script);
    void updateScriptHotreload();

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

    std::vector<HookResult> m_hookActionStack;
};

void updateScriptHotreload();

} // namespace GenomeScript
