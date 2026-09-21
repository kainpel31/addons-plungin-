# MMO Hotbar

An SKSE add-on for **STB Hotkey System** that draws an MMORPG-style hotbar overlay.

STB Hotkey System owns the *hotkeys* (key → item); this add-on owns the *layout*
(item → slot). The two stay separate on purpose, so moving an item between slots never
changes its hotkey. See [`Konsep.md`](Konsep.md) for the full feature list.

**Status: M0 — scaffold.** The plugin loads, logs its runtime and is ready for the
binding registry (M1). The hotbar itself is not drawn yet.

## Requirements

- Skyrim Special Edition 1.5.97, or Skyrim Anniversary Edition 1.6.x / 1.7.x
- [SKSE64](https://skse.silverlock.org/) and the Address Library
- [STB Hotkey System](https://github.com/STB-Team/STB-Hotkey-System) — without it the hotbar has nothing to show
- [SKSE Menu Framework 3](https://github.com/QTR-Modding/SKSE-Menu-Framework-3) — the hotbar HUD and both settings pages are drawn through it

## Building

Requires Visual Studio 2022 (Desktop development with C++), CMake 3.21+ and vcpkg.

```powershell
$env:VCPKG_ROOT = "C:\vcpkg"
cmake --preset default
cmake --build --preset release
```

Output: `build/Release/MMOHotbar.dll` and `build/Release/MMOHotbar.pdb`. Copy the DLL to
`<Skyrim>/Data/SKSE/Plugins/`; `dist/SKSE/Plugins/MMOHotbar.ini` carries the startup
defaults. To have CMake copy the build into a mod folder, use the `deploy` preset with
`-DMMOHOTBAR_DEPLOY_DIR=...`.

Unit tests (pure logic, no game needed):

```powershell
cmake --preset default
cmake --build --preset release --target MMOHotbarTests
ctest --preset release
```

## Build strategy notes

Decisions that are easy to get wrong again later, all of them deliberate:

- **CommonLibSSE-NG from `alandtse/CommonLibSSE-NG@v9.0.0`**, fetched with
  `FetchContent` unless `COMMONLIB_SSE_FOLDER` points at a local checkout. The
  `CharmedBaryon/CommonLibSSE-NG` repository (and the vcpkg port pinned to it) still
  classifies the runtime with `switch (minor) { case 4: VR; case 6: AE; default: SE; }`,
  which reads **AE 1.7.x as SE** and resolves SE offsets against an AE executable. The
  active fork compares `minor >= 6` instead.
- **`USE_ADDRESS_LIBRARY` and no `COMPATIBLE_RUNTIMES`.** The runtime list in a plugin
  declaration is a strict whitelist; listing one generation is how a plugin ends up
  refusing to load on a newer AE release. Address-library independence is what lets a
  single DLL load on SE 1.5.97 through AE 1.7.x.
- **No `STRUCT_DEPENDENT`.** CommonLibSSE-NG supports both the pre-1.6.629 and the
  post-1.6.629 struct layouts in one binary, so the plugin declares
  `StructCompatibility::Independent` and SKSE loads it on either side.
- **vcpkg triplet `x64-windows` (dynamic CRT, `/MD`).** Standard for SKSE plugins, and
  the only CRT that can link CommonLibSSE-NG's published prebuilt bundle. On a
  static-CRT triplet (`x64-windows-static`, `/MT`) the library deliberately falls back to
  compiling ~16 minutes of source instead.
- **`COMMONLIB_PREBUILT_MULTICONFIG=ON` in CI**, because the Visual Studio generator is
  multi-config and the prebuilt bundle is Release-only. Without the opt-in the bundle is
  skipped. If the runner's MSVC toolset version differs from the bundle's, the build
  falls back to source on its own — correct either way, just slower.
- **`builtin-baseline` in `vcpkg.json` points at a dated vcpkg release tag**, not at
  master: a baseline newer than the runner's vcpkg tool is a needless failure mode.

## Layout

```
CMakeLists.txt              target MMOHotbar, CommonLibSSE-NG wiring, optional tests
vcpkg.json                  spdlog, rapidcsv, directxtk (CommonLibSSE-NG needs these), catch2, xbyak
cmake/                      generated Version.h and version resource templates
include/                    PCH.h and the plugin's own headers
src/                        plugin sources
third_party/                vendored interface headers (STB plugin API, SKSE Menu Framework) - see its README
tests/                      Catch2 tests for the pure logic (slot engine, key labels, codec)
dist/SKSE/Plugins/          shipping files, starting with MMOHotbar.ini
```

## Licence

GPL-3.0. This add-on links nothing from STB Hotkey System, but it does copy that
project's plugin API header — which its own README tells plugin authors to do — and STB
Hotkey System is GPL-3.0, so the same licence keeps the arrangement unambiguous. See
[`third_party/README.md`](third_party/README.md).
