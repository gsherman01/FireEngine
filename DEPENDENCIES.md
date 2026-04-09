# FireEngine Dependency & Linking Plan

This repository now has a single dependency/linking setup intended to cover the full engine scope (renderer, editor, scripting, physics, audio, assets, and utilities) instead of only Milestone 1.

## Dependency Sources

## 1) vcpkg manifest (preferred)
- File: `vcpkg.json`
- Covers core engine dependencies for all planned modules.

Install example:
1. Install vcpkg.
2. From repo root run: `vcpkg install --triplet x64-windows`
3. Ensure Visual Studio vcpkg integration is enabled (`vcpkg integrate install`).

## 2) Manual third_party layout (fallback)
- Property file: `FireEngine/FireEngine.ThirdParty.props`
- Expected root: `FireEngine/third_party/`

Expected subfolders:
- `glad`, `glfw`, `glm`, `stb`, `assimp`, `imgui`, `imguizmo`, `mono`, `openal`,
  `bullet`, `yaml-cpp`, `entt`, `nlohmann_json`, `spdlog`, `freetype`, `physfs`.

The property sheet already defines include/library paths and linker dependencies for this full set.

## Linked Libraries (project-wide)
Configured in `FireEngine/FireEngine.ThirdParty.props`:
- OpenGL + windowing: `opengl32.lib`, `glfw3.lib`, `glad.lib`
- Rendering/editor/model pipeline: `assimp.lib`, `imgui.lib`, `ImGuizmo.lib`
- Scripting: `mono-2.0-sgen.lib`
- Physics: `BulletDynamics.lib`, `BulletCollision.lib`, `LinearMath.lib`
- Audio: `openal32.lib`
- Data/assets: `yaml-cpp.lib`, `physfs.lib`, `freetype.lib`
- Required Windows system libs: `ws2_32.lib`, `winmm.lib`, `imm32.lib`, `version.lib`, `bcrypt.lib`, `dbghelp.lib`

## Troubleshooting: baseline checkout errors (git 128)
If Visual Studio shows errors similar to:
- `failed to git show versions/baseline.json`
- `while checking out baseline from commit ...`
- `git failed with exit code 128`

then your local `C:\vcpkg` clone likely does not contain the baseline commit yet.

Run these commands in **Developer PowerShell**:
1. `git -C C:\vcpkg fetch --all --tags --prune`
2. `git -C C:\vcpkg rev-parse HEAD`
3. `C:\vcpkg\vcpkg.exe install --triplet x64-windows --x-manifest-root <PATH_TO_REPO_ROOT>`

Or use the helper script from repo root:
- `powershell -ExecutionPolicy Bypass -File .\scripts\Sync-VcpkgBaseline.ps1 -VcpkgRoot C:\vcpkg -ManifestPath .\vcpkg.json -Triplet x64-windows`

This script fetches vcpkg history, rewrites `builtin-baseline` in `vcpkg.json` to your local vcpkg HEAD commit, and runs install.

## Notes
- Some library names may vary by package manager/build profile (for example debug suffixes or compiler-specific names).
- If your local package names differ, update `FireEngineThirdPartyLibraries` in `FireEngine/FireEngine.ThirdParty.props` once and all configurations will pick it up.
- If your repository is under OneDrive, ensure files are fully available offline to avoid intermittent toolchain read failures.
