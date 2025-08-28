#include "Dump.hpp"

#define _WIN32_WINNT 0x0600

#pragma comment(lib, "wininet.lib")

namespace Offsets
{
    std::unordered_map<std::string, std::ptrdiff_t> table;
    std::unordered_map<std::string, std::unordered_map<std::string, std::ptrdiff_t>> perFile;
}

static BOOL ConnectedToInternet()
{
    return InternetCheckConnectionA(
        "http://www.msftconnecttest.com/connecttest.txt",
        FLAG_ICC_FORCE_CONNECTION,
        0
    );
}

static std::string DownloadFile(const char* url)
{
    if (!url) return {};

    HINTERNET hInternet = InternetOpenA("OffsetFetcher", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet)
        return {};

    HINTERNET hUrl = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hUrl) {
        InternetCloseHandle(hInternet);
        return {};
    }

    std::string result;
    char buffer[4096];
    DWORD bytesRead = 0;

    while (true) {
        if (!InternetReadFile(hUrl, buffer, sizeof(buffer), &bytesRead)) {
            result.clear();
            break;
        }
        if (bytesRead == 0) break;
        result.append(buffer, static_cast<size_t>(bytesRead));
    }

    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);

    return result;
}


static VOID ParseOffsets(const std::string& data, const std::string& fileKey)
{
    std::istringstream iss(data);
    std::string line;
    std::vector<std::string> nsStack;

    std::regex nsRegex(R"(^\s*namespace\s+([\w\d_]+))");
    std::regex constRegex(R"(^\s*constexpr\s+std::ptrdiff_t\s+([\w\d_]+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)(?:[uUlL]*)\s*;?)");

    const std::unordered_set<std::string> ignoreNamespaces = { "cs2_dumper", "offsets", "schemas" };

    while (std::getline(iss, line))
    {
        line.erase(0, line.find_first_not_of(" \t"));
        if (line.empty() || line.rfind("//", 0) == 0)
            continue;

        std::smatch match;

        if (std::regex_search(line, match, nsRegex)) {
            std::string nsName = match[1];
            if (ignoreNamespaces.find(nsName) == ignoreNamespaces.end())
                nsStack.push_back(nsName);
            continue;
        }

        if (line.find("}") != std::string::npos && !nsStack.empty()) {
            nsStack.pop_back();
            continue;
        }

        if (std::regex_search(line, match, constRegex)) {
            std::string fullName;
            for (const auto& n : nsStack) {
                if (!fullName.empty()) fullName += ".";
                fullName += n;
            }
            if (!fullName.empty()) fullName += ".";
            fullName += match[1];

            try {
                auto value = std::stoull(match[2], nullptr, 0); // numeric value
                Offsets::table[fullName] = static_cast<std::ptrdiff_t>(value);
                Offsets::perFile[fileKey][fullName] = static_cast<std::ptrdiff_t>(value);
            }
            catch (...) {
                // ignore parse errors
            }
        }
    }
}

extern "C" {
    static std::string g_LastInitError;

    EXPORT_API std::string Initialize_Error()
    {
        if (g_LastInitError.empty() || g_LastInitError.size() == 0)
            return "No error";
        return g_LastInitError;
    }

    EXPORT_API BOOL Initialize()
    {
        g_LastInitError.clear();

        if (!ConnectedToInternet()) {
            g_LastInitError = "No internet connection.";
            return FALSE;
        }

        struct URLFile { const char* url; const char* key; };
        std::vector<URLFile> urls = {
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/animationsystem_dll.hpp", "AnimationSystem" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/buttons.hpp", "Buttons" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/client_dll.hpp", "Client" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/engine2_dll.hpp", "Engine2" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/refs/heads/main/output/interfaces.hpp", "Interfaces" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/inputsystem_dll.hpp", "InputSystem" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/matchmaking_dll.hpp", "Matchmaking" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/materialsystem2_dll.hpp", "materialsystem2" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/offsets.hpp", "Offsets" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/panorama_dll.hpp", "Panorama" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/particles_dll.hpp", "Particles" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/pulse_system_dll.hpp", "PulseSystem" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/resourcesystem_dll.hpp", "ResourceSystem" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/rendersystemdx11_dll.hpp", "RenderSystemDX11" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/scenesystem_dll.hpp", "SceneSystem" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/schemasystem_dll.hpp", "SchemaSystem" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/server_dll.hpp", "Server" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/soundsystem_dll.hpp", "SoundSystem" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/steamaudio_dll.hpp", "SteamAudio" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/host_dll.hpp", "Host" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/networksystem_dll.hpp", "NetworkSystem" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/vphysics2_dll.hpp", "Vphysics2" },
            { "https://raw.githubusercontent.com/a2x/cs2-dumper/main/output/worldrenderer_dll.hpp", "WorldRenderer" }
        };

        bool anySuccess = false;

        for (auto& uf : urls) {
            std::string file = DownloadFile(uf.url);
            if (file.empty()) {
                g_LastInitError += "Failed to fetch: ";
                g_LastInitError += uf.url;
                g_LastInitError += "\n";
                continue;
            }
            ParseOffsets(file, uf.key);
            anySuccess = true;
        }

        if (!anySuccess) {
            if (g_LastInitError.empty())
                g_LastInitError = "Failed to fetch any offsets.";
            return FALSE;
        }

        return TRUE;
    }

    EXPORT_API VOID Cleanup()
    {
        for (auto& [file, map] : Offsets::perFile) {
            map.clear();
            map.rehash(0);
        }

        Offsets::perFile.clear();
        Offsets::perFile.rehash(0);

        Offsets::table.clear();
        Offsets::table.rehash(0);

        g_LastInitError.clear();
    }

    EXPORT_API std::ptrdiff_t Get(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::table.find(name);
        return it != Offsets::table.end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetOffsets(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["Offsets"].find(name);
        return it != Offsets::perFile["Offsets"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetClient(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["Client"].find(name);
        return it != Offsets::perFile["Client"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetAnimationSystem(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["AnimationSystem"].find(name);
        return it != Offsets::perFile["AnimationSystem"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetButtons(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["Buttons"].find(name);
        return it != Offsets::perFile["Buttons"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetEngine2(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["Engine2"].find(name);
        return it != Offsets::perFile["Engine2"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetInputSystem(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["InputSystem"].find(name);
        return it != Offsets::perFile["InputSystem"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetMatchmaking(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["Matchmaking"].find(name);
        return it != Offsets::perFile["Matchmaking"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetMaterialSystem(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["MaterialSystem"].find(name);
        return it != Offsets::perFile["MaterialSystem"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetRenderSystemDX11(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["RenderSystemDX11"].find(name);
        return it != Offsets::perFile["RenderSystemDX11"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetSoundSystem(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["SoundSystem"].find(name);
        return it != Offsets::perFile["SoundSystem"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetHost(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["Host"].find(name);
        return it != Offsets::perFile["Host"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetInterfaces(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["Interfaces"].find(name);
        return it != Offsets::perFile["Interfaces"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetNetworkSystem(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["NetworkSystem"].find(name);
        return it != Offsets::perFile["NetworkSystem"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetPanorama(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["Panorama"].find(name);
        return it != Offsets::perFile["Panorama"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetParticles(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["Particles"].find(name);
        return it != Offsets::perFile["Particles"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetPulseSystem(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["PulseSystem"].find(name);
        return it != Offsets::perFile["PulseSystem"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetResourceSystem(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["ResourceSystem"].find(name);
        return it != Offsets::perFile["ResourceSystem"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetSceneSystem(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["SceneSystem"].find(name);
        return it != Offsets::perFile["SceneSystem"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetSchemaSystem(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["SchemaSystem"].find(name);
        return it != Offsets::perFile["SchemaSystem"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetServer(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["Server"].find(name);
        return it != Offsets::perFile["Server"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetSteamAudio(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["SteamAudio"].find(name);
        return it != Offsets::perFile["SteamAudio"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetVphysics2(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["Vphysics2"].find(name);
        return it != Offsets::perFile["Vphysics2"].end() ? it->second : 0;
    }

    EXPORT_API std::ptrdiff_t GetWorldRenderer(const char* name)
    {
        if (!name) return 0;
        auto it = Offsets::perFile["WorldRenderer"].find(name);
        return it != Offsets::perFile["WorldRenderer"].end() ? it->second : 0;
    }
}