#### REQUIREMENTS

- [Visual Studio 2026](https://visualstudio.microsoft.com/) with the **Desktop development with C++** workload.
- [vcpkg](https://github.com/microsoft/vcpkg)
- Two clones of [CommonLibSSE](https://github.com/powerof3/CommonLibSSE) by powerof3:
  - branch `dev` for SE 1.5.97 and AE 1.7.104+
  - branch `dev-1.6.1170` for AE 1.6.1170

#### WINDOWS ENVIRONMENT VARIABLES TO SET

1. **`VCPKG_ROOT`**: The path to your clone of vcpkg.
2. **`CommonLibSSEPath`**: The path to your CommonLibSSE clone on `dev`.
3. **`CommonLibSSE1170Path`**: The path to your CommonLibSSE clone on `dev-1.6.1170`.
4. (optional) **`SKYRIM_MODS_FOLDER`**: The path of the folder where your mods are. Each build copies the DLL and PDB to `<SKYRIM_MODS_FOLDER>/<NAME>/<SE|AE1170|AE>/SKSE/Plugins`.

#### THINGS TO EDIT

1. CMakeLists.txt
- **`NAME`**: Your plugin's name. Default: `ExamplePlugin`
- (optional) **`VERSION`**: Your plugin version. Default: `1.0.0`

2. vcpkg.json
- **`name`**: Your plugin's name, lowercase letters, digits and hyphens only.
- **`version-string`**: Your plugin version. Default: `1.0.0`

#### BUILDING

Open the folder in Visual Studio and pick a preset, or run these from a Developer PowerShell:

```
cmake --preset vs2026-ae
cmake --build --preset vs2026-ae
```

| Runtime | Preset | Output |
|---|---|---|
| SE 1.5.97 | `vs2026-se` | `build/Release` |
| AE 1.6.1170 | `vs2026-ae1170` | `buildae1170/Release` |
| AE 1.7.104+ | `vs2026-ae` | `buildae/Release` |

#### FEATURES

- One DLL per runtime: SE 1.5.97, AE 1.6.1170 and AE 1.7.104+.
- Automatically imports through vcpkg:
  - [commonlib-shared](https://github.com/libxse/commonlib-shared), from CommonLibSSE's overlay port
  - [spdlog](https://github.com/gabime/spdlog)
  - [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352) by Thiago099: `#include "SKSEMCP/SKSEMenuFramework.hpp"`
  - [FLICK API](https://github.com/Fuzzlesz/FUCK_API) v5 (FLICK 1.5.0) by Fuzzlesz, with the matching ImGui headers: `#include <FUCK_API.h>`

Include SKSE Menu Framework and FLICK in separate .cpp files; their ImGui declarations clash in one file.
