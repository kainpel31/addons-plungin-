# MMO Hotbar Addon for STB-Hotkey-System

Addon plugin SKSE (Skyrim Script Extender) yang dirancang untuk menghadirkan sistem bilah pintasan (*Hotbar*) bergaya game MMORPG (hingga 12 slot horizontal) yang terintegrasi secara mulus dengan **STB-Hotkey-System** tanpa memodifikasi kode inti aslinya.

---

## 🚀 Konsep & Arsitektur Mod

Mod ini dibangun dengan pendekatan **Standalone Companion Plugin** menggunakan ekosistem modding modern Skyrim SE/AE:
1. **Separasi Visual & Backend:** Antarmuka visual (UI) dirender menggunakan aset Scaleform (`.swf`) yang dihasilkan melalui *SWF Smelter*, diposisikan secara fix secara horizontal di bagian bawah layar.
2. **Manajemen Dinamis via SKSE Menu Framework:** Pemain dapat mengatur jumlah slot aktif serta kustomisasi tata letak langsung di dalam game menggunakan antarmuka berbasis ImGui tanpa perlu menggunakan menu MCM tradisional yang kaku.
3. **Komunikasi Aman via API (`STB_HotkeySystemAPI.h`):** Addon ini bertindak sebagai pengendali input independen yang memanggil fungsi eksekusi hotkey inti milik STB secara *real-time* saat tombol pintas ditekan.

---

## 🛠️ Detail Teknik Code

* **Bahasa Pemrograman:** C++ (C++23)
* **Framework Utama:** 
  * [CommonLibSSE-NG](https://github.com/CharmedBaryon/CommonLibSSE-NG) (Kompatibel dengan Skyrim SE & AE)
  * [SKSE Menu Framework](https://github.com/QTR-Modding/SKSE-Menu-Framework-3) (Untuk konfigurasi menu ImGui)
* **Manajemen Dependensi:** Dikelola menggunakan `vcpkg` (`spdlog`, `nlohmann_json`, `xbyak`).
* **Sistem Event Listener:** Menggunakan `RE::BSTEventSink<RE::InputEvent*>` untuk mendeteksi penekanan tombol keyboard secara langsung (`DXScanCode`) dan memetakannya ke fungsi `ExecuteSlot` dari API STB.

---

## 📂 Struktur Proyek

```text
.
├── .github/workflows/build.yml   # Skrip GitHub Actions untuk kompilasi otomatis (.dll)
├── include/
│   └── STB_HotkeySystemAPI.h     # Header resmi API STB-Hotkey-System
├── src/
│   └── main.cpp                  # Logika utama plugin (Input listener, ImGui menu, & STB API bridge)
├── CMakeLists.txt                # Konfigurasi build CMake & CommonLibSSE
├── vcpkg.json                    # Konfigurasi dependensi pustaka C++
└── README.md
