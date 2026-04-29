# FireEngine

Prompt Engineering of a 3D Game Engine.

## Importing meshes, models, textures, and animations

FireEngine already supports importing these through the **Asset Browser** in editor mode:

1. Launch editor mode (default executable startup).
2. In **Asset Browser**, paste a path into **Path**.
3. Click **Import**.

Supported workflow today:
- **Models / Meshes:** imported with Assimp via `AssetManager::LoadModel`.
- **Textures:** imported with stb via `AssetManager::LoadTexture`.
- **Animations (skeletal):** imported from model files containing animation clips/channels; bone keyframes are sampled by `AnimationSystem`.

### Example paths

- Model with textures: `assets/models/demo_model.obj`
- Texture only: `assets/textures/brick_albedo.png`
- Animated model (example): `assets/models/character.fbx`

> Notes:
> - Animation playback currently uses the first animation clip in the imported model.
> - Skinning uses up to 100 bones in shader uniforms.

---

## How the graphics pipeline works in FireEngine

FireEngine uses a forward rendering pipeline in two passes plus skybox:

1. **Asset import and CPU setup**
   - Assimp reads meshes/materials/skeleton/animation data.
   - Mesh data is uploaded into OpenGL buffers (VAO/VBO/EBO).
   - Texture images are uploaded as OpenGL textures.

2. **Animation update (CPU)**
   - `AnimationSystem` advances playback time.
   - Bone transforms are generated per frame from animation keyframes.
   - Bone matrices are written to `AnimationComponent::boneMatrices`.

3. **Shadow depth pass (GPU)**
   - Engine renders scene from light POV into shadow map depth texture.
   - Uses `shadow_depth.vert/.frag` and optional skinning matrices.

4. **Main forward pass (GPU)**
   - Framebuffer is bound and cleared.
   - For each mesh:
     - Bind shader/material/lighting uniforms.
     - Bind diffuse/specular/normal textures.
     - Bind shadow map.
     - Upload bone matrices (if skinned).
     - Draw indexed mesh.

5. **Skybox pass**
   - Cubemap rendered after scene with `GL_LEQUAL` depth func.

6. **Present**
   - Editor mode: offscreen framebuffer texture is shown inside ImGui viewport.
   - Game mode: rendered directly to default framebuffer.

---

## How to use it (quick start)

1. Start FireEngine in editor mode.
2. Import a model (`.obj`, `.fbx`, `.gltf`, etc.) in Asset Browser.
3. Ensure textures are in relative paths expected by the model material.
4. Add an entity with `MeshComponent` that references the imported model.
5. Add `AnimationComponent` for skinned animation playback.
6. Press play / run loop; `AnimationSystem` + `RenderSystem` handle the rest.

If a model has no animation clips, the engine falls back to a small procedural bone wiggle so you can verify skinning data is present.


## AI/CLI Windows Build Instruction
If you are running in a Windows-capable AI/dev environment, compile without the Visual Studio IDE:

```powershell
pwsh ./scripts/Build-Windows.ps1 -Configuration Debug -Platform x64
```

This script auto-discovers `MSBuild.exe` using `vswhere.exe` and builds `FireEngine/FireEngine.sln`, which lets an agent see compile errors directly in terminal output.

Prerequisites:
- Visual Studio 2022 or Build Tools 2022
- MSBuild component installed
- vcpkg dependencies restored (manifest mode)

## Learner Documentation
For intrusive graphics-call explanations (what call runs + why), see `LEARNER_HELPER.md`.
