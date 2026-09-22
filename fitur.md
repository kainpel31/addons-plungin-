# Fitur MMO Hotbar — status pengerjaan

> File ini sumber kebenaran status fitur.
> Aturan: perbarui tabel di bawah setiap kali sebuah milestone lolos CI / uji in-game.
> Terakhir diperbarui: 2026-09-22 (perbaikan C1189 PCH).

## Ringkasan

| Area | Status | Keterangan |
|------|--------|------------|
| M0 scaffold + CI hijau | ✅ Selesai | CI hijau, artifact `MMOHotbar.dll` + `.pdb` (belum uji in-game) |
| M1 data layer (StbApi, BindingRegistry 3 lapis, KeyLabel, ItemClassifier) | 🟡 Kode ditulis, menunggu CI | `include/KeyLabel.h`, `include/ItemClassifier.h`, `include/BindingRegistry.h`, `src/KeyLabel.cpp`, `src/ItemClassifier.cpp`, `src/ItemClassifier2.cpp`, `src/BindingRegistry.cpp`, `src/StbAdapter.{h,cpp}` + `tests/DataLayer.cpp`; CI kini juga menjalankan `ctest` |
| M2 HUD hotbar ImGui | ⬜ Belum | Menunggu M1 |
| M3 settings page | ⬜ Belum | Menunggu M2 stabil |
| M4 slot assignment page | ⬜ Belum | Menunggu M2 stabil |
| M5 co-save | ⬜ Belum | ID `MMOH` (0x4D4D4F48) sudah diklaim di `Plugin.cpp`, callback menyusul |
| M6 ikon | ⏸️ Ditunda | Slot wajib jalan tanpa ikon (fallback nama); daftar ikon yang perlu dibuat ada di `docs/icon-names.md` |
| Unit test (Catch2) | ⬜ Belum | Aktif otomatis saat `tests/*.cpp` pertama ditambahkan (M1) |

Legenda: ✅ selesai · 🟡 berjalan · ⬜ belum · ⏸️ ditunda sengaja.

## Detail per milestone

### M0 — Scaffold + CI hijau pertama (✅ selesai, 2026-09-22)
- [x] `CMakeLists.txt` target `MMOHotbar` (C++23, output `build/Release/MMOHotbar.dll`)
- [x] `vcpkg.json` (spdlog, rapidcsv, directxtk, xbyak, simpleini, nlohmann-json, catch2)
- [x] FetchContent `alandtse/CommonLibSSE-NG@v9.0.0` + opsi `COMMONLIB_SSE_FOLDER` lokal + `COMMONLIB_PREBUILT_MULTICONFIG`
- [x] `include/PCH.h` — urutan include diperbaiki (lihat "Perbaikan C1189" di bawah)
- [x] `src/Plugin.cpp` — init log, `SKSE::Init`, klaim co-save ID, listener `kDataLoaded`/`kPostLoadGame`/`kNewGame`, log runtime SE/AE/VR
- [x] `dist/SKSE/Plugins/MMOHotbar.ini`, `.gitignore`, `.clang-format`, `CMakePresets.json`
- [x] `build.yml` — triplet `x64-windows`, cache `vcpkg_installed` + `build/_deps`
- [x] CI hijau + artifact `MMOHotbar.dll` + `.pdb` (belum uji in-game)

### M1 — Data layer (🟡 kode ditulis, menunggu CI)
- [x] `KeyLabel.h/.cpp`: DX scancode/mouse/gamepad → label + kandidat stem ikon (tabel keyboard 78 entri + mouse + gamepad, `DigitSlotIndex` untuk slot 1–10)
- [x] `ItemClassifier.h` + `ItemClassifier.cpp`/`ItemClassifier2.cpp`: deskriptor → kategori + warna border + ID ikon (senjata per tipe, armor per slot mask, spell per school + warna resist fire/frost/shock, power, shout, scroll, consumable, soul gem, torch, misc)
- [x] `BindingRegistry.h/.cpp`: snapshot kanonis `{form,ench,health}` + `{device,keyCount,keys}`, merge `Rebuild` (pinned bertahan jadi stale), `NotePressed` (aturan buffer STB Resolve), `SetPinned`/`SetName`
- [x] `StbAdapter.h/.cpp` (khusus target plugin): `ResolveStbApi` via `GetModuleHandleA` + `RequestAPI(1)`, `SweepSingleKeys` (~528 panggilan), `HasChord`, `ResolvePress`
- [x] `Plugin.cpp`: resolve API di `kPostLoad`, sweep di `kDataLoaded`, log status di `MMOHotbar.log`
- [x] `tests/DataLayer.cpp`: 7 TEST_CASE Catch2 (key label, slot digit, classifier senjata/spell/armor, merge registry pinned/stale, grup NotePressed + aturan buffer)
- [x] CI menjalankan `ctest` (`-DENABLE_TESTS=ON -DBUILD_TESTING=ON`)
- [ ] CI hijau (push berikutnya); in-game: log `STB Hotkey System API: resolved (v1)` + `bindings: sweep done`

### M2 — HUD hotbar (⬜ belum)
- [ ] Slot persegi via `AddHudElement` + `ImDrawListManager`, label key + nama terpotong + border warna kategori
- [ ] Tooltip hover, badge `×N` untuk grup, sembunyi saat menu blocking terbuka
- [ ] Tes `SlotEngine` (auto-fill digit → slot, first-free, swap, clear, overflow)

### M3 — Settings page (⬜ belum)
- [ ] X/Y, ukuran, spacing, jumlah slot aktif, reset ke default

### M4 — Slot Assignment page (⬜ belum)
- [ ] Daftar binding + pindah/swap/clear + pin

### M5 — Co-save (⬜ belum)
- [ ] Record `CONF` + `SLOT`, `ResolveFormID`; entry pinned yang hilang tetap tampil redup + opsi hapus

### M6 — Ikon (⏸️ ditunda)
- Keputusan: **tanpa ikon pun mod tetap berfungsi penuh** (slot menampilkan nama item).
- Kamu akan membuat sendiri paket ikon; acuannya `docs/icon-names.md` (+ `docs/icons.md` untuk desain loader).
- [ ] `IconLabelResolver` (port logika label ikon inventory)
- [ ] `TextureCache` (agnostik format PNG/DDS, hanya slot terlihat dimuat, dispose saat pack/ukuran berubah)
- [ ] `icons.json` override (`byFormID → byEditorID → byKeyword → byName → kategori`)

## Perbaikan C1189 (2026-09-22)
- Error: `REX/W32/BASE.h(4,1): error C1189: #error: Windows API detected...` pada `cmake_pch.cxx`.
- Penyebab: guard CommonLibSSE-NG memeriksa `#ifdef _INC_WINAPIFAMILY` — header STL MSVC baru bisa menarik `winapifamily.h` secara transitif **sebelum** `REX/W32/BASE.h` sempat dimuat, walau tidak ada `#include <windows.h>` eksplisit di kode kita. `CommonLibSSE.lib` sendiri lolos karena PCH internalnya (`SKSE/Impl/PCH.h`) memuat puluhan header STL **dulu** lalu `REX/W32/*` belakangan dalam satu urutan yang terbukti aman di toolchain itu.
- Perbaikan di `include/PCH.h`: muat `<array> <bit> <cstdint> <cstring>` (hanya yang dibutuhkan `BASE.h`, terbukti aman) lalu `<REX/W32/BASE.h>` **paling awal**, sebelum `RE/Skyrim.h` / `SKSE/SKSE.h` / STL lain / spdlog. Include `BASE.h` berikutnya jadi no-op via `#pragma once`, jadi guard jinak untuk seluruh TU.
- Yang TIDAK diubah: tidak ada `#include <windows.h>` di proyek; `CMakeLists.txt` dan `build.yml` tidak disentuh untuk perbaikan ini.

## Uji manual wajib (sebelum rilis, SE 1.5.97 + AE 1.7.x)
- [ ] Tanpa STB → hotbar kosong, tidak crash
- [ ] Tanpa Menu Framework → tidak load UI, tidak crash
- [ ] Buka inventory/console → hotbar hilang
- [ ] Bind/unbind saat main → hotbar ikut berubah
- [ ] Save → load → penempatan kembali
