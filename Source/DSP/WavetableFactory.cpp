#include "WavetableFactory.h"
#include <cmath>
#include <functional>

namespace
{
    constexpr int frameSize = 2048;
    // Límite práctico de armónicos para reducir (no eliminar) aliasing en
    // registros medios/graves. El aliasing residual en notas muy agudas es
    // un comportamiento conocido de la Fase 2 (ver README).
    constexpr int numHarmonicsBandlimited = 32;

    std::vector<float> makeHarmonicFrame (const std::function<float (int harmonic)>& amplitudeForHarmonic,
                                           int numHarmonics)
    {
        std::vector<float> data (frameSize, 0.0f);

        for (int h = 1; h <= numHarmonics; ++h)
        {
            const float amp = amplitudeForHarmonic (h);
            if (amp == 0.0f)
                continue;

            for (int n = 0; n < frameSize; ++n)
            {
                const double phase = (double) n / (double) frameSize;
                data[(size_t) n] += amp * (float) std::sin (juce::MathConstants<double>::twoPi * (double) h * phase);
            }
        }

        float peak = 0.0f;
        for (auto v : data)
            peak = juce::jmax (peak, std::abs (v));

        if (peak > 0.0f)
            for (auto& v : data)
                v /= peak;

        return data;
    }

    std::vector<float> makeSine()
    {
        return makeHarmonicFrame ([] (int h) { return h == 1 ? 1.0f : 0.0f; }, 1);
    }

    std::vector<float> makeSaw()
    {
        return makeHarmonicFrame ([] (int h) { return 1.0f / (float) h; }, numHarmonicsBandlimited);
    }

    std::vector<float> makeSquare()
    {
        return makeHarmonicFrame ([] (int h) { return (h % 2 == 1) ? 1.0f / (float) h : 0.0f; }, numHarmonicsBandlimited);
    }

    std::vector<float> makeTriangle()
    {
        return makeHarmonicFrame ([] (int h)
        {
            if (h % 2 == 0)
                return 0.0f;

            const float sign = (((h - 1) / 2) % 2 == 0) ? 1.0f : -1.0f;
            return sign / (float) (h * h);
        }, numHarmonicsBandlimited);
    }

    std::vector<float> makePulse (float dutyCycle)
    {
        std::vector<float> data (frameSize, 0.0f);
        const int highSamples = (int) (dutyCycle * (float) frameSize);

        for (int n = 0; n < frameSize; ++n)
            data[(size_t) n] = (n < highSamples) ? 1.0f : -1.0f;

        return data;
    }
}

const juce::StringArray& WavetableFactory::getDefaultWavetableNames()
{
    static const juce::StringArray names { "Sine", "Triangle", "Saw", "Square", "PWM", "Harmonic Morph" };
    return names;
}

std::unique_ptr<WavetableSet> WavetableFactory::createDefaultSet()
{
    auto set = std::make_unique<WavetableSet>();
    const auto& names = getDefaultWavetableNames();

    set->addWavetable (names[0], std::make_unique<Wavetable> (std::vector<std::vector<float>> { makeSine() }));
    set->addWavetable (names[1], std::make_unique<Wavetable> (std::vector<std::vector<float>> { makeTriangle() }));
    set->addWavetable (names[2], std::make_unique<Wavetable> (std::vector<std::vector<float>> { makeSaw() }));
    set->addWavetable (names[3], std::make_unique<Wavetable> (std::vector<std::vector<float>> { makeSquare() }));

    // PWM: varios frames morphing el duty cycle de 5% a 95%
    {
        std::vector<std::vector<float>> pwmFrames;
        constexpr int numPwmFrames = 8;
        for (int i = 0; i < numPwmFrames; ++i)
        {
            const float duty = juce::jmap ((float) i, 0.0f, (float) (numPwmFrames - 1), 0.05f, 0.95f);
            pwmFrames.push_back (makePulse (duty));
        }
        set->addWavetable (names[4], std::make_unique<Wavetable> (std::move (pwmFrames)));
    }

    // Harmonic Morph: de seno puro a un timbre rico en armónicos — demuestra
    // el escaneo de wavetable / cambio de timbre en tiempo real pedido en
    // la sección 3 del documento de diseño.
    {
        std::vector<std::vector<float>> morphFrames;
        constexpr int numMorphFrames = 8;
        for (int i = 0; i < numMorphFrames; ++i)
        {
            const float amount = (float) i / (float) (numMorphFrames - 1);
            morphFrames.push_back (makeHarmonicFrame ([amount] (int h)
            {
                return h == 1 ? 1.0f : (amount / (float) h);
            }, numHarmonicsBandlimited));
        }
        set->addWavetable (names[5], std::make_unique<Wavetable> (std::move (morphFrames)));
    }

    return set;
}
