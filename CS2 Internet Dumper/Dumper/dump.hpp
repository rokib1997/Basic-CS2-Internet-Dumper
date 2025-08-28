#pragma once
#include <cstddef>
#include <windows.h>
#include <wininet.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <regex>
#include <unordered_map>
#include <unordered_set>

#ifdef _WIN32
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

	EXPORT_API BOOL Initialize();
	EXPORT_API std::string Initialize_Error();
	EXPORT_API VOID Cleanup();
	EXPORT_API std::ptrdiff_t Get(const char* name);
	EXPORT_API std::ptrdiff_t GetOffsets(const char* name);
	EXPORT_API std::ptrdiff_t GetClient(const char* name);
	EXPORT_API std::ptrdiff_t GetAnimationSystem(const char* name);
	EXPORT_API std::ptrdiff_t GetButtons(const char* name);
	EXPORT_API std::ptrdiff_t GetEngine2(const char* name);
	EXPORT_API std::ptrdiff_t GetInputSystem(const char* name);
	EXPORT_API std::ptrdiff_t GetMatchmaking(const char* name);
	EXPORT_API std::ptrdiff_t GetMaterialSystem(const char* name);
	EXPORT_API std::ptrdiff_t GetRenderSystemDX11(const char* name);
	EXPORT_API std::ptrdiff_t GetSoundSystem(const char* name);
	EXPORT_API std::ptrdiff_t GetHost(const char* name);
	EXPORT_API std::ptrdiff_t GetInterfaces(const char* name);
	EXPORT_API std::ptrdiff_t GetNetworkSystem(const char* name);
	EXPORT_API std::ptrdiff_t GetPanorama(const char* name);
	EXPORT_API std::ptrdiff_t GetParticles(const char* name);
	EXPORT_API std::ptrdiff_t GetPulseSystem(const char* name);
	EXPORT_API std::ptrdiff_t GetResourceSystem(const char* name);
	EXPORT_API std::ptrdiff_t GetSceneSystem(const char* name);
	EXPORT_API std::ptrdiff_t GetSchemaSystem(const char* name);
	EXPORT_API std::ptrdiff_t GetServer(const char* name);
	EXPORT_API std::ptrdiff_t GetSteamAudio(const char* name);
	EXPORT_API std::ptrdiff_t GetVphysics2(const char* name);
	EXPORT_API std::ptrdiff_t GetWorldRenderer(const char* name);

#ifdef __cplusplus
}
#endif