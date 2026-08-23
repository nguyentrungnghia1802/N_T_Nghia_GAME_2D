#define SDL_MAIN_HANDLED
#include "../src/Profiler.h"
#include "../src/CommonFunc.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

namespace
{
void Expect(bool condition, const char *name)
{
    if (!condition)
    {
        std::cerr << "FAILED: " << name << '\n';
        std::exit(1);
    }
}
}

int main()
{
    const char *log_path = std::getenv("GAME_PROFILE_LOG");
    Expect(log_path != NULL && log_path[0] != '\0', "profile log path supplied");
    Expect(SDL_Init(SDL_INIT_TIMER) == 0, "SDL timer initializes");

    Profiler::Init();
    Profiler::StartInterval();
    const Uint32 start = SDL_GetTicks();
    while (SDL_GetTicks() - start < 5200)
    {
        Profiler::BeginFrame();
        Profiler::CountEntityUpdate();
        Profiler::CountEntityRender();
        Profiler::EndFrame();
        SDL_Delay(10);
    }

    std::ifstream log(log_path);
    std::string line;
    std::string last_line;
    while (std::getline(log, line))
    {
        last_line = line;
    }

    Expect(last_line.find("[profiler]") == 0, "portable profiler line emitted");
    Expect(last_line.find("updates_per_frame=") != std::string::npos, "frame counters retained");
    Expect(last_line.find("cpu_pct=") == std::string::npos, "Win32 CPU metric removed");
    Expect(last_line.find("ram_kb=") == std::string::npos, "Win32 RAM metric removed");
    Expect(last_line.find("threads=") == std::string::npos, "Win32 thread metric removed");

    SDL_Quit();
    std::cout << "profiler portability tests passed\n";
    return 0;
}
