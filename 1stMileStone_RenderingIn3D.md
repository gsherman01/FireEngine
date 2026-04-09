You are an expert C++ graphics programmer and engine architect.

I am building a simple, modular 3D game engine in C++17.
For this milestone, I want to implement a complete 3D rendering pipeline using OpenGL.

Constraints:
- Use C++17 (avoid advanced/complex modern C++ features)
- Prioritize readability, simplicity, and maintainability
- Avoid over-engineering and unnecessary abstractions
- Keep code straightforward and beginner-friendly where possible
- No heavy template meta-programming
- Use clear class/struct design
- Add comments explaining key parts

Libraries to use:
- GLFW (window + input)
- GLAD (OpenGL loader)
- glm (math)
- stb_image (texture loading)
- Assimp (model loading, optional but preferred)

---

Goal:
Build a working 3D renderer that can:
- Open a window
- Render multiple 3D objects
- Support camera movement
- Apply basic lighting
- Load models and textures
- Render through a framebuffer (for future editor integration)

---

Features to implement:

1. OpenGL Initialization
- Create window using GLFW
- Initialize GLAD
- Setup viewport and resize callback
- Enable depth testing

2. Shader System
- Load vertex + fragment shaders from file
- Compile and link shaders
- Provide uniform setters (mat4, vec3, float)

3. Mesh System
- Setup VAO, VBO, EBO
- Support indexed drawing
- Define vertex layout (position, normal, uv)

4. Texture System
- Load textures using stb_image
- Setup texture parameters (wrapping, filtering)
- Generate mipmaps

5. Camera System
- Perspective projection
- View matrix calculation
- Basic FPS camera (WASD + mouse look)

6. Transform System (simple)
- Position, rotation, scale
- Compute model matrix

7. Rendering Pipeline
- Clear color and depth buffer
- Compute MVP (Model, View, Projection)
- Pass matrices to shader
- Draw multiple objects

8. Lighting
- Implement basic Phong or Blinn-Phong lighting:
  - Ambient
  - Diffuse
  - Specular

9. Framebuffer
- Create framebuffer object
- Attach color texture
- Support resize
- Render scene into framebuffer

10. Model Loading (optional but preferred)
- Use Assimp to load .obj or .fbx models
- Extract meshes and textures

---

Output Requirements:

Generate:
- Clean, modular C++ code split into logical classes:
  - Shader
  - Mesh
  - Texture
  - Camera
  - Renderer
  - Framebuffer
- A simple main.cpp that ties everything together
- Minimal but clear class interfaces
- Avoid unnecessary abstraction layers

---

Coding Style:
- Use simple structs/classes
- Keep functions short and readable
- Avoid deep inheritance hierarchies
- Prefer composition over inheritance
- Use clear naming (no abbreviations)
- Comment important logic (especially OpenGL parts)

---

Explain briefly:
- Why certain design decisions were made
- Any tradeoffs in simplicity vs flexibility


 Rendering Features Checklist
1. Window & Context
Create window using GLFW
Initialize GLAD
Handle resize events
Enable VSync
2. Shader System
Load shaders from file
Compile and link
Set uniforms (mat4, vec3, float)
Optional: hot reload
3. Mesh System
VAO / VBO / EBO setup
Indexed drawing
Vertex attributes
4. Texture System
Load textures via stb_image
Configure wrapping & filtering
Generate mipmaps
5. Camera System
Perspective projection
View matrix
FPS-style controls
6. Transform Integration
Model matrix per entity
Combine into MVP
7. Lighting (Core)
Ambient
Diffuse
Specular (Phong / Blinn-Phong)
8. Material System
Material:
 - Shader
 - Textures
 - Parameters
9. Scene Rendering
For each entity:
 - Get Transform + Mesh
 - Compute MVP
 - Bind shader
 - Draw mesh
10. Depth Testing
glEnable(GL_DEPTH_TEST);
11. Framebuffer
Render scene to texture
Required for editor viewport
12. Debug Rendering
Wireframe mode
Grid
Axis gizmo
13. Model Loading
Load .obj / .fbx via Assimp
Extract mesh + textures

Rendering Prompts
 OpenGL Setup
Generate a simple OpenGL setup using GLFW and GLAD:
- Create window
- Initialize GLAD
- Enable depth testing
- Setup resize callback
Keep code simple and readable.
 Render Loop
Generate a basic render loop:
- Clear color and depth buffer
- Update camera
- Draw multiple objects using transforms
Keep logic straightforward.
 MVP Pipeline
Generate code to compute:
- Model matrix from position, rotation, scale
- View matrix from camera
- Projection matrix
- Pass MVP to shader
Keep math clear and readable.
 Framebuffer
Generate a simple framebuffer class in OpenGL:
- Create color attachment texture
- Support resize
- Bind/unbind functions
Avoid overengineering.
 ImGui Viewport
Generate ImGui code to:
- Display framebuffer texture
- Act as a viewport window
Keep it minimal and clean.
 Editor (ImGui)
 Panels
Editor:
 - Scene Hierarchy
 - Inspector
 - Viewport
 - Asset Browser
 Key Concept
Render Scene -> Framebuffer -> Display in ImGui
 Asset Manager
 Asset Types
Graphics
Models (.obj, .fbx)
Textures (.png, .jpg)
Shaders (.glsl)
Audio
.wav / .mp3
Scripts
Compiled C# assemblies
Data
Scenes / Prefabs (JSON/YAML)
 Responsibilities
AssetManager:
 - Load assets
 - Cache assets
 - Return handles
 - Manage dependencies
 Event System
EventBus:
 - Subscribe<T>()
 - Publish<T>()

Used for:

Input events
Window events
Debug signals
 Milestone 1 Success Criteria

By the end, your engine should:

Render 3D models
Support camera movement (WASD + mouse)
Handle multiple objects
Use lighting (Phong)
Enable depth testing
Render to framebuffer
Display viewport via ImGui
Load models and textures