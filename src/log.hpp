#pragma once

#include "common.hpp"

struct lua_State;

namespace GenomeScript {

class log {
public:

    template<typename... TArgs> static void trace(TArgs... args) {
        if (m_devModeEnabled) {
            init();
            spdlog::trace(args...);
        }
    }

    template <typename... TArgs> static void info(TArgs... args) {
        if (m_devModeEnabled) {
            init();
            spdlog::info(args...);
        }
    }

    template <typename... TArgs> static void debug(TArgs... args) {
        if (m_devModeEnabled) {
            init();
            spdlog::debug(args...);
        }
    }

    template <typename... TArgs> static void warn(TArgs... args) {
        if (m_devModeEnabled) {
            init();
            spdlog::warn(args...);
        }
    }

    template <typename... TArgs> static void error(TArgs... args) {
        if (m_devModeEnabled) {
            init();
            spdlog::error(args...);
        }
    }

    static void enableDevMode() {
        m_devModeEnabled = true;
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    }

    static bool devModeEnabled() {
        m_devModeEnabled = true;
    }

    static void defineLuaTypes(lua_State* state, const std::string& moduleName);

private:
    log() = default;

    static void init() {
        if (!m_logger) {
            m_logger = spdlog::basic_logger_mt("G3", "logs/GenomeScript-last.txt", true);
        }
    }

    inline static bool m_devModeEnabled = false;
    inline static std::shared_ptr<spdlog::logger> m_logger;
};

} // namespace GenomeScript
