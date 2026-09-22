#pragma once

// Pure binding snapshot: canonical item/chord keys + merge rules. Game code
// feeds it from the STB API; tests drive it with a fake. No RE/SKSE here.

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace MMOHotbar {

// Canonical item key: hands ignored (binding descriptor, not item identity).
struct ItemKey
{
	std::uint32_t form = 0;
	std::uint32_t ench = 0;
	std::int32_t  health = 0;

	bool operator==(const ItemKey& a_o) const noexcept
	{
		return form == a_o.form && ench == a_o.ench && health == a_o.health;
	}
};

// Canonical chord key: STB sorts keys ascending, reused verbatim.
struct ChordKey
{
	std::uint32_t device = 0;
	std::uint32_t keyCount = 0;
	std::array<std::uint32_t, 2> keys{ 0, 0 };

	bool operator==(const ChordKey& a_o) const noexcept
	{
		return device == a_o.device && keyCount == a_o.keyCount && keys == a_o.keys;
	}
};

struct BindingEntry
{
	ItemKey  item;
	ChordKey chord;
	std::string name;       // display name when known (layer 3 fills it in)
	bool pressedSeen = false;
	bool pinned = false;    // survives rebuilds that drop it from STB
	bool stale = false;     // pinned but no longer in STB -> drawn dimmed
};

struct ItemKeyHash
{
	std::size_t operator()(const ItemKey& a_k) const noexcept
	{
		std::size_t h = std::hash<std::uint32_t>{}(a_k.form);
		h ^= std::hash<std::uint32_t>{}(a_k.ench) + 0x9e3779b9u + (h << 6) + (h >> 2);
		h ^= std::hash<std::int32_t>{}(a_k.health) + 0x9e3779b9u + (h << 6) + (h >> 2);
		return h;
	}
};

struct ChordKeyHash
{
	std::size_t operator()(const ChordKey& a_k) const noexcept
	{
		std::size_t h = std::hash<std::uint32_t>{}(a_k.device);
		h ^= std::hash<std::uint32_t>{}(a_k.keyCount) + 0x9e3779b9u + (h << 6) + (h >> 2);
		h ^= std::hash<std::uint32_t>{}(a_k.keys[0]) + 0x9e3779b9u + (h << 6) + (h >> 2);
		h ^= std::hash<std::uint32_t>{}(a_k.keys[1]) + 0x9e3779b9u + (h << 6) + (h >> 2);
		return h;
	}
};

// Snapshot owned by BindingRegistry: one row per (item, chord) pair so one
// item on two chords and one chord firing N items both round-trip.
class BindingRegistry
{
public:
	// Layer 1/3 feed: full replace of what STB reports, except pinned rows.
	// Returns {added, removed}. Pinned-but-missing rows flip stale=true.
	struct RebuildStats
	{
		std::size_t added = 0;
		std::size_t removed = 0;
	};
	RebuildStats Rebuild(const std::vector<BindingEntry>& a_reported);

	// Layer 2 feed: Resolve() hit for one press; creates the row if new and
	// marks pressedSeen. Returns false when a_out would overflow (caller grows
	// the buffer and retries); group rows share the chord.
	bool NotePressed(const ChordKey& a_chord, const std::vector<ItemKey>& a_items,
	                 std::vector<BindingEntry>* a_out = nullptr);

	void SetPinned(const ItemKey& a_item, bool a_pinned);
	void SetName(const ItemKey& a_item, const std::string& a_name);

	[[nodiscard]] std::vector<BindingEntry> Snapshot() const;
	[[nodiscard]] std::size_t Size() const noexcept { return m_rows.size(); }
	[[nodiscard]] bool HasStb() const noexcept { return m_hasStb; }
	void SetHasStb(bool a_v) noexcept { m_hasStb = a_v; }

private:
	struct RowKey
	{
		ItemKey  item;
		ChordKey chord;
		bool operator==(const RowKey& a_o) const noexcept
		{
			return item == a_o.item && chord == a_o.chord;
		}
	};
	struct RowKeyHash
	{
		std::size_t operator()(const RowKey& a_k) const noexcept
		{
			return ItemKeyHash{}(a_k.item) ^ (ChordKeyHash{}(a_k.chord) << 1);
		}
	};

	std::unordered_map<RowKey, BindingEntry, RowKeyHash> m_rows;
	bool m_hasStb = false;
};

}  // namespace MMOHotbar
