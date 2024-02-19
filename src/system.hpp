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
    std::pair<HookAction, std::optional<luabridge::LuaRef>> callFunctionInAllScripts(const std::string& function, TArgs... args) {
        //log::trace("Calling lua function {} in all scripts", function);
        HookAction hookAction = HookAction::None;
        std::optional<luabridge::LuaRef> result;
        for (auto& script : m_scripts) {
            ExecLuaCode(script, "ClearPreventDefaultHook()");
            std::optional<luabridge::LuaRef> luaRef = callLuaFunction(script, function, args...);
            luabridge::LuaRef hookActionRef = luabridge::getGlobal(script.luaState, "Hook")["action"];
            if (!hookActionRef.isInstance<int>()) {
                log::error("The value of HookAction.action is not an int");
                continue;
            }
            switch (hookActionRef.cast<int>().value()) {
            case HookAction::None:
                break;
            case HookAction::PreventDefaultAsSuccess:
                hookAction = HookAction::PreventDefaultAsSuccess;
                //log::warn("Script {} raised PreventDefaultAsSuccess in {}", script.moduleName, function);
                break;
            case HookAction::PreventDefaultAsFailure:
                hookAction = HookAction::PreventDefaultAsFailure;
                //log::warn("Script {} raised PreventDefaultAsFailure in {}", script.moduleName, function);
                break;
            case HookAction::PreventDefaultWithValue:
                hookAction = HookAction::PreventDefaultWithValue;
                result = luaRef;
                //log::warn("Script {} raised PreventDefaultWithValue in {}", script.moduleName, function);
                break;
            }
        }
        return { hookAction, result };
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
};

void updateScriptHotreload();

} // namespace GenomeScript
