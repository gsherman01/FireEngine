# FireEngine Graphics Learner Helper (Intrusive)

> Purpose: explain **graphics-related calls only** and **why each one is invoked**.

---

## 1) OpenGL/Window Bootstrap Calls in `main.cpp`

### `glfwInit()`
**Why:** creates GLFW global state so window/context APIs work.

### `glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3)` + `...MINOR, 3`
**Why:** request OpenGL 3.3 core profile, which matches shader version (`#version 330`) and engine shader expectations.

### `glfwCreateWindow(...)`
**Why:** allocates a native OS window and owns an OpenGL context handle.

### `glfwMakeContextCurrent(window)`
**Why:** all subsequent OpenGL function calls affect this context.

### `gladLoadGLLoader(...)`
**Why:** OpenGL function pointers are resolved at runtime per-driver; without GLAD load, calls like `glBindFramebuffer` are null.

### `glfwSetFramebufferSizeCallback(..., FramebufferSizeCallback)`
**Why:** keeps OpenGL viewport in sync with swapchain size after resize.

### `glEnable(GL_DEPTH_TEST)`
**Why:** enables per-fragment depth rejection so nearer geometry hides farther geometry.

---

## 2) Frame Loop Graphics Calls

### `glfwPollEvents()`
**Why:** pumps OS window/input events; without it the app appears frozen.

### `engine.Update(deltaTime)`
**Why (graphics side):** updates camera and animation matrices before rendering.

### Editor path (`RenderEditor`)
- Renders scene into offscreen framebuffer.
- ImGui samples that color attachment as an image widget.
**Why:** editor UI can surround viewport while keeping scene rendering independent.

### Game path (`RenderGame`)
- Renders directly to default framebuffer (window backbuffer).
**Why:** lower overhead, no editor compositor needed.

### `glfwSwapBuffers(window)`
**Why:** presents rendered backbuffer to screen and acquires next backbuffer for drawing.

---

## 3) Engine Render Pass Calls (`Engine.cpp`)

## Shadow pass
### `m_shadowMap.BindForWriting()`
**Why:** route depth output into dedicated shadow depth texture.

### `RenderSystem::RenderShadowMap(...)`
**Why:** render from light-space camera so later shaders can test shadow occlusion.

## Main forward pass
### `m_framebuffer.Bind()` (editor) or `glBindFramebuffer(GL_FRAMEBUFFER, 0)` (game)
**Why:** choose offscreen target vs window target.

### `glViewport(...)`
**Why:** maps NDC to target resolution; must match current render target size.

### `m_renderer.BeginFrame(clearColor)`
**Why:** clear color/depth buffers before drawing new frame to avoid previous-frame artifacts.

### `RenderSystem::RenderScene(...)`
**Why:** submits meshes/materials/lights/shadows/skybox into final frame.

---

## 4) Shader-level Graphics Data Flow (Why these uniforms exist)

- **Model matrix**: transforms mesh local vertices into world space.
- **View matrix**: transforms world into camera space.
- **Projection matrix**: applies perspective projection.
- **Light-space matrix**: transforms world points into shadow map UV/depth space.
- **Bone matrices**: deform skinned vertices for animated characters.
- **Material params**: tune BRDF-ish response (albedo/spec/shininess in current pipeline).

These are passed every frame because camera, animation pose, and lighting can change every frame.

---

## 5) Why ImGui is compile-gated (`FIREENGINE_ENABLE_IMGUI`)

ImGui calls are wrapped by compile-time flag so builds can:
1. ship runtime without editor overhead,
2. avoid ImGui dependency in pure game/profile builds,
3. keep editor feature velocity independent from core renderer.

---

## 6) Requested feature set: implementation map (what to build next)

### A) True viewport gizmos (translate/rotate/scale)
1. Integrate ImGuizmo draw/interaction in viewport window.
2. Feed selected entity transform matrix to gizmo.
3. Decompose edited matrix back to position/rotation/scale.
4. Add local/global mode + snap increments.

### B) PBR material pipeline + authoring UI
1. Extend `Material` with: metallic, roughness, ao, emissive (+ texture slots).
2. Add PBR fragment shader path (Cook-Torrance).
3. Bind PBR textures in `RenderSystem`.
4. Add editor UI for sliders/maps.

### C) Scene serialization (JSON/YAML)
1. Serialize entity list and component payloads.
2. Persist asset references by path/UUID, not raw pointers.
3. Load scene by reconstructing entities then resolving assets.
4. Add save/load buttons in editor.

### D) Asset pipeline upgrades
1. Async import worker thread + job queue.
2. Asset metadata DB (uuid, source path, import options, dependencies).
3. Reimport detection via file timestamp/hash.
4. Thumbnail renderer for model/material previews.

---

## 7) Fast learning exercises (graphics-only)

1. Change near/far planes and observe depth precision artifacts.
2. Disable depth test temporarily and inspect draw-order issues.
3. Visualize shadow map (fullscreen quad) to debug acne/peter-panning.
4. Toggle wireframe mode with `glPolygonMode`.
5. Vary light-space orthographic bounds and inspect clipped shadows.

