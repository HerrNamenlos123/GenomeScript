#pragma once

#include "pch.hpp"

struct lua_State;

namespace GenomeScript {

class log {
public:

    template<typename... TArgs> static void trace(TArgs... args) {
        if (m_devModeEnabled) {
            init();
            spdlog::trace(args...);
            m_logger->trace(args...);
        }
    }

    template <typename... TArgs> static void info(TArgs... args) {
        if (m_devModeEnabled) {
            init();
            spdlog::info(args...);
            m_logger->info(args...);
        }
    }

    template <typename... TArgs> static void debug(TArgs... args) {
        if (m_devModeEnabled) {
            init();
            spdlog::debug(args...);
            m_logger->debug(args...);
        }
    }

    template <typename... TArgs> static void warn(TArgs... args) {
        if (m_devModeEnabled) {
            init();
            spdlog::warn(args...);
            m_logger->warn(args...);
        }
    }

    template <typename... TArgs> static void error(TArgs... args) {
        if (m_devModeEnabled) {
            init();
            spdlog::error(args...);
            m_logger->error(args...);
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

    static void defineLuaTypes(lua_State* state, std::string moduleName);

private:
    log() = default;

    static void init() {
        if (!m_logger) {
            m_logger = spdlog::basic_logger_mt("G3", "logs/GenomeScript-last.txt", true);
            m_logger->set_level(spdlog::level::trace);
            spdlog::set_level(spdlog::level::trace);
        }
    }

    inline static bool m_devModeEnabled = false;
    inline static std::shared_ptr<spdlog::logger> m_logger;
};

} // namespace GenomeScript
