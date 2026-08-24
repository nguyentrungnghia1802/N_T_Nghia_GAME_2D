#ifndef WINDOW_CONFIG_H_
#define WINDOW_CONFIG_H_

#include <algorithm>
#include <cmath>

namespace WindowConfig
{
constexpr int HORIZONTAL_MARGIN = 32;
constexpr int VERTICAL_MARGIN = 96;
constexpr int FALLBACK_WIDTH = 1280;
constexpr int FALLBACK_HEIGHT = 720;

struct Size
{
    int width;
    int height;
};

// Calculate a window size that preserves the logical render aspect ratio while
// leaving room for the desktop taskbar and window decorations.
inline Size CalculateWindowSize(const int usable_width,
                                const int usable_height,
                                const int logical_width,
                                const int logical_height) noexcept
{
    if (usable_width <= 0 || usable_height <= 0 || logical_width <= 0 || logical_height <= 0)
    {
        return {FALLBACK_WIDTH, FALLBACK_HEIGHT};
    }

    const int available_width = std::max(1, usable_width - HORIZONTAL_MARGIN);
    const int available_height = std::max(1, usable_height - VERTICAL_MARGIN);
    const double width_scale = static_cast<double>(available_width) / logical_width;
    const double height_scale = static_cast<double>(available_height) / logical_height;
    const double scale = std::min(1.0, std::min(width_scale, height_scale));

    if (!(scale > 0.0) || !std::isfinite(scale))
    {
        return {FALLBACK_WIDTH, FALLBACK_HEIGHT};
    }

    return {
        std::max(1, static_cast<int>(std::floor(logical_width * scale))),
        std::max(1, static_cast<int>(std::floor(logical_height * scale)))};
}
}

#endif
