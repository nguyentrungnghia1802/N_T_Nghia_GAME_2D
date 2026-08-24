#include <cassert>
#include <cmath>
#include <iostream>

#include "../src/WindowConfig.h"

namespace
{
void AssertAspectRatio(const WindowConfig::Size size, const int logical_width, const int logical_height)
{
    const double actual_ratio = static_cast<double>(size.width) / size.height;
    const double logical_ratio = static_cast<double>(logical_width) / logical_height;
    assert(std::abs(actual_ratio - logical_ratio) < 0.01);
}
}

int main()
{
    constexpr int logical_width = 1422;
    constexpr int logical_height = 800;

    const WindowConfig::Size large = WindowConfig::CalculateWindowSize(1920, 1080, logical_width, logical_height);
    assert(large.width == logical_width);
    assert(large.height == logical_height);
    AssertAspectRatio(large, logical_width, logical_height);

    const WindowConfig::Size twelve_inch_class = WindowConfig::CalculateWindowSize(1366, 768, logical_width, logical_height);
    assert(twelve_inch_class.width <= 1366 - WindowConfig::HORIZONTAL_MARGIN);
    assert(twelve_inch_class.height <= 768 - WindowConfig::VERTICAL_MARGIN);
    AssertAspectRatio(twelve_inch_class, logical_width, logical_height);

    const WindowConfig::Size narrow = WindowConfig::CalculateWindowSize(1024, 600, logical_width, logical_height);
    assert(narrow.width <= 1024 - WindowConfig::HORIZONTAL_MARGIN);
    assert(narrow.height <= 600 - WindowConfig::VERTICAL_MARGIN);
    AssertAspectRatio(narrow, logical_width, logical_height);

    const WindowConfig::Size fallback = WindowConfig::CalculateWindowSize(0, 0, logical_width, logical_height);
    assert(fallback.width == WindowConfig::FALLBACK_WIDTH);
    assert(fallback.height == WindowConfig::FALLBACK_HEIGHT);

    std::cout << "window_config_tests: passed\n";
    return 0;
}
