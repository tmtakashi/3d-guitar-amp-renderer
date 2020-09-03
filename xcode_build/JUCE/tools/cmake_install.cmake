# Install script for directory: /Users/kazumawatanabe/3d-guitar-amp-renderer/JUCE

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-6.0.1" TYPE FILE FILES
    "/Users/kazumawatanabe/3d-guitar-amp-renderer/xcode_build/JUCE/tools/JUCEConfigVersion.cmake"
    "/Users/kazumawatanabe/3d-guitar-amp-renderer/xcode_build/JUCE/tools/JUCEConfig.cmake"
    "/Users/kazumawatanabe/3d-guitar-amp-renderer/JUCE/extras/Build/CMake/LaunchScreen.storyboard"
    "/Users/kazumawatanabe/3d-guitar-amp-renderer/JUCE/extras/Build/CMake/PIPAudioProcessor.cpp.in"
    "/Users/kazumawatanabe/3d-guitar-amp-renderer/JUCE/extras/Build/CMake/PIPComponent.cpp.in"
    "/Users/kazumawatanabe/3d-guitar-amp-renderer/JUCE/extras/Build/CMake/PIPConsole.cpp.in"
    "/Users/kazumawatanabe/3d-guitar-amp-renderer/JUCE/extras/Build/CMake/RecentFilesMenuTemplate.nib"
    "/Users/kazumawatanabe/3d-guitar-amp-renderer/JUCE/extras/Build/CMake/UnityPluginGUIScript.cs.in"
    "/Users/kazumawatanabe/3d-guitar-amp-renderer/JUCE/extras/Build/CMake/copyDir.cmake"
    "/Users/kazumawatanabe/3d-guitar-amp-renderer/JUCE/extras/Build/CMake/JUCEHelperTargets.cmake"
    "/Users/kazumawatanabe/3d-guitar-amp-renderer/JUCE/extras/Build/CMake/JUCEUtils.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/kazumawatanabe/3d-guitar-amp-renderer/xcode_build/JUCE/tools/modules/cmake_install.cmake")
  include("/Users/kazumawatanabe/3d-guitar-amp-renderer/xcode_build/JUCE/tools/extras/Build/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/Users/kazumawatanabe/3d-guitar-amp-renderer/xcode_build/JUCE/tools/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
