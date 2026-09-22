// M1 data-layer tests: KeyLabel, ItemClassifier, BindingRegistry.
// Pure logic only — no RE/SKSE, no game process.
#include <catch2/catch_test_macros.hpp>

#include "BindingRegistry.h"
#include "ItemClassifier.h"
#include "KeyLabel.h"

using namespace MMOHotbar;

TEST_CASE("KeyLabel: digit row labels and slots", "[keylabel]")
{
	const auto one = DescribeKey(HotbarDevice::kKeyboard, 0x02);
	CHECK(one.label == "1");
	REQUIRE(one.candidateStems.size() == 1);
	CHECK(one.candidateStems[0] == "1");
	CHECK(DigitSlotIndex(HotbarDevice::kKeyboard, 0x02) == 0);

	const auto zero = DescribeKey(HotbarDevice::kKeyboard, 0x0B);
	CHECK(zero.label == "0");
	CHECK(DigitSlotIndex(HotbarDevice::kKeyboard, 0x0B) == 9);

	CHECK(DigitSlotIndex(HotbarDevice::kKeyboard, 0x10) == -1);  // Q: no slot
	CHECK(DigitSlotIndex(HotbarDevice::kMouse, 0) == -1);
}

TEST_CASE("KeyLabel: modifiers, mouse, gamepad, fallback", "[keylabel]")
{
	CHECK(DescribeKey(HotbarDevice::kKeyboard, 0x2A).label == "Shift");
	CHECK(DescribeKey(HotbarDevice::kKeyboard, 0x1D).label == "Ctrl");
	CHECK(DescribeKey(HotbarDevice::kKeyboard, 0x39).label == "Space");
	CHECK(DescribeKey(HotbarDevice::kMouse, 3).label == "M4");
	CHECK(DescribeKey(HotbarDevice::kGamepad, 4).label == "LB");

	// Unknown codes never come back empty (HUD badge must always render).
	const auto weird = DescribeKey(HotbarDevice::kKeyboard, 0xFF);
	CHECK(!weird.label.empty());
	CHECK(weird.candidateStems[0] == "UnknownKey");
}

TEST_CASE("ItemClassifier: weapons by type", "[classifier]")
{
	ItemDescriptor d;
	d.formType = 41;  // Weapon
	d.weaponType = 1;
	const auto sword = ClassifyItem(d);
	CHECK(sword.category == ItemCategory::kWeapon);
	CHECK(sword.iconId == "weapon_sword_1h");

	d.weaponType = 7;
	CHECK(ClassifyItem(d).iconId == "weapon_bow");

	d.weaponType = 8;
	CHECK(ClassifyItem(d).iconId == "weapon_staff");

	d.weaponType = 99;
	const auto def = ClassifyItem(d);
	CHECK(def.category == ItemCategory::kWeapon);
	CHECK(def.iconId == "weapon_default");
}

TEST_CASE("ItemClassifier: spells, scrolls, consumables", "[classifier]")
{
	ItemDescriptor d;
	d.formType = 22;  // Spell
	d.school = SpellSchool::kDestruction;
	d.resist = SpellResist::kFire;
	const auto fire = ClassifyItem(d);
	CHECK(fire.category == ItemCategory::kSpell);
	CHECK(fire.iconId == "spell_fire");

	d.resist = SpellResist::kFrost;
	CHECK(ClassifyItem(d).iconId == "spell_frost");
	d.resist = SpellResist::kShock;
	CHECK(ClassifyItem(d).iconId == "spell_shock");

	d.school = SpellSchool::kRestoration;
	d.resist = SpellResist::kNone;
	CHECK(ClassifyItem(d).iconId == "spell_restoration");

	d = ItemDescriptor{};
	d.formType = 23;  // Scroll
	CHECK(ClassifyItem(d).category == ItemCategory::kScroll);

	d = ItemDescriptor{};
	d.formType = 46;  // AlchemyItem
	d.isPoison = true;
	CHECK(ClassifyItem(d).iconId == "poison");
	d.isPoison = false;
	d.isFood = true;
	CHECK(ClassifyItem(d).iconId == "food");
}

TEST_CASE("ItemClassifier: armor slots and soul gems", "[classifier]")
{
	ItemDescriptor d;
	d.formType = 26;  // Armor
	d.isArmor = true;
	d.armorSlotMask = 0x0001;
	CHECK(ClassifyItem(d).iconId == "armor_helmet");
	d.armorSlotMask = 0x0200;
	CHECK(ClassifyItem(d).iconId == "armor_shield");

	d = ItemDescriptor{};
	d.formType = 52;  // SoulGem
	CHECK(ClassifyItem(d).iconId == "soulgem_empty");
	d.soulFull = true;
	CHECK(ClassifyItem(d).iconId == "soulgem_full");
}

TEST_CASE("BindingRegistry: rebuild keeps pinned rows as stale", "[registry]")
{
	BindingRegistry reg;
	BindingEntry a, b;
	a.item = { 0x1234, 0, 0 };
	a.chord = { 0, 1, { 0x02, 0 } };
	b.item = { 0x5678, 0, 0 };
	b.chord = { 0, 1, { 0x03, 0 } };

	const auto s1 = reg.Rebuild({ a, b });
	CHECK(s1.added == 2);
	CHECK(reg.Size() == 2);

	reg.SetPinned(b.item, true);
	const auto s2 = reg.Rebuild({ a });  // b vanished from STB
	CHECK(s2.removed == 0);              // pinned: kept, not removed
	REQUIRE(reg.Size() == 2);
	bool foundStale = false;
	for (const auto& row : reg.Snapshot()) {
		if (row.item == b.item) {
			foundStale = row.stale;
		}
	}
	CHECK(foundStale);

	reg.SetPinned(b.item, false);
	const auto s3 = reg.Rebuild({ a });
	CHECK(s3.removed == 1);
	CHECK(reg.Size() == 1);
}

TEST_CASE("BindingRegistry: NotePressed creates rows and groups", "[registry]")
{
	BindingRegistry reg;
	ChordKey chord{ 0, 1, { 0x02, 0 } };
	std::vector<ItemKey> group{ { 0x1111, 0, 0 }, { 0x2222, 0, 0 } };

	std::vector<BindingEntry> small;
	small.reserve(1);
	CHECK_FALSE(reg.NotePressed(chord, group, &small));  // STB buffer rule

	std::vector<BindingEntry> out;
	out.reserve(4);
	CHECK(reg.NotePressed(chord, group, &out));
	CHECK(out.size() == 2);
	CHECK(reg.Size() == 2);
	for (const auto& row : reg.Snapshot()) {
		CHECK(row.pressedSeen);
	}
}
