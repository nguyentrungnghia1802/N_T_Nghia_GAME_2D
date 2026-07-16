#ifndef PROFILER_H_
#define PROFILER_H_

#include <cstdint>

namespace Profiler
{
    void Init();
    void StartInterval();
    void BeginFrame();
    void EndFrame();

    void CountEntityUpdate();
    void CountEntityRender();
    void CountCollisionCheck();
    void CountImageLoad();
    void CountTextureCreate();
    void CountFontLoad();
    void CountSoundLoad();
    void CountTextRender();
}

#endif
