// ItemClassifier part 2: spell / power / shout / scroll / consumable / misc.
#include "ItemClassifier.h"

namespace MMOHotbar {

ItemVisual ClassifyItemTail(const ItemDescriptor& a_desc)
{
	ItemVisual out;

	// RE::FormType: Scroll 23, Spell 22, Shout 27, Ingredient 30,
	// AlchemyItem 46, SoulGem 52, Light 31.
	if (a_desc.formType == 22 || a_desc.school != SpellSchool::kNone) {
		if (a_desc.isLesserPower) {
			out.category = ItemCategory::kPower;
			out.colorRGB = 0xB39DDB;
			out.iconId = "power_lesser";
			return out;
		}
		if (a_desc.isGreaterPower) {
			out.category = ItemCategory::kPower;
			out.colorRGB = 0xB39DDB;
			out.iconId = "power_greater";
			return out;
		}
		out.category = ItemCategory::kSpell;
		switch (a_desc.school) {
		case SpellSchool::kAlteration: out.colorRGB = 0xB39DDB; out.iconId = "spell_alteration"; break;
		case SpellSchool::kConjuration: out.colorRGB = 0x9575CD; out.iconId = "spell_conjuration"; break;
		case SpellSchool::kIllusion: out.colorRGB = 0x7986CB; out.iconId = "spell_illusion"; break;
		case SpellSchool::kRestoration: out.colorRGB = 0xFFF176; out.iconId = "spell_restoration"; break;
		case SpellSchool::kDestruction:
		default:
			switch (a_desc.resist) {
			case SpellResist::kFire: out.colorRGB = 0xC73636; out.iconId = "spell_fire"; break;
			case SpellResist::kFrost: out.colorRGB = 0x1FFBFF; out.iconId = "spell_frost"; break;
			case SpellResist::kShock: out.colorRGB = 0xEAAB00; out.iconId = "spell_shock"; break;
			default: out.colorRGB = 0xEF5350; out.iconId = "spell_destruction"; break;
			}
			break;
		}
		return out;
	}

	if (a_desc.formType == 27 || a_desc.isShout) {
		out.category = ItemCategory::kShout;
		out.colorRGB = 0x90A4AE;
		out.iconId = "shout";
		return out;
	}

	if (a_desc.formType == 23) {
		out.category = ItemCategory::kScroll;
		out.colorRGB = 0xD7CCC8;
		out.iconId = "scroll";
		return out;
	}

	if (a_desc.formType == 46) {
		out.category = ItemCategory::kConsumable;
		if (a_desc.isPoison) {
			out.colorRGB = 0x7CB342;
			out.iconId = "poison";
		} else if (a_desc.isFood) {
			out.colorRGB = 0xFFB74D;
			out.iconId = "food";
		} else {
			out.colorRGB = 0xF06292;
			out.iconId = "potion_misc";
		}
		return out;
	}

	if (a_desc.formType == 30 || a_desc.isIngredient) {
		out.category = ItemCategory::kConsumable;
		out.colorRGB = 0xAED581;
		out.iconId = "ingredient";
		return out;
	}

	if (a_desc.formType == 52) {
		out.category = ItemCategory::kSoulGem;
		out.colorRGB = 0x4DD0E1;
		out.iconId = a_desc.soulFull ? "soulgem_full" : "soulgem_empty";
		return out;
	}

	if (a_desc.formType == 31) {
		out.category = ItemCategory::kTorch;
		out.colorRGB = 0xFFCA28;
		out.iconId = "torch";
		return out;
	}

	out.category = ItemCategory::kMisc;
	out.colorRGB = 0x9E9E9E;
	out.iconId = "misc_default";
	return out;
}

}  // namespace MMOHotbar
