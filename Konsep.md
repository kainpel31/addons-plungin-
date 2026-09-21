## Yang kamu buat

Sebuah **add-on/patch SKSE** untuk mod **STB Hotkey System**, yang menambahkan **sistem hotbar bergaya MMORPG** ke Skyrim.

**Konsep inti:** STB mengurus *hotkey*-nya (key → item), add-on kamu mengurus *tampilannya* (item → posisi slot). Keduanya terpisah, jadi memindahkan item antar slot **tidak mengubah hotkey**-nya. tapi secara default slot hotbar akan di isi dari kiri. 

---

## Fitur

**Tampilan Hotbar**
1. Bar hotbar horizontal melayang di layar (HUD) bergaya MMORPG.
2. Jumlah slot yang tampil bisa diatur, 1–12.
3. Tiap slot menampilkan tombol hotkey-nya (mis. "1", "Q", "Shift") nama item, icon item jika bisa
4. Warna/border slot dibedakan menurut jenis item (senjata, armor, sihir, ramuan).
5. Tooltip nama item lengkap saat slot disorot mouse.

**Pengaturan Posisi & Ukuran**
6. Slider posisi X dan Y untuk memindahkan hotbar.
7. Pengaturan ukuran slot dan jarak antar-slot.
8. Tombol reset ke pengaturan awal.

**Pengaturan Slot Item** *(fitur kunci)*
9. Daftar semua item yang sudah di-bind di STB, beserta key-nya.
10. Item otomatis masuk slot sesuai nomor tombolnya (key 1 → slot 1).
11. Item bisa dipindah manual ke slot mana pun (1–12) **tanpa mengubah hotkey** di STB.
12. Bisa mengosongkan slot. dan slider berapa slot aktif yang ingin di gunakan 1-12.
13. Memilih slot yang sudah terisi akan menukar posisi kedua item.

**Menu**
14. Menu sendiri di dalam SKSE Menu Framework, dengan sub-halaman *Settings* dan *Slot Assignment*.

**Perilaku**
15. Sinkron otomatis: hotbar ikut berubah saat inventory/item di-bind berubah. 
16. Mendukung hotkey keyboard, mouse, dan gamepad.
17. Hotbar diam dan invisible saat menu game terbuka (inventory, konsol, dsb.) agar tidak bentrok.
18. Binding milik STB tetap berfungsi normal — add-on tidak ikut memicu equip.
19. Pengaturan dan penempatan slot tersimpan di save game.

---

## Batasan yang perlu diketahui

- **Ikon item asli Skyrim tidak bisa ditampilkan.** Ikon game ada di dalam file SWF, sedangkan jalur tampilan yang dipakai add-on ini (ImGui/HUD) hanya bisa memuat gambar PNG. Jadi slot menampilkan **nama item + label key**, bukan ikon asli. Kalau ingin ada gambar, harus PNG buatan sendiri untuk ditempel sebagai frame slot. mencari cara jika  ada mod lain yang bisa di tambahkan untuk menambahkan tampilan item di [nexusmod](https://www.nexusmods.com/games/skyrimspecialedition/mods)
- **Butuh STB Hotkey System terpasang.** Tanpa itu, hotbar tampil kosong.
- **Butuh SKSE Menu Framework terpasang** untuk menu pengaturannya.
- Add-on ini **bukan** pengganti sistem hotkey — ia bergantung penuh pada binding yang dibuat pemain lewat STB.

## refferensi repo
https://www.nexusmods.com/skyrimspecialedition/mods/191157?tab=description
https://www.nexusmods.com/skyrimspecialedition/mods/191157?tab=files
https://github.com/STB-Team/STB-Hotkey-System
https://github.com/QTR-Modding/SKSE-Menu-Framework-3
https://github.com/QTR-Modding/SKSE-Menu-Framework-3-Example
https://github.com/Thiago099/SKSE-Menu-Framework-2/blob/main/Usage.md
https://github.com/pWn3d1337/Skyrim_SpellHotbar
https://github.com/CharmedBaryon/CommonLibSSE-NG
