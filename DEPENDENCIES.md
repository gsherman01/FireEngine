# FireEngine Dependency & Linking Plan

This repository now has a single dependency/linking setup intended to cover the full engine scope (renderer, editor, scripting, physics, audio, assets, and utilities) instead of only Milestone 1.

## Dependency Sources

## 1) vcpkg manifest (preferred)
- File: `FireEngine/vcpkg.json` (same folder as `FireEngine.sln`).
- Default install profile is intentionally minimal (`milestone1`) so Visual Studio can restore and compile the first milestone reliably.

Install examples from repo root:
1. Milestone 1 only (default):
   - `C:\vcpkg\vcpkg.exe install --triplet x64-windows --x-manifest-root .\FireEngine`
2. Full planned engine feature set:
   - `C:\vcpkg\vcpkg.exe install --triplet x64-windows --x-manifest-root .\FireEngine --x-feature=editor,ecs-data,physics,audio,asset-pipeline,tooling`
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
- Scripting: `mono-2.0-sgen.lib` (manual integration; not in vcpkg baseline)
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
3. `C:\vcpkg\vcpkg.exe install --triplet x64-windows --x-manifest-root <PATH_TO_REPO_ROOT>\FireEngine`

Or use the helper script from repo root:
- Milestone 1 only:
  - `powershell -ExecutionPolicy Bypass -File .\scripts\Sync-VcpkgBaseline.ps1 -VcpkgRoot C:\vcpkg -ManifestPath .\FireEngine\vcpkg.json -Triplet x64-windows`
- Full feature set:
  - `powershell -ExecutionPolicy Bypass -File .\scripts\Sync-VcpkgBaseline.ps1 -VcpkgRoot C:\vcpkg -ManifestPath .\FireEngine\vcpkg.json -Triplet x64-windows -Features editor,ecs-data,physics,audio,asset-pipeline,tooling`

This script fetches vcpkg history, rewrites `builtin-baseline` in `FireEngine/vcpkg.json` to your local vcpkg HEAD commit, and runs install.

## Notes
- `mono` is intentionally excluded from `FireEngine/vcpkg.json` because that port may be unavailable for your selected baseline/triplet.
- Integrate Mono manually in `third_party/mono` (or another package source) and keep linking via `FireEngine.ThirdParty.props`.
- Some library names may vary by package manager/build profile (for example debug suffixes or compiler-specific names).
- If your local package names differ, update `FireEngineThirdPartyLibraries` in `FireEngine/FireEngine.ThirdParty.props` once and all configurations will pick it up.
- If your repository is under OneDrive, ensure files are fully available offline to avoid intermittent toolchain read failures.
