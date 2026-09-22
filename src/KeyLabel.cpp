// KeyLabel: DX scancode / mouse / gamepad code -> short label + icon stems.
//
// Scancode values are the DirectInput set the engine reports for keyboards.
// Only the digit row maps to slots (DigitSlotIndex); every other key still
// gets a label so the HUD never shows a blank badge.

#include "KeyLabel.h"

#include <array>
#include <cstdio>

namespace MMOHotbar {
namespace {

struct KeyEntry
{
	std::uint32_t code;
	std::string_view label;
	std::string_view stem;
};

// Digit row + letters + function keys + common controls. Stems follow the
// ImGuiIcons keycap pack naming so TextureCache can reuse them later.
constexpr KeyEntry kKeyboard[]{
	{ 0x01, "Esc", "Esc" },
	{ 0x02, "1", "1" },
	{ 0x03, "2", "2" },
	{ 0x04, "3", "3" },
	{ 0x05, "4", "4" },
	{ 0x06, "5", "5" },
	{ 0x07, "6", "6" },
	{ 0x08, "7", "7" },
	{ 0x09, "8", "8" },
	{ 0x0A, "9", "9" },
	{ 0x0B, "0", "0" },
	{ 0x0C, "-", "Hyphen" },
	{ 0x0D, "=", "Equal" },
	{ 0x0E, "Back", "Backspace" },
	{ 0x0F, "Tab", "Tab" },
	{ 0x10, "Q", "Q" },
	{ 0x11, "W", "W" },
	{ 0x12, "E", "E" },
	{ 0x13, "R", "R" },
	{ 0x14, "T", "T" },
	{ 0x15, "Y", "Y" },
	{ 0x16, "U", "U" },
	{ 0x17, "I", "I" },
	{ 0x18, "O", "O" },
	{ 0x19, "P", "P" },
	{ 0x1A, "[", "Bracketleft" },
	{ 0x1B, "]", "Bracketright" },
	{ 0x1C, "Enter", "Enter" },
	{ 0x1D, "Ctrl", "L-Ctrl" },
	{ 0x1E, "A", "A" },
	{ 0x1F, "S", "S" },
	{ 0x20, "D", "D" },
	{ 0x21, "F", "F" },
	{ 0x22, "G", "G" },
	{ 0x23, "H", "H" },
	{ 0x24, "J", "J" },
	{ 0x25, "K", "K" },
	{ 0x26, "L", "L" },
	{ 0x27, ";", "Semicolon" },
	{ 0x28, "'", "Quotesingle" },
	{ 0x29, "`", "Tilde" },
	{ 0x2A, "Shift", "L-Shift" },
	{ 0x2B, "\\", "Backslash" },
	{ 0x2C, "Z", "Z" },
	{ 0x2D, "X", "X" },
	{ 0x2E, "C", "C" },
	{ 0x2F, "V", "V" },
	{ 0x30, "B", "B" },
	{ 0x31, "N", "N" },
	{ 0x32, "M", "M" },
	{ 0x33, ",", "Comma" },
	{ 0x34, ".", "Period" },
	{ 0x35, "/", "Slash" },
	{ 0x36, "Shift", "R-Shift" },
	{ 0x38, "Alt", "L-Alt" },
	{ 0x39, "Space", "Space" },
	{ 0x3A, "Caps", "CapsLock" },
	{ 0x3B, "F1", "F1" },
	{ 0x3C, "F2", "F2" },
	{ 0x3D, "F3", "F3" },
	{ 0x3E, "F4", "F4" },
	{ 0x3F, "F5", "F5" },
	{ 0x40, "F6", "F6" },
	{ 0x41, "F7", "F7" },
	{ 0x42, "F8", "F8" },
	{ 0x43, "F9", "F9" },
	{ 0x44, "F10", "F10" },
	{ 0x45, "Num", "NumLock" },
	{ 0x47, "Num7", "NumPad7" },
	{ 0x48, "Num8", "NumPad8" },
	{ 0x49, "Num9", "NumPad9" },
	{ 0x4A, "Num-", "NumPadMinus" },
	{ 0x4B, "Num4", "NumPad4" },
	{ 0x4C, "Num5", "NumPad5" },
	{ 0x4D, "Num6", "NumPad6" },
	{ 0x4E, "Num+", "NumPadPlus" },
	{ 0x4F, "Num1", "NumPad1" },
	{ 0x50, "Num2", "NumPad2" },
	{ 0x51, "Num3", "NumPad3" },
	{ 0x52, "Num0", "NumPad0" },
	{ 0x57, "F11", "F11" },
	{ 0x58, "F12", "F12" }
};

constexpr KeyEntry kMouse[]{
	{ 0, "M1", "Mouse1" },
	{ 1, "M2", "Mouse2" },
	{ 2, "M3", "Mouse3" },
	{ 3, "M4", "Mouse4" },
	{ 4, "M5", "Mouse5" },
	{ 5, "M6", "Mouse6" },
	{ 6, "M7", "Mouse7" },
	{ 7, "M8", "Mouse8" }
};

// Provisional XInput-style ids; STB stores device-native codes sorted
// ascending and Resolve() reports what it stored, so this table only affects
// the label, never the binding identity.
constexpr KeyEntry kGamepad[]{
	{ 0, "A", "360_A" },
	{ 1, "B", "360_B" },
	{ 2, "X", "360_X" },
	{ 3, "Y", "360_Y" },
	{ 4, "LB", "360_LB" },
	{ 5, "RB", "360_RB" },
	{ 6, "LT", "360_LT" },
	{ 7, "RT", "360_RT" },
	{ 8, "Back", "360_Back" },
	{ 9, "Start", "360_Start" },
	{ 10, "LS", "360_LS" },
	{ 11, "RS", "360_RS" },
	{ 12, "Up", "360_Up" },
	{ 13, "Down", "360_Down" },
	{ 14, "Left", "360_Left" },
	{ 15, "Right", "360_Right" }
};

template <std::size_t N>
KeyLabelResult Lookup(const KeyEntry (&a_table)[N], std::uint32_t a_code,
                      std::string_view a_prefix)
{
	for (const auto& entry : a_table) {
		if (entry.code == a_code) {
			return KeyLabelResult{ std::string(entry.label),
				                   std::vector<std::string>{ std::string(entry.stem) } };
		}
	}
	char fallback[32];
	std::snprintf(fallback, sizeof(fallback), "%.*s%02X",
	              static_cast<int>(a_prefix.size()), a_prefix.data(), a_code & 0xFF);
	return KeyLabelResult{ fallback, { "UnknownKey" } };
}

}  // namespace

KeyLabelResult DescribeKey(HotbarDevice a_device, std::uint32_t a_code)
{
	switch (a_device) {
	case HotbarDevice::kKeyboard:
		return Lookup(kKeyboard, a_code, "Key");
	case HotbarDevice::kMouse:
		return Lookup(kMouse, a_code, "Mouse");
	case HotbarDevice::kGamepad:
		return Lookup(kGamepad, a_code, "Pad");
	default:
		return KeyLabelResult{ "?", { "UnknownKey" } };
	}
}

int DigitSlotIndex(HotbarDevice a_device, std::uint32_t a_code) noexcept
{
	// DX scancodes 0x02..0x0B are the "1..9,0" row -> slots 0..9.
	if (a_device != HotbarDevice::kKeyboard) {
		return -1;
	}
	if (a_code >= 0x02 && a_code <= 0x0B) {
		return static_cast<int>(a_code - 0x02);  // 0x02->0 ... 0x0B->9
	}
	return -1;
}

}  // namespace MMOHotbar
