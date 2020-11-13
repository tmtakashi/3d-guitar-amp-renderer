cmake_minimum_required(VERSION 3.15)
project(3D_GUIAR_AMP_RENDERER VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(JUCE)                    # If you've put JUCE in a subdirectory called JUCE

juce_add_plugin(3DGuitarAmpRenderer
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME "Kawahara Lab"                          # Specify the name of the plugin's author
    # IS_SYNTH TRUE/FALSE                       # Is this a synth or an effect?
    # NEEDS_MIDI_INPUT TRUE/FALSE               # Does the plugin need midi input?
    # NEEDS_MIDI_OUTPUT TRUE/FALSE              # Does the plugin need midi output?
    # IS_MIDI_EFFECT TRUE/FALSE                 # Is this plugin a MIDI effect?
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD TRUE        # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Juce               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Dem0                            # A unique four-character plugin id with at least one upper-case character
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "3DGuitarAmpRenderer")        # The name of the final executable, which can differ from the target name

    add_subdirectory(assets)
    juce_add_binary_data(3DGuitarAmpRendererBinaryData
    SOURCES
    assets/amp.png
    assets/head.png)

target_sources(3DGuitarAmpRenderer PRIVATE
    Source/PluginEditor.cpp
    Source/PluginProcessor.cpp
    Source/AzimuthDialLookAndFeel.cpp
    Source/Convolver.cpp)

target_compile_definitions(3DGuitarAmpRenderer 
    PUBLIC
    # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
    JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
    JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_include_directories(3DGuitarAmpRenderer PRIVATE ${PROJECT_SOURCE_DIR}/include)


target_link_libraries(3DGuitarAmpRenderer PRIVATE
    # AudioPluginData           # If we'd created a binary data target, we'd link to it here
    3DGuitarAmpRendererBinaryData
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_osc
    )

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(3DGuitarAmpRenderer PRIVATE
        ${PROJECT_SOURCE_DIR}/lib/fftw-3.3.8/libfftw3f.dylib
    )
endif()
if (WIN32)
    target_link_libraries(3DGuitarAmpRenderer PRIVATE
        ${PROJECT_SOURCE_DIR}/lib/fftw-3.3.8/libfftw3f-3.dll
    )
endif()


include(CTest)
enable_testing()
include(CPack)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
