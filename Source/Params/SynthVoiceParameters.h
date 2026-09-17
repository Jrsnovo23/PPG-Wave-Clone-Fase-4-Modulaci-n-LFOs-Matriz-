#pragma once

#include <array>
#include "OscillatorParameterPointers.h"
#include "FilterParameterPointers.h"
#include "EnvelopeParameterPointers.h"
#include "LFOParameterPointers.h"
#include "ModSlotParameterPointers.h"

/**
    Agrupa todos los punteros a parámetros que necesita una SynthVoice, para
    no pasar diez y tantos argumentos sueltos a su constructor. Se construye
    una vez en el PluginProcessor y se comparte (por referencia) entre todas
    las voces — son ajustes del patch, no por nota.
*/
struct SynthVoiceParameters
{
    OscillatorParameterPointers osc1, osc2;
    FilterParameterPointers filter;
    EnvelopeParameterPointers env1, env2;
    LFOParameterPointers lfo1, lfo2;
    std::array<ModSlotParameterPointers, 4> modSlots;
};
