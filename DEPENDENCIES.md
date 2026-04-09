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

## Notes
- Some library names may vary by package manager/build profile (for example debug suffixes or compiler-specific names).
- If your local package names differ, update `FireEngineThirdPartyLibraries` in `FireEngine/FireEngine.ThirdParty.props` once and all configurations will pick it up.
