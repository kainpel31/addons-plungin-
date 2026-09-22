// ItemClassifier part 1: weapon / ammo / armor branches.
#include "ItemClassifier.h"

namespace MMOHotbar {
namespace {

constexpr std::uint32_t kFormWeapon = 41;
constexpr std::uint32_t kFormArmor = 26;
constexpr std::uint32_t kFormAmmo = 42;

constexpr int kWeapSword = 1;
constexpr int kWeapDagger = 2;
constexpr int kWeapAxe = 3;
constexpr int kWeapMace = 4;
constexpr int kWeapGreatsword = 5;
constexpr int kWeapBattleaxe = 6;
constexpr int kWeapBow = 7;
constexpr int kWeapStaff = 8;
constexpr int kWeapCrossbow = 9;
constexpr int kWeapH2H = 10;
constexpr int kWeapWarhammer = 11;

constexpr std::uint32_t kSlotHead = 0x0001;
constexpr std::uint32_t kSlotBody = 0x0004;
constexpr std::uint32_t kSlotHands = 0x0008;
constexpr std::uint32_t kSlotFeet = 0x0080;
constexpr std::uint32_t kSlotShield = 0x0200;
constexpr std::uint32_t kSlotAmulet = 0x0400;
constexpr std::uint32_t kSlotRing = 0x1000;

}  // namespace

const char* CategoryName(ItemCategory a_cat) noexcept
{
	switch (a_cat) {
	case ItemCategory::kWeapon: return "Weapon";
	case ItemCategory::kArmorClothing: return "Armor/Clothing";
	case ItemCategory::kSpell: return "Spell";
	case ItemCategory::kPower: return "Power";
	case ItemCategory::kShout: return "Shout";
	case ItemCategory::kScroll: return "Scroll";
	case ItemCategory::kConsumable: return "Consumable";
	case ItemCategory::kAmmo: return "Ammo";
	case ItemCategory::kSoulGem: return "Soul Gem";
	case ItemCategory::kTorch: return "Torch";
	case ItemCategory::kMisc: return "Misc";
	default: return "Unknown";
	}
}

// Forward: spell/consumable/misc branches live in ItemClassifier2.cpp.
ItemVisual ClassifyItemTail(const ItemDescriptor& a_desc);

ItemVisual ClassifyItem(const ItemDescriptor& a_desc)
{
	ItemVisual out;

	if (a_desc.formType == kFormWeapon) {
		out.category = ItemCategory::kWeapon;
		out.colorRGB = 0xE08A3Cu;
		switch (a_desc.weaponType) {
		case kWeapSword: out.iconId = "weapon_sword_1h"; break;
		case kWeapDagger: out.iconId = "weapon_dagger"; break;
		case kWeapAxe: out.iconId = "weapon_waraxe_1h"; break;
		case kWeapMace: out.iconId = "weapon_mace_1h"; break;
		case kWeapGreatsword: out.iconId = "weapon_greatsword"; break;
		case kWeapBattleaxe: out.iconId = "weapon_battleaxe"; break;
		case kWeapWarhammer: out.iconId = "weapon_warhammer"; break;
		case kWeapBow: out.iconId = "weapon_bow"; break;
		case kWeapCrossbow: out.iconId = "weapon_crossbow"; break;
		case kWeapStaff: out.iconId = "weapon_staff"; break;
		case kWeapH2H: out.iconId = "weapon_unarmed"; break;
		default: out.iconId = "weapon_default"; break;
		}
		return out;
	}

	if (a_desc.formType == kFormAmmo) {
		out.category = ItemCategory::kAmmo;
		out.colorRGB = 0xB08D57u;
		out.iconId = "weapon_bolt";
		return out;
	}

	if (a_desc.formType == kFormArmor) {
		out.category = ItemCategory::kArmorClothing;
		out.colorRGB = 0x7FA8C9u;
		const auto mask = a_desc.armorSlotMask;
		if (mask & kSlotShield) {
			out.iconId = "armor_shield";
		} else if (mask & kSlotHead) {
			out.iconId = "armor_helmet";
		} else if (mask & kSlotHands) {
			out.iconId = "armor_gauntlets";
		} else if (mask & kSlotFeet) {
			out.iconId = "armor_boots";
		} else if (mask & kSlotAmulet) {
			out.iconId = "jewelry_amulet";
		} else if (mask & kSlotRing) {
			out.iconId = "jewelry_ring";
		} else if (mask & kSlotBody) {
			out.iconId = a_desc.isArmor ? "armor_cuirass" : "clothing_robe";
		} else {
			out.iconId = "armor_default";
		}
		return out;
	}

	return ClassifyItemTail(a_desc);
}

}  // namespace MMOHotbar
