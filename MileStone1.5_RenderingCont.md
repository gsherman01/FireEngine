You are an expert C++ game engine architect and graphics programmer.

I have an existing OpenGL renderer using GLFW that can render a 3D scene.

I want to refactor and extend this into a modular game engine architecture with:
- ECS (Entity Component System) using simple templates
- Clean separation of systems (rendering, input, etc.)
- ImGui editor integration using framebuffer rendering
- Scalable rendering pipeline (for future engine features)

---

Constraints:
- Use C++17 only
- DO NOT over-engineer
- DO NOT use complex template meta-programming
- Templates should be simple and practical (for usability, not cleverness)
- Prefer readability over abstraction
- Keep code easy to debug and extend
- Avoid deep inheritance hierarchies
- Use clear and consistent naming

Libraries used:
- GLFW (window/input)
- GLAD (OpenGL loader)
- glm (math)
- stb_image (textures)
- Assimp (model loading)
- Dear ImGui (editor UI)

---

Goals:
1. Refactor code into modular engine structure
2. Introduce ECS with template-based component management
3. Clean up GLFW/input handling into an abstraction layer
4. Integrate ImGui viewport using framebuffer
5. Expand rendering system to support engine-level features


---

## 1. Application + Engine Architecture

Refactor into:

Application
 - Engine
      - Scene
      - ECS
      - Renderer
      - AssetManager
      - Systems

Main loop should be minimal and clean:

while (running):
    engine.Update()
    engine.Render()

---

## 2. ECS Integration

Implement a simple ECS:

- Entity (ID-based)
- Component storage using contiguous arrays
- Systems operating on components

Required components:
- TransformComponent
- MeshComponent
- CameraComponent
- MaterialComponent
- LightComponent

---

## 3. ImGui + OpenGL Integration (IMPORTANT)

I want a proper editor-style rendering flow:

- Use ONE GLFW window and ONE OpenGL context
- Render scene into a framebuffer
- Display framebuffer inside an ImGui "Viewport" window

Required:
- Framebuffer class
- ImGui setup and render loop
- Viewport panel showing rendered texture

---

## 4. Rendering System Expansion

Upgrade renderer to support:

### Core
- Multiple meshes in scene
- Material system
- Texture binding per material

### Lighting
- Directional light
- Point light
- Basic Blinn-Phong shading

### Texturing
- Diffuse maps
- Specular maps
- Multiple texture slots

### Model Loading
- Use Assimp to import:
  - Meshes
  - Materials
  - Textures

### Advanced Rendering Features (structure-ready, simple implementation)

Include architecture (even if basic implementation):
- Environment mapping (skybox / cubemap)
- Normal mapping (optional stub)
- Specular maps
- Multiple texture slots

---

## 5. Animation System (Basic Support)

Add basic structure for:
- Skeletal animation (can be stubbed or simplified)
- Bone hierarchy
- Animation update system

Focus on:
- Data structures
- Integration with renderer

---

## 6. Asset Manager

Implement:

AssetManager:
- Load models (Assimp)
- Load textures
- Cache assets (avoid duplicates)
- Return handles or pointers
Clean class structure
Simple and readable interfaces
Modular systems
Minimal coupling
Easy to extend

---

## 7. Clean Update Loop

Move logic OUT of main.cpp into systems:

Systems:
- RenderSystem
- CameraSystem
- InputSystem
- AnimationSystem (basic)

---

## 8. Code Output Requirements

Generate:
- Refactored class structure
- Minimal but clean interfaces
- Clear separation of concerns

Important:
- Keep implementation simple and understandable
- Do NOT over-engineer
- Do NOT introduce complex patterns unnecessarily

---

## 9. Explain

Briefly explain:
- How ImGui + framebuffer integration works
- How ECS improves structure
- Tradeoffs made for simplicity vs scalability


Input System (GLFW Abstraction)
Goal

Remove ALL direct GLFW calls from the engine.

Example API
Input::IsKeyPressed(Key::W);
Input::GetMousePosition();

Implementation Notes
Use GLFW callbacks internally
Store key/mouse state in arrays or maps
Polling-based system is sufficient

Rules
No GLFW calls outside Input module
Keep implementation simple