# Ikon di hotbar — opsi dan catatan keputusan

Status: usulan (kandidat M6). Belum ada yang diimplementasikan; M0–M2 dulu.

## Batasnya di mana

Hotbar digambar dengan ImGui lewat SKSE Menu Framework 3. Jadi satu slot bisa menampilkan
teks atau tekstur, dan tekstur hanya bisa masuk lewat
`SKSEMenuFramework::LoadTexture(path, size)`. Loader framework menerima **PNG, SVG, dan
DDS**; `DisposeTexture(path)` membebaskan satu tekstur.

Ikon item asli Skyrim adalah symbol SWF di dalam `Interface/*.swf`, dan ImGui tidak bisa
membacanya — inilah batasan yang tercatat di `Konsep.md`. Semua opsi di bawah ini entah
menghindarinya (A, C) atau menyeberang ke Scaleform untuk mengalahkannya (B).

Yang sudah diverifikasi saat menulis catatan ini (supaya tidak perlu ditemukan ulang):

- `LoadTexture` / `DisposeTexture` hanya pembungkus tipis ke DLL framework:
  `Model::LoadTextureFuction = ImTextureID(*)(const char*, ImVec2*)` yang diambil dengan
  `GetProcAddress` (`third_party/SKSEMenuFramework.h:82` dan `:214`).
- Framework menyelesaikan jalur Data **relatif ke folder game** — `IsInstalled()`-nya sendiri
  adalah `std::filesystem::exists("Data/SKSE/Plugins/SKSEMenuFramework.dll")`. Jadi
  `std::filesystem::exists` untuk `Data/...` di sisi kita juga valid.
- Font Awesome sudah dibundel di framework (`FontAwesome::UnicodeToUtf8`, `PushSolid` /
  `PushRegular` / `PushBrands`), jadi glyph tidak butuh berkas aset sama sekali.
- Vanilla Skyrim **tidak punya art keycap keyboard** (hotkey digambar sebagai teks); hanya
  indikator mouse dan gamepad yang ada. Semua set keycap yang beredar berasal dari SkyUI
  (STB, `flash/README.md`).
- STB sendiri tidak menggambar keycap: ia menyuntikkan symbol SWF (`STBKeycap` dari
  `Interface/STB_Keycaps.swf`) saat runtime dan menempelkannya ke setiap baris list, lalu
  `GotoAndStop` ke frame = DX scancode (`+256` mouse, `+266` gamepad) — layout yang sama
  dengan clip `ButtonArt` milik SkyUI. Perkakasnya FFDec, dan tidak ada `.swf` yang
  di-commit karena lisensi art-nya milik pembuat UI overhaul.

## Opsi A — keycap PNG dari "ImGui Icons" + glyph kategori (rekomendasi)

**Ide:** label tombol di slot digambar sebagai gambar keycap asli, diambil dari konvensi
jalur milik resource mod **ImGui Icons**:

```
Data/Interface/ImGuiIcons/Icons/<Nama>.png
```

Mod itu (dan paket gaya seperti *ImGui Icons - More Styles* karya GroundAura) menyediakan
~137 nama: digit `0`–`9`, huruf `A`–`Z`, `F1`–`F12`, `L-Shift`, `R-Ctrl`, `L-Alt`, `Space`,
`Tab`, `Enter`, `Esc`, `Backspace`, `Semicolon`, `Comma`, `Period`, `Slash`, `Backslash`,
`Quotesingle`, `Bracketleft`, `Bracketright`, `Hyphen`, `Equal`, `Tilde`, `Pause`,
`ScrollLock`, `CapsLock`, `NumLock`, `Insert`, `Home`, `End`, `Delete`, `PgUp`, `PgDn`,
`Left`/`Right`/`Up`/`Down`, `Mouse1`–`Mouse8`, `Wheel`, `WheelUp`, `WheelDown`,
`360_A/B/X/Y/LB/RB/LT/RT/LS/RS/Start/Back`, `PS3_A/B/X/Y/LB/RB/LT/RT/L3/R3/Start/Back`,
`NumPad0`–`NumPad9`, `NumPadPlus/Minus/Mult/Divide`, `Keypad1`–`Keypad8`, `KeypadEnter`,
`UnknownKey`. Gaya dipecah per UI overhaul (SkyUI, Untarnished, Dear Diary, ...).

Kenapa ini menarik:

- Ikonnya **PNG siap pakai di jalur yang sudah jadi konvensi**, dan framework ini memang
  sudah menyatakan ImGui Icons sebagai dependency untuk prompt tombol — jadi banyak pemain
  memasangnya tanpa diminta oleh kita.
- Kita **tidak perlu merilis art siapa pun**: kita hanya membaca berkas yang sudah ada di
  instalasi pemain, dan kalau tidak ada kita jatuh ke teks.
- Restyling gratis: pemain mengganti paket gaya ImGui Icons, hotbar ikut berubah.

Untuk ikon **item**, opsi A memakai Font Awesome yang sudah ikut framework (map kategori →
codepoint: senjata/armor/sihir/ramuan) atau SVG kecil buatan sendiri
(`dist/SKSE/Plugins/MMOHotbar/icons/*.svg`) kalau ingin tampilan yang khas MMORPG.
Gambar slotnya sendiri (frame kategori) juga SVG buatan sendiri — inilah jawaban untuk
permintaan di `Konsep.md` "harus PNG buatan sendiri untuk ditempel sebagai frame slot".

Aturan resolusi yang diusulkan, dari paling spesifik:

1. mapping pengguna `icons.json`: `byFormID` → `byEditorID` → `byKeyword` → `byName`
2. kategori otomatis dari form: `TESObjectWEAP` (+ `GetWeaponType()`), `TESObjectARMO`
   (+ slot mask: shield/pakaian), `SpellItem` (+ `GetSpellType()`: spell/power/shout),
   `AlchemyItem` (+ `IsPoison()`), `TESAmmo`, `TESObjectBOOK`, `Ingredient`, `SoulGem`
3. fallback: glyph Font Awesome kategori → inisial nama item + warna kategori

### Pemetaan key → berkas

`KeyLabel` (pure, sudah ada di rencana M1) memetakan `(device, code)` ke label teks
(`"1"`, `"Q"`, `"Shift"`, `"Mouse4"`, `"LB"`). Untuk opsi A tabel yang sama menghasilkan
**daftar kandidat nama berkas**, dicoba berurutan sampai ada yang ditemukan di disk:

| input | kandidat nama berkas |
|---|---|
| digit `1`–`9`, `0` | `1`…`9`, `0` |
| huruf | `A`…`Z` |
| `F1`–`F12` | `F1`…`F12` |
| Shift / Ctrl / Alt | `L-Shift`+`R-Shift`, `L-Ctrl`+`R-Ctrl`, `L-Alt`+`R-Alt` |
| Space, Tab, Enter, Esc | `Space`, `Tab`, `Enter`, `Esc` |
| Backspace, CapsLock, NumLock | `Backspace`, `CapsLock`, `NumLock` |
| `;` `,` `.` `/` `\` `'` `[` `]` `-` `=` `` ` `` | `Semicolon`, `Comma`, `Period`, `Slash`, `Backslash`, `Quotesingle`, `Bracketleft`, `Bracketright`, `Hyphen`, `Equal`, `Tilde` |
| Insert / Home / End / Delete / PgUp / PgDn | `Insert`, `Home`, `End`, `Delete`, `PgUp`, `PgDn` |
| panah | `Left`, `Right`, `Up`, `Down` |
| keypad | `NumPad0`…`NumPad9`, lalu `Keypad1`…`Keypad8`, lalu `NumPadPlus`/`NumPadMinus`/`NumPadMult`/`NumPadDivide`, `NumPadEnter`/`KeypadEnter` |
| mouse tombol 0–7 | `Mouse1`…`Mouse8` |
| mouse wheel | `Wheel`, `WheelUp`, `WheelDown` |
| gamepad Xbox | `360_A/B/X/Y/LB/RB/LT/RT/LS/RS/Start/Back` |
| gamepad PlayStation | `PS3_A/B/X/Y/LB/RB/LT/RT/L3/R3/Start/Back` |
| tidak dikenal | `UnknownKey` |

Ada dua skema penamaan keypad (`NumPad*` dan `Keypad*`) dan dua keluarga gamepad, jadi
resolusi memang harus berupa daftar kandidat, bukan satu nama pasti.

### Implementasi yang direncanakan

- `KeyLabel` menghasilkan `label` + `candidateStems` dan tidak menyentuh berkas sama sekali;
  `IconResolver` mengubah form → `IconSpec`. Keduanya pure, jadi bisa diuji unit tanpa game.
- `TextureCache`: `unordered_map` berkunci `path + "@" + size`. Hanya slot yang benar-benar
  terlihat yang dimuat (maksimal 12). Ukuran berubah → `DisposeTexture` lalu muat ulang,
  dengan debounce supaya slider ukuran tidak membanjiri VRAM.
- Fallback saat menggambar slot: **gambar** → **glyph Font Awesome** → **inisial + warna
  kategori**. Tooltip hover selalu memuat nama lengkap, jadi informasi tidak pernah hilang
  hanya karena asetnya tidak ada.
- Setting baru: `bEnableIcons`, `fIconScale`, `bPreferIconOverName` (slot kecil: ikon + keycap,
  nama pindah ke tooltip), `sIconPack` (gaya ImGui Icons mana yang dipakai bila pemain memasang
  lebih dari satu).

## Opsi B — ikon asli lewat Scaleform/SWF (spike, bukan v1)

Satu-satunya cara menampilkan ikon *asli* Skyrim adalah jalur yang dipakai STB dan Dynamic
Inventory Icon Injector: menyuntik symbol SWF lalu merendernya dengan Scaleform. Konsekuensinya
hotbar harus menjadi movie Scaleform, bukan ImGui — dua jalur render yang wajib tetap sinkron
dengan model slot yang sama. Biayanya besar, rapuh terhadap UI overhaul, dan `swfhelper` STB
(GPL-3.0, adaptasi karya JerryYOJ) hanya boleh dipakai ulang karena kita juga GPL-3.0 — dengan
atribusi.

Jalan yang jauh lebih murah untuk hasil serupa: **minta STB Team menambahkan
`GetIconName(binding)` di API v2**. Logika ikon sudah ada di sisi mereka (mereka yang menyuntik
ke menu inventory), jadi kita cukup menerima nama symbol dan hotbar menampilkan ikon yang sama
dengan inventory — tanpa SWF surgery di sisi kita.

## Opsi C — paket ikon item dari mod lain (perlu evaluasi)

Ada beberapa mod yang menyediakan set ikon item lewat framework mereka sendiri (mis. *Inventory
Interface Information Injector*, *Icon Collection Utilization Platform*, *Dynamic Inventory
Icon Injector*). Detailnya belum saya verifikasi — yang pasti format dan lisensinya
berbeda-beda, dan yang aman bagi kita adalah **membaca** aset yang dipasang pemain saat runtime,
bukan merilis ulang art-nya.

## Rekomendasi

1. **v1 (M6): opsi A** — keycap dari ImGui Icons + glyph/SVG kategori + `icons.json`. Tanpa
   redistribusi art pihak ketiga, dan tetap berfungsi penuh tanpa mod tambahan.
2. **v1.5:** frame slot SVG buatan sendiri, ikon juga di halaman *Slot Assignment* (memilih item
   jadi jauh lebih mudah), dan ikon besar di tooltip.
3. **v2 (spike):** opsi B, dimulai dari permintaan API ke STB; Scaleform hanya kalau tidak ada
   jalan lain.

## Kriteria penerimaan v1

- Tanpa mod tambahan: semua slot tetap terbaca (glyph/teks), tidak ada slot kosong.
- Dengan ImGui Icons terpasang: keycap tampil untuk keyboard, mouse, dan gamepad.
- Mengubah ukuran slot berkali-kali tidak menaikkan jumlah tekstur hidup — dipastikan dengan
  log `LoadTexture`/`DisposeTexture` yang berpasangan.
- `icons.json` rusak atau menunjuk berkas yang hilang → jatuh ke fallback, tidak crash, dan
  hanya satu warning di log.

