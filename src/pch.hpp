#pragma once

#ifdef _WIN64
#error This project can only be built in 32-bit mode!
#endif

#define WIN32_LEAN_AND_MEAN

#ifndef SPDLOG_COMPILED_LIB
#define SPDLOG_COMPILED_LIB
#endif

#include <Windows.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <detours.h>

#define GS_API __declspec(dllexport)

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <codecvt>
#include <chrono>

// gothic3sdk
#include "Engine.h"
#include "Script.h"
#include "Game.h"