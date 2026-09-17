cmake_minimum_required(VERSION 3.22)

# ------------------------------------------------------------------
# IMPORTANTE: los runners "macos-14"/"macos-latest" de GitHub Actions corren
# en hardware Apple Silicon (arm64). Sin esto, CMake compila por defecto para
# la arquitectura del propio runner (arm64) — un binario que simplemente NO
# puede correr en un Mac Intel como el Mac Pro 5,1. Esto debe fijarse ANTES
# de la primera llamada a project(), o no tiene efecto.
# ------------------------------------------------------------------
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Arquitecturas de macOS a compilar" FORCE)
endif()

project(PPGWaveClone VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------
# JUCE debe estar disponible como subcarpeta "JUCE" en la raíz de este
# proyecto (al mismo nivel que este CMakeLists.txt). Para obtenerlo:
#
#   git clone --depth 1 --branch 7.0.12 https://github.com/juce-framework/JUCE.git
#
# Estructura esperada:
#   PPGWaveClone/
#     CMakeLists.txt   <- este archivo
#     JUCE/             <- clonado aquí
#     Source/
# ------------------------------------------------------------------
add_subdirectory(JUCE)

juce_add_plugin(PPGWaveClone
    COMPANY_NAME                "TuNombreOMarca"
    PLUGIN_MANUFACTURER_CODE    Ppgc
    PLUGIN_CODE                 Pwc1
    FORMATS                     VST3
    PRODUCT_NAME                "PPG Wave Clone"
    IS_SYNTH                    TRUE
    NEEDS_MIDI_INPUT            TRUE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES             "Instrument" "Synth"
)

target_sources(PPGWaveClone
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/Params/ParameterLayout.cpp
        Source/DSP/Wavetable.cpp
        Source/DSP/WavetableSet.cpp
        Source/DSP/WavetableFactory.cpp
        Source/DSP/Filter.cpp
        Source/DSP/Envelope.cpp
        Source/DSP/LFO.cpp
        Source/DSP/ModulationMatrix.cpp
        Source/Synth/SynthVoice.cpp
)

target_include_directories(PPGWaveClone
    PRIVATE
        Source
)

target_compile_definitions(PPGWaveClone
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(PPGWaveClone
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# ------------------------------------------------------------------
# Compatibilidad con Macs Intel viejas: por defecto Xcode/Clang no exige
# AVX/AVX2, pero lo forzamos explícitamente para evitar sorpresas si algún
# módulo de JUCE (p.ej. juce_dsp) intentara usarlo en fases futuras. Muchos
# crashes de "illegal instruction" en hosts como Ableton vienen de plugins
# compilados asumiendo AVX2 en CPUs que no lo tienen.
# ------------------------------------------------------------------
if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" OR CMAKE_OSX_ARCHITECTURES MATCHES "x86_64")
    target_compile_options(PPGWaveClone PRIVATE -mno-avx -mno-avx2)
endif()
