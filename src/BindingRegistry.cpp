// BindingRegistry: merge rules for the 3-layer STB snapshot.
#include "BindingRegistry.h"

namespace MMOHotbar {

BindingRegistry::RebuildStats BindingRegistry::Rebuild(
	const std::vector<BindingEntry>& a_reported)
{
	RebuildStats stats;
	std::unordered_map<RowKey, BindingEntry, RowKeyHash> next;
	next.reserve(a_reported.size() + m_rows.size());

	for (const auto& in : a_reported) {
		RowKey key{ in.item, in.chord };
		auto it = m_rows.find(key);
		BindingEntry row = in;
		if (it != m_rows.end()) {
			// Keep UI state the sweep cannot see.
			row.pressedSeen = it->second.pressedSeen || in.pressedSeen;
			row.pinned = it->second.pinned;
			if (row.name.empty()) {
				row.name = it->second.name;
			}
		} else {
			++stats.added;
		}
		row.stale = false;
		next.emplace(key, std::move(row));
	}

	for (const auto& [key, old] : m_rows) {
		if (next.find(key) != next.end()) {
			continue;
		}
		if (old.pinned) {
			BindingEntry kept = old;
			kept.stale = true;
			next.emplace(key, std::move(kept));
		} else {
			++stats.removed;
		}
	}

	m_rows = std::move(next);
	return stats;
}

bool BindingRegistry::NotePressed(const ChordKey& a_chord,
                                  const std::vector<ItemKey>& a_items,
                                  std::vector<BindingEntry>* a_out)
{
	if (a_out && a_out->capacity() - a_out->size() < a_items.size()) {
		return false;  // caller grows the buffer and retries (STB Resolve rule)
	}
	for (const auto& item : a_items) {
		RowKey key{ item, a_chord };
		auto it = m_rows.find(key);
		if (it == m_rows.end()) {
			BindingEntry row;
			row.item = item;
			row.chord = a_chord;
			row.pressedSeen = true;
			it = m_rows.emplace(key, std::move(row)).first;
		} else {
			it->second.pressedSeen = true;
			it->second.stale = false;
		}
		if (a_out) {
			a_out->push_back(it->second);
		}
	}
	return true;
}

void BindingRegistry::SetPinned(const ItemKey& a_item, bool a_pinned)
{
	for (auto& [key, row] : m_rows) {
		if (key.item == a_item) {
			row.pinned = a_pinned;
			if (!a_pinned) {
				row.stale = false;
			}
		}
	}
}

void BindingRegistry::SetName(const ItemKey& a_item, const std::string& a_name)
{
	for (auto& [key, row] : m_rows) {
		if (key.item == a_item && row.name.empty()) {
			row.name = a_name;
		}
	}
}

std::vector<BindingEntry> BindingRegistry::Snapshot() const
{
	std::vector<BindingEntry> out;
	out.reserve(m_rows.size());
	for (const auto& [key, row] : m_rows) {
		out.push_back(row);
	}
	return out;
}

}  // namespace MMOHotbar
