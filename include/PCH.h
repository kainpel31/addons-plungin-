#pragma once

// Precompiled header for the plugin target only. The unit test target deliberately does
// not include this: it must stay free of RE/SKSE types so it can run outside Skyrim.
//
// ORDER IS LOAD-BEARING (CommonLibSSE-NG REX/W32/BASE.h raises C1189 if the Windows API
// was included first): RE/SKSE first, then everything else. Never put <Windows.h> (or a
// header that pulls it in, e.g. third_party/SKSEMenuFramework.h) above the CommonLib
// block; include such headers after it, or better yet only from .cpp files.

// REX/W32/BASE.h aborts with C1189 when the real Windows API (winapifamily.h) was
// included before it, and newer MSVC STL headers can pull winapifamily.h in
// transitively. The four headers below are the only thing BASE.h itself needs
// (proven safe: CommonLibSSE's own build includes them before REX on this same
// toolchain), so include BASE.h before anything else that might touch the Windows
// API. Later includes of BASE.h are no-ops via #pragma once, so this single early
// include disarms the guard for the whole translation unit.
#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <REX/W32/BASE.h>

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

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

#include "Version.h"

namespace logger = SKSE::log;
namespace stl = SKSE::stl;

using namespace std::literals;
