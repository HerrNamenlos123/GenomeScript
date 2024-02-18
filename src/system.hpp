#pragma once

#include "common.hpp"

struct lua_State;

namespace GenomeScript {

/// @brief Create a new console window and redirect stdout to it.
void CreateConsole();

std::wstring widen(const std::string& str);
std::string narrow(const std::wstring& str);

class ScriptMaster {
public:
    static ScriptMaster& get() {
        static ScriptMaster master;
        return master;
    }

    void loadAllScripts();
    void unloadAllScripts();
    void loadScript(const std::string& filename);
    bool unloadScript(const std::string& filename);

    bool callFunctionInAllScripts(const std::string& function);

    void ExecLuaCode(lua_State* luaState, const std::string& code);
    bool callLuaFunction(lua_State* state, const std::string& function);
    void defineTypes(lua_State* state);

    ScriptMaster(const ScriptMaster&) = delete;
    ScriptMaster(ScriptMaster&&) = delete;
    ScriptMaster& operator=(const ScriptMaster&) = delete;
    ScriptMaster& operator=(ScriptMaster&&) = delete;

private:
    ScriptMaster() = default;
    ~ScriptMaster();

    inline static const std::string m_luaOnAttachFunction = "OnAttach";
    inline static const std::string m_luaOnDetachFunction = "OnDetach";
    std::vector<std::pair<std::string, lua_State*>> m_scripts;
};

} // namespace GenomeScript
