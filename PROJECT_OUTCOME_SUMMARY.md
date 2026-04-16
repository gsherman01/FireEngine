# Project Outcome Summary

This document captures the expected outcome from the master context and milestone architecture notes,
and confirms where implementation work should happen.

## Intended Outcome

The repository is targeting a **modular 3D game engine** in **C++17** with:
- ECS-oriented, data-friendly architecture
- modern OpenGL rendering
- clean, readable code over heavy abstraction
- future editor integration through ImGui and framebuffer rendering

## Milestone 1 (Rendering in 3D)

The first milestone focuses on delivering a complete, understandable rendering vertical slice:
- window/context initialization (GLFW + GLAD)
- core renderer classes (`Shader`, `Mesh`, `Texture`, `Camera`, `Renderer`, `Framebuffer`)
- transform + MVP pipeline
- basic lighting
- model/texture support
- framebuffer output for editor viewport embedding

## File Architecture Direction

The long-term architecture is organized into core engine modules (Core, ECS, Scene, Renderer, Input, etc.)
and driven by systems like Transform, Render, Camera, Input, Script, Physics, and Audio.

## Where to Implement

Implementation work should be done in the Visual Studio project at:

- `FireEngine/FireEngine/FireEngine.vcxproj`
- source root: `FireEngine/FireEngine/src/`

This project already references renderer and scene source/header files and is the correct place to continue milestone work.
