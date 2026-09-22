#pragma once

// Game-side adapter: STB API <-> pure BindingRegistry. Included only by the
// plugin target (needs RE/SKSE via PCH + third_party/STB_HotkeySystemAPI.h).
// Unit tests never see this header.

#include <cstddef>
#include <cstdint>
#include <vector>

namespace MMOHotbar {

class BindingRegistry;
struct BindingEntry;
struct ChordKey;
struct ItemKey;

// Resolve the STB v1 API. Returns nullptr when the mod is absent / too old /
// refused the version. Call from kPostLoad or later; the object is owned by
// STB and lives for the process: never delete it.
[[nodiscard]] const void* ResolveStbApi() noexcept;

// Layer 1: sweep every single-key code on all three devices through
// IVersion1::Resolve. Buffer grows on demand (Resolve reports the true count
// even when it exceeds a_max). ~600 calls, each microseconds.
std::vector<BindingEntry> SweepSingleKeys(const void* a_api);

// Layer 3 helper: exact chord for one inventory/spellbook instance. Hands are
// ignored by the API; keyCount 0 means "not bound".
[[nodiscard]] bool HasChord(const void* a_api, const ItemKey& a_item) noexcept;

// Layer 2 helper: what would pressing a_key fire right now (chord-aware).
// Returns the group; empty means unbound.
std::vector<ItemKey> ResolvePress(const void* a_api, const ChordKey& a_chord,
                                  std::uint32_t a_pressedKey);

}  // namespace MMOHotbar
