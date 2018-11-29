%{
#include <NeoCore.h>
#include <NeoEngine.h>

#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>
#include <Matrix4x4.h>
#include <Color.h>
#include <Image.h>
#include <Log.h>
#include <Utils.h>

#include <Behavior.h>
#include <Game.h>
#include <GameState.h>
#include <InputContext.h>
#include <InputMapping.h>
#include <Level.h>
#include <Mesh.h>
#include <NeoEngine.h>
#include <Object.h>
#include <PhysicsContext.h>
#include <Renderer.h>
#include <Sound.h>
#include <SoundContext.h>
#include <Texture.h>
#include <Window.h>
#include <Platform.h>

#include <behaviors/CameraBehavior.h>
#include <behaviors/LightBehavior.h>
#include <behaviors/MeshBehavior.h>
#include <behaviors/RigidbodyPhysicsBehavior.h>
#include <behaviors/SoundBehavior.h>
#include <behaviors/StaticRenderBehavior.h>

using namespace Neo;
%}

%include <windows.i>
%include <cpointer.i>
%include <std_string.i>
%include <std_vector.i>

#define NEO_CORE_EXPORT
#define NEO_ENGINE_EXPORT

%include <Vector2.h>
%include <Vector3.h>
%include <Vector4.h>
%include <Matrix4x4.h>
%include <Color.h>
%include <Image.h>
%include <Log.h>
%include <Utils.h>
%include <Array.h>
%include <Handle.h>

%include <Behavior.h>
%include <Game.h>
%include <GameState.h>
%include <InputContext.h>
%include <InputMapping.h>
%include <Level.h>
%include <Mesh.h>
%include <NeoEngine.h>
%include <Object.h>
%include <PhysicsContext.h>
%include <Renderer.h>
%include <Sound.h>
%include <SoundContext.h>
%include <Texture.h>
%include <Window.h>

%include <behaviors/CameraBehavior.h>
%include <behaviors/LightBehavior.h>
%include <behaviors/MeshBehavior.h>
%include <behaviors/RigidbodyPhysicsBehavior.h>
%include <behaviors/SoundBehavior.h>
%include <behaviors/StaticRenderBehavior.h>

%include <Platform.h>

%template(ObjectHandle) Neo::Handle<Object, std::vector<Object>>;

 
