#include <Windows.h>
#include <iostream>
#include <iomanip>

#pragma comment(lib, "../Build/Dumper.lib")

namespace Dumper
{
    extern "C" {
        BOOL Initialize();
        std::string Initialize_Error();
        VOID Cleanup();

        std::ptrdiff_t __cdecl Get(const char* name);
        std::ptrdiff_t __cdecl GetOffsets(const char* name);
        std::ptrdiff_t __cdecl GetClient(const char* name);
        std::ptrdiff_t __cdecl GetAnimationSystem(const char* name);
        std::ptrdiff_t __cdecl GetButtons(const char* name);
        std::ptrdiff_t __cdecl GetEngine2(const char* name);
        std::ptrdiff_t __cdecl GetInputSystem(const char* name);
        std::ptrdiff_t __cdecl GetMatchmaking(const char* name);
        std::ptrdiff_t __cdecl GetMaterialSystem(const char* name);
        std::ptrdiff_t __cdecl GetRenderSystemDX11(const char* name);
        std::ptrdiff_t __cdecl GetSoundSystem(const char* name);
        std::ptrdiff_t __cdecl GetHost(const char* name);
        std::ptrdiff_t __cdecl GetInterfaces(const char* name);
        std::ptrdiff_t __cdecl GetNetworkSystem(const char* name);
        std::ptrdiff_t __cdecl GetPanorama(const char* name);
        std::ptrdiff_t __cdecl GetParticles(const char* name);
        std::ptrdiff_t __cdecl GetPulseSystem(const char* name);
        std::ptrdiff_t __cdecl GetResourceSystem(const char* name);
        std::ptrdiff_t __cdecl GetSceneSystem(const char* name);
        std::ptrdiff_t __cdecl GetSchemaSystem(const char* name);
        std::ptrdiff_t __cdecl GetServer(const char* name);
        std::ptrdiff_t __cdecl GetSteamAudio(const char* name);
        std::ptrdiff_t __cdecl GetVphysics2(const char* name);
        std::ptrdiff_t __cdecl GetWorldRenderer(const char* name);
    }
}

int main()
{
    std::cout << "[*] Initializing CS2 Dumper..." << std::endl;

    if (!Dumper::Initialize())
    {
        std::cout << "[!] Failed to load offsets!" << std::endl;
        std::cout << "[*] Reason: " << Dumper::Initialize_Error() << std::endl;
        Sleep(5000);
        return 1;
    }

    std::cout << "[+] Offsets loaded successfully!" << std::endl;

    std::ptrdiff_t dwCSGOInput = Dumper::Get("client_dll.dwCSGOInput");
    std::ptrdiff_t dwInputSystem = Dumper::Get("inputsystem_dll.dwInputSystem");
    std::ptrdiff_t attackButton = Dumper::Get("buttons.attack");
    std::ptrdiff_t m_hPlayerPing = Dumper::GetClient("CCSPlayer_PingServices.m_hPlayerPing");

    std::cout << "[client_dll] dwCSGOInput = 0x"
        << std::hex << std::uppercase << dwCSGOInput << std::endl;
    std::cout << "[inputsystem_dll] dwInputSystem = 0x"
        << std::hex << std::uppercase << dwInputSystem << std::endl;
    std::cout << "[buttons] attack = 0x"
        << std::hex << std::uppercase << attackButton << std::endl;
    std::cout << "[CCSPlayer_PingServices] m_hPlayerPing = 0x"
        << std::hex << std::uppercase << m_hPlayerPing << std::endl;

    Dumper::Cleanup();
    Sleep(7500);
    return 0;
}
