#pragma once

#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <assert.h>
#include <wrl.h>

//STL
#include <functional>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <fstream>
#include <algorithm>

//DirectX
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>

//Mainframe
#include "./System/Context.h"
#include "./Engine.h"

//Utilities
#include "./Utility/Math.h"
#include "./Utility/FamilyTypeID.h"
#include "./Utility/Handle.h"
#include "./Utility/FileStream.h"
#include "./Utility/Geometry.h"
#include "./Utility/ChunkAllocator.h"
#include "./Utility/Serializable.h"

//Graphics
#include "D3D11/ConstantBuffer.h"
#include "D3D11/DepthStencilState.h"
#include "D3D11/RasterizerState.h"
#include "D3D11/BlendState.h"
#include "D3D11/Shader.h"
#include "D3D11/InputLayout.h"
#include "D3D11/RenderTarget.h"

//Resource
#include "./Resource/Resource.h"
#include "./Resource/Texture.h"
#include "./Resource/Material.h"
#include "./Resource/Mesh.h"
#include "./Resource/Animation.h"

//System
#include "./System/Time.h"
#include "./System/Window.h"
#include "./System/Input.h"
#include "./System/Graphics.h"
#include "./System/ResourceManager.h"
#include "./System/SceneManager.h"
#include "./System/Renderer.h"

//Core
#include "./Core/Scene.h"
#include "./Core/GameObject.h"

//Component
#include "./Core/Component/Component.h"
#include "./Core/Component/Transform.h"
#include "./Core/Component/Camera.h"
#include "./Core/Component/Light.h"
#include "./Core/Component/Animator.h"
#include "./Core/Component/MeshRenderer.h"
#include "./Core/Component/SkinnedMeshRenderer.h"