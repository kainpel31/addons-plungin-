#pragma once

// Precompiled header for the plugin target only. The unit test target deliberately does
// not include this: it must stay free of RE/SKSE types so it can run outside Skyrim.

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#include "Version.h"

namespace logger = SKSE::log;
namespace stl = SKSE::stl;

using namespace std::literals;
