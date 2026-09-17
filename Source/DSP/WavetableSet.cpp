#include "WavetableSet.h"

void WavetableSet::addWavetable (const juce::String& name, std::unique_ptr<Wavetable> table)
{
    names.add (name);
    tables.push_back (std::move (table));
}

const Wavetable& WavetableSet::getWavetable (int index) const
{
    const int safeIndex = juce::jlimit (0, (int) tables.size() - 1, index);
    return *tables[(size_t) safeIndex];
}
