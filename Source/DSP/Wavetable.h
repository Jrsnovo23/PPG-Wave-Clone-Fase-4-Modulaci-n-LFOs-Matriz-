#pragma once

#include <vector>
#include <juce_core/juce_core.h>

/**
    Una tabla de onda con uno o varios "frames" (posiciones). Cada frame
    contiene un ciclo completo de la forma de onda, todos del mismo tamaño.
    Permite morphing continuo entre frames (interpolación de posición) y
    lectura con interpolación lineal de fase (para cualquier frecuencia).

    Fase 2: interpolación lineal simple, sin mip-mapping por octava. Esto
    puede introducir aliasing perceptible en notas agudas — es un
    comportamiento conocido, documentado en el README de esta fase. El
    propio documento de diseño original contempla el aliasing como un
    carácter "vintage" opcional (sección Digital Vintage Character), así
    que no es solo una limitación temporal: en fases posteriores se vuelve
    un control explícito (Vintage/Modern) en vez de solo un límite técnico.
*/
class Wavetable
{
public:
    explicit Wavetable (std::vector<std::vector<float>> framesIn);

    int getNumFrames() const noexcept { return (int) frames.size(); }
    int getFrameSize() const noexcept { return frameSize; }

    /** phase: 0..1 (posición dentro del ciclo). position: 0..1 (morph entre frames). */
    float getSample (double phase, float position) const noexcept;

private:
    std::vector<std::vector<float>> frames;
    int frameSize = 0;

    float readFrame (int frameIndex, double phase) const noexcept;
};
