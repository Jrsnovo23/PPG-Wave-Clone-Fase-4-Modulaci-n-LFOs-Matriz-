# PPG Wave Clone — Fase 4: Modulación (LFOs + Matriz)

Implementación original de un instrumento virtual VST3 de síntesis wavetable
inspirado conceptualmente en el PPG Wave 3.3. No contiene ROMs, muestras,
gráficos ni código propietario de terceros.

## Estado de esta entrega

**Fases 1-3**: listo y confirmado sonando en hardware real (arquitectura,
osciladores wavetable, filtro + envolventes ADSR).

**Fase 4** (esta entrega): 2 LFOs + matriz de modulación de 4 rutas.

- `LFO`: 7 formas de onda (Sine, Triangle, Square, Saw Up, Saw Down, Random,
  Sample & Hold), rate libre en Hz o sincronizado al tempo del host (7
  divisiones, de 1/16 a 4 compases), retrigger opcional, fase inicial, depth.
- `ModulationMatrix`: 4 rutas fijas, cada una con Source / Destination /
  Amount. Fuentes: LFO1, LFO2, Envelope1, Envelope2, Velocity, Mod Wheel
  (CC1), Aftertouch, Note Number, Random (fijo por nota). Destinos: OSC1/OSC2
  Pitch, OSC1/OSC2 Wave Position, Filter Cutoff, Amplifier.
- `SynthVoiceParameters` agrupa todos los punteros de parámetros (osc,
  filtro, envolventes, LFOs, slots de modulación) en una sola estructura —
  refactor sobre la Fase 3 para no pasar 10+ argumentos sueltos al
  constructor de `SynthVoice`.
- El tempo del host (BPM) se lee una vez por bloque en `PluginProcessor` vía
  `AudioPlayHead` y se propaga a los LFOs de cada voz.
- GUI ampliada con paneles LFO1, LFO2, y la matriz de modulación (4 filas).

**Lo que NO incluye todavía**: efectos (chorus/delay/reverb/drive), gestión
de presets, carácter "vintage", interfaz gráfica definitiva.

## Problemas conocidos

- **Sync y Retrigger de cada LFO no tienen knob dedicado en esta GUI
  provisional** (ya son 7 parámetros por LFO — no entraban cómodos). Son
  parámetros reales y automatizables: se pueden mover desde la lista de
  parámetros/automatización de Ableton Live. La Fase 7 les da su lugar en
  la interfaz final.
- **Fuentes/destinos pendientes** (documentados en `DSP/ModulationMatrix.h`):
  "MIDI CC" genérico (cualquier número de controlador, no solo Mod Wheel) y
  "Key Tracking" como fuente separada de Note Number; como destinos, Fine
  Pitch, Filter Resonance, Pan y LFO Rate. Ninguno es estrictamente
  necesario para que la modulación funcione — son ampliaciones directas
  sobre la misma arquitectura cuando se necesiten.
- **Solo 4 rutas de modulación** en vez de una lista abierta — es la forma
  práctica de implementar una matriz con parámetros de plugin estáticos
  (automatizables por host). Ampliar el número de slots es un cambio
  mecánico, no arquitectónico.
- **Sensible a la versión exacta de JUCE**: `channelPressureChanged`,
  `aftertouchChanged` y `AudioPlayHead::getPosition()/getBpm()` son API de
  JUCE relativamente recientes. Con JUCE 7.0.12 (la versión que este
  proyecto clona) deberían compilar sin problema; si el compilador se queja
  de que alguno de esos métodos no es virtual en la clase base, es una señal
  de version mismatch — mándame el error exacto y lo ajustamos.
- Mismo aliasing en notas agudas que en fases anteriores (comportamiento
  esperado, no un bug).

## Compilación sin instalar nada (recomendado)

Este proyecto incluye `.github/workflows/build.yml`: compila el VST3 en un
Mac con Xcode ya instalado, en la nube, gratis, vía GitHub Actions. Sube la
carpeta a un repositorio (con GitHub Desktop, para no perder `.github` por
ser una carpeta oculta), espera la palomita verde en la pestaña "Actions", y
descarga el `.vst3` desde "Artifacts".

## Compilación local (requiere CMake + compilador C++20)

1. Clona JUCE dentro de esta carpeta:
   ```
   git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git
   ```
   Debe quedar como `PPGWaveClone/JUCE/`.

2. Configura y compila:

   **macOS** (Xcode instalado):
   ```
   cmake -B build -G Xcode
   cmake --build build --config Release
   ```
   El `.vst3` queda en `build/PPGWaveClone_artefacts/Release/VST3/`.
   Cópialo a `~/Library/Audio/Plug-Ins/VST3/` (tu usuario) o
   `/Library/Audio/Plug-Ins/VST3/` (todo el sistema — recomendado si Ableton
   corre en otra sesión de usuario). Si lo bajaste de un navegador:
   ```
   xattr -dr com.apple.quarantine "/ruta/al/PPG Wave Clone.vst3"
   ```

   **Windows** (Visual Studio 2022):
   ```
   cmake -B build -G "Visual Studio 17 2022"
   cmake --build build --config Release
   ```
   El `.vst3` queda en `build/PPGWaveClone_artefacts/Release/VST3/`.
   Cópialo a `C:\Program Files\Common Files\VST3\`.

3. Rescanea plugins en Ableton Live (Preferences → Plug-ins → Rescan).

## Estructura

```
PPGWaveClone/
  CMakeLists.txt
  .github/workflows/build.yml   — compila el VST3 en la nube (macOS, x86_64)
  Source/
    PluginProcessor.h/.cpp      — AudioProcessor, APVTS, Synthesiser, tempo del host
    PluginEditor.h/.cpp         — GUI (Fase 4)
    Params/
      ParameterIDs.h
      ParameterLayout.h/.cpp
      OscillatorParameterPointers.h
      FilterParameterPointers.h
      EnvelopeParameterPointers.h
      LFOParameterPointers.h
      ModSlotParameterPointers.h
      SynthVoiceParameters.h      — agrupa todos los punteros de una voz
    DSP/
      Wavetable.h/.cpp
      WavetableSet.h/.cpp
      WavetableFactory.h/.cpp
      Oscillator.h
      Filter.h/.cpp
      Envelope.h/.cpp
      LFO.h/.cpp                  — 7 formas de onda, sync a tempo
      ModulationMatrix.h/.cpp     — fuentes, destinos, rutas
    Synth/
      SynthSound.h
      SynthVoice.h/.cpp            — OSC1+OSC2 -> Filter -> salida, con LFOs y matriz
```

## Próxima fase (Fase 5 — Efectos)

- `Source/DSP/Effects/Chorus.h/.cpp`, `Delay.h/.cpp`, `Reverb.h/.cpp`,
  `Drive.h/.cpp` (o un solo `Effects.h/.cpp` si conviene más simple).
- Cadena de efectos a nivel de plugin (post-mezcla de voces, no por voz).
- Cada efecto con su propio bypass on/off.
- Nuevos parámetros de efectos en `ParameterIDs.h` / `ParameterLayout.cpp`.

Dime cuándo avanzamos a la Fase 5.
