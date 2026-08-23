#include "Profiler.h"

#include "CommonFunc.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#endif

namespace
{
    const double LOG_INTERVAL_MS = 5000.0;

    struct FrameCounters
    {
        std::uint64_t entity_updates = 0;
        std::uint64_t entity_renders = 0;
        std::uint64_t collision_checks = 0;
        std::uint64_t image_loads = 0;
        std::uint64_t texture_creates = 0;
        std::uint64_t font_loads = 0;
        std::uint64_t sound_loads = 0;
        std::uint64_t text_renders = 0;
    };

    struct ProcessStats
    {
        std::uint64_t memory_kb = 0;
        std::uint32_t thread_count = 0;
        double cpu_percent = 0.0;
    };

    bool initialized = false;
    Uint64 frequency = 0;
    Uint64 frame_start = 0;
    Uint64 last_log = 0;
    std::uint64_t frames = 0;
    double min_frame_ms = std::numeric_limits<double>::max();
    double max_frame_ms = 0.0;
    double total_frame_ms = 0.0;
    FrameCounters interval_counters;
    std::ofstream profile_log;

#ifdef _WIN32
    std::uint64_t previous_process_time_100ns = 0;
    Uint64 previous_cpu_sample = 0;
    std::uint32_t logical_processor_count = 1;
#endif

    double CounterToMs(Uint64 counter)
    {
        if (frequency == 0)
        {
            return 0.0;
        }
        return (static_cast<double>(counter) * 1000.0) / static_cast<double>(frequency);
    }

#ifdef _WIN32
    std::uint64_t FileTimeToUint64(const FILETIME &value)
    {
        ULARGE_INTEGER result;
        result.LowPart = value.dwLowDateTime;
        result.HighPart = value.dwHighDateTime;
        return result.QuadPart;
    }
#endif

    void PrimeCpuSample(Uint64 now)
    {
#ifdef _WIN32
        SYSTEM_INFO system_info;
        GetSystemInfo(&system_info);
        logical_processor_count = std::max<std::uint32_t>(1, system_info.dwNumberOfProcessors);

        FILETIME creation_time;
        FILETIME exit_time;
        FILETIME kernel_time;
        FILETIME user_time;
        if (GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time, &kernel_time, &user_time))
        {
            previous_process_time_100ns = FileTimeToUint64(kernel_time) + FileTimeToUint64(user_time);
            previous_cpu_sample = now;
        }
#else
        (void)now;
#endif
    }

    ProcessStats ReadProcessStats(Uint64 now)
    {
        ProcessStats stats;

#ifdef _WIN32
        PROCESS_MEMORY_COUNTERS memory_counters;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &memory_counters, sizeof(memory_counters)))
        {
            stats.memory_kb = static_cast<std::uint64_t>(memory_counters.WorkingSetSize / 1024);
        }

        FILETIME creation_time;
        FILETIME exit_time;
        FILETIME kernel_time;
        FILETIME user_time;
        if (GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time, &kernel_time, &user_time))
        {
            const std::uint64_t process_time_100ns = FileTimeToUint64(kernel_time) + FileTimeToUint64(user_time);
            const double wall_seconds = CounterToMs(now - previous_cpu_sample) / 1000.0;
            const double process_seconds = static_cast<double>(process_time_100ns - previous_process_time_100ns) / 10000000.0;
            if (previous_cpu_sample != 0 && wall_seconds > 0.0)
            {
                stats.cpu_percent = (process_seconds / wall_seconds) * 100.0 / static_cast<double>(logical_processor_count);
            }
            previous_process_time_100ns = process_time_100ns;
            previous_cpu_sample = now;
        }

        const DWORD process_id = GetCurrentProcessId();
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snapshot != INVALID_HANDLE_VALUE)
        {
            THREADENTRY32 entry;
            entry.dwSize = sizeof(entry);

            if (Thread32First(snapshot, &entry))
            {
                do
                {
                    if (entry.th32OwnerProcessID == process_id)
                    {
                        stats.thread_count++;
                    }
                    entry.dwSize = sizeof(entry);
                } while (Thread32Next(snapshot, &entry));
            }

            CloseHandle(snapshot);
        }
#endif

        return stats;
    }

    void ResetInterval(Uint64 now)
    {
        last_log = now;
        frames = 0;
        min_frame_ms = std::numeric_limits<double>::max();
        max_frame_ms = 0.0;
        total_frame_ms = 0.0;
        interval_counters = FrameCounters();
    }
}

void Profiler::Init()
{
    const char *profile_log_path = std::getenv("GAME_PROFILE_LOG");
    if (profile_log_path != NULL && profile_log_path[0] != '\0')
    {
        profile_log.open(profile_log_path, std::ios::out | std::ios::trunc);
    }

    frequency = SDL_GetPerformanceFrequency();
    const Uint64 now = SDL_GetPerformanceCounter();
    frame_start = now;
    ResetInterval(now);
    PrimeCpuSample(now);
    initialized = true;
}

void Profiler::StartInterval()
{
    if (!initialized)
    {
        Init();
        return;
    }

    const Uint64 now = SDL_GetPerformanceCounter();
    last_log = now;
    frames = 0;
    min_frame_ms = std::numeric_limits<double>::max();
    max_frame_ms = 0.0;
    total_frame_ms = 0.0;
    PrimeCpuSample(now);
}

void Profiler::BeginFrame()
{
    if (!initialized)
    {
        Init();
    }
    frame_start = SDL_GetPerformanceCounter();
}

void Profiler::EndFrame()
{
    if (!initialized)
    {
        return;
    }

    const Uint64 now = SDL_GetPerformanceCounter();
    const double frame_ms = CounterToMs(now - frame_start);

    frames++;
    min_frame_ms = std::min(min_frame_ms, frame_ms);
    max_frame_ms = std::max(max_frame_ms, frame_ms);
    total_frame_ms += frame_ms;

    const double elapsed_ms = CounterToMs(now - last_log);
    if (elapsed_ms < LOG_INTERVAL_MS)
    {
        return;
    }

    const double avg_frame_ms = frames > 0 ? total_frame_ms / static_cast<double>(frames) : 0.0;
    const double avg_fps = elapsed_ms > 0.0 ? (static_cast<double>(frames) * 1000.0) / elapsed_ms : 0.0;
    const double min_fps = max_frame_ms > 0.0 ? 1000.0 / max_frame_ms : 0.0;
    const double updates_per_frame = frames > 0 ? static_cast<double>(interval_counters.entity_updates) / static_cast<double>(frames) : 0.0;
    const double renders_per_frame = frames > 0 ? static_cast<double>(interval_counters.entity_renders) / static_cast<double>(frames) : 0.0;
    const double collisions_per_frame = frames > 0 ? static_cast<double>(interval_counters.collision_checks) / static_cast<double>(frames) : 0.0;
    const ProcessStats process_stats = ReadProcessStats(now);

    std::ostringstream output;
    output << std::fixed << std::setprecision(2)
           << "[profiler] frames=" << frames
           << " fps_avg=" << avg_fps
           << " fps_min_est=" << min_fps
           << " frame_ms_min=" << min_frame_ms
           << " frame_ms_avg=" << avg_frame_ms
           << " frame_ms_max=" << max_frame_ms
           << " updates_per_frame=" << updates_per_frame
           << " renders_per_frame=" << renders_per_frame
           << " collisions_per_frame=" << collisions_per_frame
           << " img_loads=" << interval_counters.image_loads
           << " texture_creates=" << interval_counters.texture_creates
           << " font_loads=" << interval_counters.font_loads
           << " sound_loads=" << interval_counters.sound_loads
           << " text_renders=" << interval_counters.text_renders
           << " cpu_pct=" << process_stats.cpu_percent
           << " ram_kb=" << process_stats.memory_kb
           << " threads=" << process_stats.thread_count;

    std::cout << output.str() << std::endl;
    if (profile_log.is_open())
    {
        profile_log << output.str() << std::endl;
    }

    ResetInterval(now);
}

void Profiler::CountEntityUpdate()
{
    interval_counters.entity_updates++;
}

void Profiler::CountEntityRender()
{
    interval_counters.entity_renders++;
}

void Profiler::CountCollisionCheck()
{
    interval_counters.collision_checks++;
}

void Profiler::CountImageLoad()
{
    interval_counters.image_loads++;
}

void Profiler::CountTextureCreate()
{
    interval_counters.texture_creates++;
}

void Profiler::CountFontLoad()
{
    interval_counters.font_loads++;
}

void Profiler::CountSoundLoad()
{
    interval_counters.sound_loads++;
}

void Profiler::CountTextRender()
{
    interval_counters.text_renders++;
}
