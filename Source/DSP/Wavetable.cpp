#include "Wavetable.h"
#include <cmath>

Wavetable::Wavetable (std::vector<std::vector<float>> framesIn)
    : frames (std::move (framesIn))
{
    jassert (! frames.empty());
    frameSize = frames.empty() ? 0 : (int) frames.front().size();
}

float Wavetable::readFrame (int frameIndex, double phase) const noexcept
{
    if (frameSize == 0)
        return 0.0f;

    const auto& data = frames[(size_t) frameIndex];
    const double readPos = phase * (double) frameSize;
    const int i0 = ((int) readPos) % frameSize;
    const int i1 = (i0 + 1) % frameSize;
    const float frac = (float) (readPos - std::floor (readPos));

    return data[(size_t) i0] + frac * (data[(size_t) i1] - data[(size_t) i0]);
}

float Wavetable::getSample (double phase, float position) const noexcept
{
    if (frames.size() == 1)
        return readFrame (0, phase);

    const float scaled = juce::jlimit (0.0f, 1.0f, position) * (float) (frames.size() - 1);
    const int frameA = (int) scaled;
    const int frameB = juce::jmin (frameA + 1, (int) frames.size() - 1);
    const float frac = scaled - (float) frameA;

    const float a = readFrame (frameA, phase);
    const float b = readFrame (frameB, phase);
    return a + frac * (b - a);
}
