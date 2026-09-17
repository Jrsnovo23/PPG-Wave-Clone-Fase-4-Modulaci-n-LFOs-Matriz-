#include "Envelope.h"

void Envelope::setSampleRate (double newSampleRate) noexcept
{
    sampleRate = newSampleRate;
    recalculate();
}

void Envelope::recalculate() noexcept
{
    if (sampleRate <= 0.0)
        return;

    attackRate  = 1.0f / (float) (attackTime  * sampleRate);
    decayRate   = 1.0f / (float) (decayTime   * sampleRate);
    releaseRate = 1.0f / (float) (releaseTime * sampleRate);
}

void Envelope::noteOn() noexcept
{
    stage = Stage::attack;
}

void Envelope::noteOff() noexcept
{
    if (stage != Stage::idle)
        stage = Stage::release;
}

float Envelope::getNextSample() noexcept
{
    switch (stage)
    {
        case Stage::idle:
            return 0.0f;

        case Stage::attack:
            currentLevel += attackRate;
            if (currentLevel >= 1.0f)
            {
                currentLevel = 1.0f;
                stage = Stage::decay;
            }
            break;

        case Stage::decay:
            currentLevel -= decayRate;
            if (currentLevel <= sustainLevel)
            {
                currentLevel = sustainLevel;
                stage = Stage::sustain;
            }
            break;

        case Stage::sustain:
            currentLevel = sustainLevel;
            break;

        case Stage::release:
            currentLevel -= releaseRate;
            if (currentLevel <= 0.0f)
            {
                currentLevel = 0.0f;
                stage = Stage::idle;
            }
            break;
    }

    return currentLevel;
}
