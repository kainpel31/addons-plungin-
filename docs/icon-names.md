# Daftar nama ikon item MMOHotbar

Status: spesifikasi (M6). **Belum ada kode yang membaca berkas ini** — file ini
dibuat sekarang agar set ikon bisa digambar duluan, dan agar kontraknya jelas:
**hotbar harus 100% berfungsi tanpa satu pun file ikon.**

## Aturan main (kontrak)

1. **Ikon murni dekorasi.** Slot selalu menampilkan label key + nama item + border
   warna kategori. Ikon hanya ditempel di atas itu bila berkasnya ada.
2. **Rantai fallback per slot** (tidak pernah kosong, tidak pernah crash):
   `ikon spesifik -> ikon default kategori -> inisial + warna kategori`.
   Contoh: `weapon_bow` hilang -> pakai `weapon_default` -> masih hilang juga ->
   tampilkan inisial nama + border warna senjata.
3. **Satu warning per nama hilang** di log (tidak spam per frame).
4. Format file: **PNG** (utama, 128x128, ada alpha) atau **DDS**; stem file harus
   sama persis dengan ID di bawah (`weapon_bow.png`).
5. Lokasi (nanti, saat M6 diimplementasikan):
   `Data/SKSE/Plugins/MMOHotbar/icons/<id>.png` (atau `.dds`).
   Override per item via `icons.json` (`byFormID -> byEditorID -> byKeyword ->
   byName -> kategori otomatis`) hanya menunjuk ke ID lain di daftar ini.

## Kategori & warna border (tak berubah tanpa ikon)

- Senjata, Armor/Pakaian/Perhiasan, Sihir, Consumable, Scroll, Soul Gem, Lainnya.

## 1. Senjata (`weapon_*`)

| ID | Arti | Sumber data game (nanti) |
|---|---|---|
| `weapon_sword_1h` | Pedang satu tangan | `TESObjectWEAP::kOneHandSword` |
| `weapon_dagger` | Belati | `kOneHandDagger` |
| `weapon_waraxe_1h` | Kapak perang 1H | `kOneHandAxe` |
| `weapon_mace_1h` | Gada 1H | `kOneHandMace` |
| `weapon_greatsword` | Pedang besar 2H | `kTwoHandSword` |
| `weapon_battleaxe` | Kapak 2H | `kTwoHandAxe` |
| `weapon_warhammer` | Palu perang 2H | `kTwoHandBlunt` |
| `weapon_bow` | Busur | `kBow` |
| `weapon_crossbow` | Crossbow | `kCrossbow` |
| `weapon_staff` | Tongkat sihir | `kStaff` |
| `weapon_arrow` | Anak panah | `TESAmmo` (arrow) |
| `weapon_bolt` | Bolt crossbow | `TESAmmo` (bolt) |
| `weapon_unarmed` | Tangan kosong | `kHandToHand` |
| `weapon_default` | Fallback semua senjata | tak dikenal |

## 2. Armor / pakaian / perhiasan (`armor_*`, `clothing_*`, `jewelry_*`)

| ID | Arti | Sumber data game (nanti) |
|---|---|---|
| `armor_helmet` | Helm / penutup kepala | slot mask kepala |
| `armor_cuirass` | Zirah badan | slot mask badan + armor |
| `armor_gauntlets` | Sarung tangan | slot mask tangan + armor |
| `armor_boots` | Sepatu | slot mask kaki + armor |
| `armor_shield` | Perisai | slot perisai |
| `clothing_robe` | Jubah | badan + clothing |
| `clothing_clothes` | Pakaian biasa | badan + clothing |
| `jewelry_amulet` | Kalung | slot amulet |
| `jewelry_ring` | Cincin | slot cincin |
| `armor_default` | Fallback armor/pakaian | tak dikenal |

## 3. Sihir (`spell_*`, `power_*`, `shout`, `scroll`)

| ID | Arti | Sumber data game (nanti) |
|---|---|---|
| `spell_alteration` | Alteration | school = Alteration |
| `spell_conjuration` | Conjuration | school = Conjuration |
| `spell_destruction` | Destruction umum | school = Destruction |
| `spell_fire` | Api (merah) | Destruction + resist fire |
| `spell_frost` | Es (cyan) | Destruction + resist frost |
| `spell_shock` | Listrik (kuning) | Destruction + resist shock |
| `spell_illusion` | Illusion | school = Illusion |
| `spell_restoration` | Restoration | school = Restoration |
| `power_lesser` | Lesser power | jenis power |
| `power_greater` | Greater power | jenis power |
| `shout` | Dragon shout | jenis shout |
| `scroll` | Scroll | FormType Scroll |
| `spell_default` | Fallback sihir | tak dikenal |

## 4. Consumable (`potion_*`, `food`, `poison`, `ingredient`)

| ID | Arti | Sumber data game (nanti) |
|---|---|---|
| `potion_health` | Ramuan health (merah) | AlchemyItem efek health |
| `potion_magicka` | Ramuan magicka (biru) | AlchemyItem efek magicka |
| `potion_stamina` | Ramuan stamina (hijau) | AlchemyItem efek stamina |
| `potion_misc` | Ramuan lain | AlchemyItem non-poison/food |
| `poison` | Racun | AlchemyItem `IsPoison()` |
| `food` | Makanan | AlchemyItem food |
| `drink_soup` | Sup/minuman | food berkuah (opsional; boleh kosong -> `food`) |
| `ingredient` | Bahan alchemy | FormType Ingredient |
| `potion_default` | Fallback consumable | tak dikenal |

## 5. Lainnya

| ID | Arti | Sumber data game (nanti) |
|---|---|---|
| `soulgem_empty` | Soul gem kosong | SoulGem status Empty |
| `soulgem_full` | Soul gem isi | SoulGem status Full/Partial |
| `torch` | Obor | FormType Light |
| `misc_default` | Fallback umum | kunci, misc, buku, dsb. |

## Checklist pembuatan (untuk digambar)

- [ ] 14 senjata (`weapon_*`)
- [ ] 10 armor/pakaian/perhiasan
- [ ] 13 sihir/scroll
- [ ] 9 consumable
- [ ] 4 lainnya
- [ ] Total: **~50 file**, gaya seragam, 128x128, latar transparan.

Catatan: `drink_soup` opsional — bila tidak dibuat, resolver memakai `food`.
ID yang belum ada gambarnya **tidak merusak apa pun**, hanya memakai fallback.
