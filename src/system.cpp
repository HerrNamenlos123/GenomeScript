
#include "system.hpp"
#include "log.hpp"
#include <filesystem>
#include <fstream>
#include <codecvt>

namespace GenomeScript {

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
    for (auto& dirEntry : iter(scriptDirectory)) {
        // In every folder, we call the main script
        const auto& mainFile = dirEntry / "main.lua";
        const auto moduleName = std::filesystem::path(dirEntry).filename();
        loadScript(mainFile.u8string(), moduleName.u8string());
    }
}

void ScriptMaster::unloadAllScripts() {
    while (m_scripts.size() > 0) {
        unloadScript(m_scripts.back().filepath);
    }
}

void ScriptMaster::loadScript(const std::string& filename, const std::string& moduleName) {

    std::ifstream file(widen(filename).c_str(), std::ios::in);
    if (file.fail()) {
        log::error("Failed to load script '{}'", filename);
        return;
    }

    Script script;
    script.luaState = luaL_newstate();
    script.filepath = filename;
    script.moduleName = moduleName;
    luaL_openlibs(script.luaState);
    defineLuaTypes(script);

    std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    ExecLuaCode(script, code);
    
    callLuaFunction(script, m_luaOnAttachFunction);
    m_scripts.emplace_back(script);
}

bool ScriptMaster::unloadScript(const std::string& moduleName) {
    for (int i = 0; i < m_scripts.size(); i++) {
        auto& script = m_scripts[i];
        if (script.moduleName == moduleName) {
            callLuaFunction(script, m_luaOnDetachFunction);
            lua_close(script.luaState);
            m_scripts.erase(m_scripts.begin() + i);
            return true;
        }
    }
    return false;
}

bool ScriptMaster::callFunctionInAllScripts(const std::string& function) {
    for (auto& script : m_scripts) {
        callLuaFunction(script, function);
    }
    return true;
}

void ScriptMaster::ExecLuaCode(const Script& script, const std::string& code) {
    if (luaL_dostring(script.luaState, code.c_str()) != 0) {
        log::error("{}: {}", script.filepath, lua_tostring(script.luaState, -1));
        lua_pop(script.luaState, 1);
    }
}

bool ScriptMaster::callLuaFunction(Script& script, const std::string& function) {

    if (script.functionPool.find(function) == script.functionPool.end()) {
        // Function is not known yet
        luabridge::LuaRef const func = luabridge::getGlobal(script.luaState, function.c_str());
        if (!func.isFunction()) {
            return false;
        }
        script.functionPool.emplace(function, func);
    }

    auto& func = script.functionPool.at(function);
    luabridge::LuaResult const result = func();
    if (!result) {
        log::error("{}: {}() returned an error: {}", script.filepath, function, result.errorMessage());
        return false;
    }
    return true;
}

void ScriptMaster::defineLuaTypes(const Script& script) {
    log::defineLuaTypes(script.luaState, script.moduleName);
}

} // namespace GenomeScript
