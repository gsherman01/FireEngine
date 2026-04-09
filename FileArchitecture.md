Engine/
 - Core/
 - Memory/
 - ECS/
 - Scene/
 - Renderer/
 - Audio/
 - Input/
 - Physics/
 - Scripting/
 - Asset/
 - Serialization/
 - Event/
 - JobSystem/
 - Editor/


 Systems:
 - TransformSystem
 - RenderSystem
 - CameraSystem
 - InputSystem
 - ScriptSystem
 - PhysicsSystem
 - AudioSystem
 - AnimationSystem (future)


Renderer Architecture
Renderer/
 - RendererAPI (OpenGL)
 - Shader
 - Texture
 - Mesh
 - Material
 - Framebuffer
 - Camera
 - RenderCommand
 

 Application::Run()
 - InputSystem
 - ScriptSystem
 - PhysicsSystem
 - ECS Systems Update
 - RenderSystem
 - AudioSystem