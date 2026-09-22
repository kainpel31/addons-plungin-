#pragma once

// Pure item classification: descriptor in, category + color + icon id out.
// No RE/SKSE types so Catch2 can test it. Game code translates RE objects
// into ItemDescriptor (see ItemClassifier.cpp docs) before calling Classify.

#include <cstdint>
#include <string>

namespace MMOHotbar {

enum class ItemCategory : std::uint8_t
{
	kWeapon = 0,
	kArmorClothing,
	kSpell,
	kPower,
	kShout,
	kScroll,
	kConsumable,
	kAmmo,
	kSoulGem,
	kTorch,
	kMisc,
	kUnknown
};

enum class SpellSchool : std::uint8_t
{
	kNone = 0,
	kAlteration,
	kConjuration,
	kDestruction,
	kIllusion,
	kRestoration
};

enum class SpellResist : std::uint8_t
{
	kNone = 0,
	kFire,
	kFrost,
	kShock
};

// Plain descriptor; game adapter fills it from TESForm subclasses.
struct ItemDescriptor
{
	std::uint32_t formType = 0;  // RE::FormType numeric value, 0 = unknown
	int           weaponType = -1;
	std::uint32_t armorSlotMask = 0;
	bool          isArmor = false;  // true=armor, false=clothing (when apparel)
	SpellSchool   school = SpellSchool::kNone;
	SpellResist   resist = SpellResist::kNone;
	bool          isLesserPower = false;
	bool          isGreaterPower = false;
	bool          isShout = false;
	bool          isPoison = false;
	bool          isFood = false;
	bool          isIngredient = false;
	bool          soulFull = false;
};

struct ItemVisual
{
	ItemCategory category = ItemCategory::kUnknown;
	std::uint32_t colorRGB = 0x9E9E9Eu;  // border color, 0xRRGGBB
	std::string iconId;                  // stem from docs/icon-names.md
};

[[nodiscard]] ItemVisual ClassifyItem(const ItemDescriptor& a_desc);
[[nodiscard]] const char* CategoryName(ItemCategory a_cat) noexcept;

}  // namespace MMOHotbar
