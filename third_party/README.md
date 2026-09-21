# third_party

Interface headers copied from other projects. They are declarations only: no code from
these projects is compiled, linked or redistributed, but they are kept verbatim so that
the interfaces stay byte-identical to what the other side exports.

## STB_HotkeySystemAPI.h

- Source: <https://github.com/STB-Team/STB-Hotkey-System>, `api/STB_HotkeySystemAPI.h`
- What it is: the official plugin API (version 1) of STB Hotkey System. Its own README
  instructs plugin authors to copy this header into their project.
- Why it is copied rather than reimplemented: the header is intentionally standalone
  (no CommonLibSSE or STL types cross the DLL boundary), so it does not have to match
  either side's CommonLib version or standard library.
- Licence: STB Hotkey System is GPL-3.0. The header carries no separate licence notice.
  This project therefore ships under GPL-3.0 as well (see the repository licence) so the
  copy is unambiguous.

## SKSEMenuFramework.h

- Source: <https://github.com/QTR-Modding/SKSE-Menu-Framework-3-API>,
  `SKSEMenuFramework.h`
- What it is: the SKSE Menu Framework 3 client header. It resolves the framework's
  exported functions through `GetProcAddress`, and bundles the ImGui declarations the
  framework exposes, which is why it is ~530 KB.
- Licence: the framework repository is GPL-3.0, but the header itself is licensed so
  that plugins using it as a library do not have to be open source under the same terms
  (stated in the framework README).
- Used for: the hotbar HUD (`AddHudElement`), the settings and slot assignment pages
  (`AddSectionItem`), the input probe (`AddInputEvent`) and texture loading
  (`LoadTexture` / `DisposeTexture`).

## Updating

Both headers are pulled from the projects' default branches. When either is updated
upstream, re-download the file and note the new revision here so a wrong-ABI build is
traceable rather than mysterious.
