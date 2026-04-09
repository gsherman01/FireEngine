# FireEngine Outcome & Implementation Plan

## Intended Project Outcome
FireEngine is intended to become a **modular, readable, C++17 3D game engine** with:
- ECS-oriented architecture and loosely coupled systems.
- A practical OpenGL rendering stack suitable for an editor viewport.
- A focus on maintainability over over-engineered abstractions.
- Future-facing subsystems (scripting, assets, events, editor, physics, audio) arranged behind clean module boundaries.

The immediate target (Milestone 1) is a complete, working 3D rendering baseline:
- Window/context creation (GLFW + GLAD).
- Core render loop and depth testing.
- Shader, mesh, texture, camera, transform, lighting, and framebuffer support.
- Multi-object rendering and optional model loading through Assimp.
- Framebuffer output ready for ImGui editor viewport integration.

## Architecture Direction
Target module layout:
- `Engine/Core`
- `Engine/Memory`
- `Engine/ECS`
- `Engine/Scene`
- `Engine/Renderer`
- `Engine/Input`
- `Engine/Scripting`
- `Engine/Asset`
- `Engine/Event`
- `Engine/Editor`

Renderer module should center around:
- `RendererAPI` (OpenGL backend)
- `Shader`, `Texture`, `Mesh`, `Material`, `Camera`, `Framebuffer`, `RenderCommand`

Runtime update order should remain explicit and predictable:
1. Input
2. Scripts
3. Physics
4. ECS systems
5. Rendering
6. Audio

## Visual Studio Project Execution Note
The current Visual Studio project (`FireEngine/FireEngine/FireEngine.vcxproj`) is empty and should be used as the implementation target.

Recommended first implementation step in this project:
1. Add `src/main.cpp` with minimal GLFW + GLAD startup and render loop.
2. Add renderer primitives (`Shader`, `Mesh`, `Texture`, `Camera`, `Framebuffer`) as separate header/source files.
3. Wire MVP + basic Phong lighting.
4. Add framebuffer pass and optional ImGui viewport handoff.

## Suggested Milestone 1 Definition of Done
- Scene renders in 3D with depth testing.
- Camera movement via keyboard/mouse is functional.
- At least two objects are rendered with transforms.
- Lighting visibly affects scene output.
- Scene is rendered into a framebuffer texture.
- Code is modular, readable, and split into focused classes.
