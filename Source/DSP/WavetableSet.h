#pragma once

#include "Wavetable.h"
#include <memory>
#include <vector>

/**
    Colección de Wavetables nombradas, seleccionables por índice desde un
    parámetro. En la Fase 2 solo existe un WavetableSet (la biblioteca
    "Básicas" del documento de diseño); las categorías Digitales,
    Evolutivas y Percusivas se añaden más adelante junto con la biblioteca
    de presets (Fase 6), probablemente como WavetableSets adicionales o
    ampliando este mismo.
*/
class WavetableSet
{
public:
    void addWavetable (const juce::String& name, std::unique_ptr<Wavetable> table);

    int getNumWavetables() const noexcept { return (int) tables.size(); }
    const juce::StringArray& getNames() const noexcept { return names; }
    const Wavetable& getWavetable (int index) const;

private:
    juce::StringArray names;
    std::vector<std::unique_ptr<Wavetable>> tables;
};
