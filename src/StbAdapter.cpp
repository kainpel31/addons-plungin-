// StbAdapter: game-side glue between the STB v1 API and BindingRegistry.
//
// Only compiled into the plugin target (needs RE/SKSE via PCH). The STB
// header is standalone: no CommonLib or STL types cross the DLL boundary.

#include "PCH.h"

// Real Win32 is allowed here: PCH.h already included REX/W32/BASE.h first,
// which is exactly the order its C1189 guard demands ("move Windows API
// includes after CommonLib"). NOMINMAX keeps the min/max macros out of the
// STL code used below.
#define NOMINMAX
#include <Windows.h>

#include "StbAdapter.h"

#include "BindingRegistry.h"
#include "STB_HotkeySystemAPI.h"

#include <vector>

namespace MMOHotbar {
namespace {

using HKS = STB::HotkeySystem::IVersion1;

[[nodiscard]] const HKS* AsApi(const void* a_api) noexcept
{
	return static_cast<const HKS*>(a_api);
}

[[nodiscard]] std::uint32_t ToDeviceInt(STB::HotkeySystem::Device a_d) noexcept
{
	return static_cast<std::uint32_t>(a_d);
}

void AppendResolved(const HKS* a_api, STB::HotkeySystem::Device a_device,
                    std::uint32_t a_key, std::vector<BindingEntry>& a_out)
{
	std::vector<STB::HotkeySystem::Binding> buf(16);
	for (;;) {
		const std::uint32_t total = a_api->Resolve(a_device, a_key, buf.data(),
		                                           static_cast<std::uint32_t>(buf.size()));
		if (total > buf.size()) {
			buf.resize(total);  // Resolve reports the true count; retry bigger
			continue;
		}
		for (std::uint32_t i = 0; i < total; ++i) {
			const auto& b = buf[i];
			BindingEntry e;
			e.item.form = b.form;
			e.item.ench = b.ench;
			e.item.health = b.health;
			e.chord.device = ToDeviceInt(a_device);
			e.chord.keyCount = 1;
			e.chord.keys = { a_key, 0 };
			a_out.push_back(e);
		}
		return;
	}
}

}  // namespace

const void* ResolveStbApi() noexcept
{
	const auto mod = GetModuleHandleA(STB::HotkeySystem::kModuleName);
	if (!mod) {
		return nullptr;
	}
	const auto req = reinterpret_cast<STB::HotkeySystem::RequestAPI_t>(
		GetProcAddress(mod, STB::HotkeySystem::kRequestFunction));
	if (!req) {
		return nullptr;
	}
	return req(1);  // version 1; null = absent, too old, or refused
}

std::vector<BindingEntry> SweepSingleKeys(const void* a_api)
{
	std::vector<BindingEntry> out;
	const HKS* api = AsApi(a_api);
	if (!api) {
		return out;
	}
	// Keyboard scancodes 0x00-0xFF, mouse 0-15, gamepad 0-0xFF. Single-key only:
	// chords surface later via layer 2 (passive capture on key-down).
	for (std::uint32_t k = 0; k < 0x100; ++k) {
		AppendResolved(api, STB::HotkeySystem::Device::kKeyboard, k, out);
	}
	for (std::uint32_t k = 0; k < 16; ++k) {
		AppendResolved(api, STB::HotkeySystem::Device::kMouse, k, out);
	}
	for (std::uint32_t k = 0; k < 0x100; ++k) {
		AppendResolved(api, STB::HotkeySystem::Device::kGamepad, k, out);
	}
	return out;
}

bool HasChord(const void* a_api, const ItemKey& a_item) noexcept
{
	const HKS* api = AsApi(a_api);
	if (!api) {
		return false;
	}
	STB::HotkeySystem::Binding b{};
	b.form = a_item.form;
	b.ench = a_item.ench;
	b.health = a_item.health;
	// hands ignored by the API on purpose.
	const auto chord = api->GetHotkeyExact(b);
	return chord.keyCount != 0;
}

std::vector<ItemKey> ResolvePress(const void* a_api, const ChordKey& a_chord,
                                  std::uint32_t a_pressedKey)
{
	std::vector<ItemKey> out;
	const HKS* api = AsApi(a_api);
	if (!api || a_chord.keyCount == 0) {
		return out;
	}
	const auto device = static_cast<STB::HotkeySystem::Device>(a_chord.device);
	std::vector<STB::HotkeySystem::Binding> buf(16);
	for (;;) {
		const std::uint32_t total = api->Resolve(device, a_pressedKey, buf.data(),
		                                         static_cast<std::uint32_t>(buf.size()));
		if (total > buf.size()) {
			buf.resize(total);
			continue;
		}
		out.reserve(total);
		for (std::uint32_t i = 0; i < total; ++i) {
			ItemKey k;
			k.form = buf[i].form;
			k.ench = buf[i].ench;
			k.health = buf[i].health;
			out.push_back(k);
		}
		return out;
	}
}

}  // namespace MMOHotbar
