
#include "system.hpp"
#include "log.hpp"
#include <filesystem>
#include <fstream>
#include <codecvt>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}
#include <LuaBridge/LuaBridge.h>

namespace GenomeScript {

void CreateConsole() {
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    log::consoleEnabled();
}

std::wstring widen(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

std::string narrow(const std::wstring& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(str);
}

ScriptMaster::~ScriptMaster() {
    unloadAllScripts();
}

void ScriptMaster::loadAllScripts() {
    const std::string scriptDirectory = "GenomeScript/";

    // We loop through all folders in the script directory
    using iter = std::filesystem::directory_iterator;
    for (const auto& dirEntry : iter(scriptDirectory)) {
        // In every folder, we call the main script
        const auto& mainFile = dirEntry / "main.lua";
        loadScript(mainFile.u8string());
    }
}

void ScriptMaster::unloadAllScripts() {
    while (m_scripts.size() > 0) {
        auto [path, state] = m_scripts.back();
        callLuaFunction(state, m_luaOnDetachFunction);
        m_scripts.pop_back();
    }
}

void ScriptMaster::loadScript(const std::string& filename) {

    std::ifstream file(widen(filename).c_str(), std::ios::in);
    if (file.fail()) {
        log::error("Failed to load script '{}'", filename);
        return;
    }

    lua_State* state = luaL_newstate();
    luaL_openlibs(state);
    defineTypes(state);

    std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    ExecLuaCode(state, code);
    
    callLuaFunction(state, m_luaOnAttachFunction);
    m_scripts.emplace_back(std::make_pair(filename, state));
}

bool ScriptMaster::unloadScript(const std::string& filename) {
    for (int i = 0; i < m_scripts.size(); i++) {
        auto [path, state] = m_scripts[i];
        if (path == filename) {
            callLuaFunction(state, m_luaOnDetachFunction);
            m_scripts.erase(m_scripts.begin() + i);
            return true;
        }
    }
    return false;
}

bool ScriptMaster::callFunctionInAllScripts(const std::string& function) {
    for (auto [path, state] : m_scripts) {
        luabridge::LuaRef const func = luabridge::getGlobal(state, function.c_str());
        if (!func.isFunction()) {
            continue;
        }

        luabridge::LuaResult const result = func();
        if (!result) {
            log::error("{}: {}() returned an error: {}", path, function, result.errorMessage());
            return false;
        }
    }
    return true;
}

void ScriptMaster::ExecLuaCode(lua_State* luaState, const std::string& code) {
    if (luaL_dostring(luaState, code.c_str()) != 0) {
        log::error("{}", lua_tostring(luaState, -1));
        lua_pop(luaState, 1);
    }
}

bool ScriptMaster::callLuaFunction(lua_State* state, const std::string& function) {
    luabridge::LuaRef const func = luabridge::getGlobal(state, function.c_str());
    if (!func.isFunction()) {
        return false;
    }

    luabridge::LuaResult const result = func();
    if (!result) {
        log::error("{}() returned an error: {}", function, result.errorMessage());
        return false;
    }
    return true;
}

void ScriptMaster::defineTypes(lua_State* state) {
    
}

} // namespace GenomeScript
