#pragma once

#define NOMINMAX
#pragma warning(disable : 4996)
#include <Windows.h>
#include <d3d11_1.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <assert.h>

#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"

using namespace DirectX;

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "winmm.lib")

#define SCREEN_WIDTH	 (1280)
#define SCREEN_HEIGHT	 (720)

HWND GetWindow();