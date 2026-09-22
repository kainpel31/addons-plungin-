#pragma once

// Pure keycode -> label mapping. No RE/SKSE/Windows/STL-file types: safe for
// the Catch2 unit test target and usable from game code via KeyLabel.cpp.

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace MMOHotbar {

// Device ids mirror STB::HotkeySystem::Device (keyboard 0, mouse 1, gamepad 2)
// without including the STB header here, so tests stay dependency-free.
enum class HotbarDevice : std::uint32_t
{
	kKeyboard = 0,
	kMouse = 1,
	kGamepad = 2
};

struct KeyLabelResult
{
	std::string              label;           // "1", "Q", "Shift", "Mouse4", "LB"
	std::vector<std::string> candidateStems;  // icon keycap stems, e.g. {"1"}, {"L-Shift"}
};

// Single lookup: label + the icon stems that KeyLabel.cpp documents in
// docs/icon-names.md (keyboard stems follow the ImGuiIcons naming used by the
// keycap pack; mouse/gamepad stems follow the same pack naming).
[[nodiscard]] KeyLabelResult DescribeKey(HotbarDevice a_device, std::uint32_t a_code);

// "1..0" -> slot 1..10, everything else -> -1. Default mapping only covers the
// digit row (M4 may add remapping later); slots 11-12 stay manual.
[[nodiscard]] int DigitSlotIndex(HotbarDevice a_device, std::uint32_t a_code) noexcept;

}  // namespace MMOHotbar
