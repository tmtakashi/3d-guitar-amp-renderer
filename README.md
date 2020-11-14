# 3D Guitar Amp Renderer (for AES Design Competition)

## Overview
<img width="598" alt="Screen Shot 2020-11-09 at 20 31 55" src="https://user-images.githubusercontent.com/38929720/98536325-f723f880-22ca-11eb-829b-17d77575326c.png">

**This plugin received the AES Show 2020 Student Design Competion Silver Award!!!!**

https://www.aes.org/students/blog/2020/10/winners-of-student-competitions

The proposal sheet of the project can be found in this repository (since it is left as we submitted to the competition organizer, some implementation details are updated).

https://github.com/tmtakashi/AES149StudentDesignCompetionProposal

Click [here](https://drive.google.com/file/d/1wqahCdYpgdyeTjjDfpuPT6Urn117gXyU/view?usp=sharing) to watch the demo video presented at the AES Show.


The following libraries helped to build this application.

- https://github.com/juce-framework/JUCE
- https://github.com/AppliedAcousticsChalmers/sound_field_analysis-py
- [FFTW](http://www.fftw.org/)

For people who can read Japanese, the following repository might help you to understand the 
science behind the binaural rendering techonology used in this project.

https://github.com/tmtakashi/spherical-array-binaural-survey

## Building

Currently, it only works on macOS Catalina. 
I'll check how to make it work on other platforms
CMake can be used to build this plugin.
Follow the instructions in the following link if you don't have one on your machine.

https://cmake.org/install/

Here are the build commands for the plugin.

```
git clone --recursive https://github.com/tmtakashi/3d-guitar-amp-renderer
cd 3d-guitar-amp-renderer
cmake -Bbuild .
cmake --build ./build --config Release --target 3DGuitarAmpRenderer_VST3
```

## Usage

1. Launch your VST3 compatible DAW (we've only tested with REAPER), and insert `3DGuitarAmpRenderer` to your track.
2. Select `brirs/listening_room_brirs` or `brirs/tajigen_hall_brirs` from the file selector.
3. Get [GyrOSC](https://apps.apple.com/us/app/gyrosc/id418751595) on your iPhone, and set the `Target IP Address`.
You can check the adress from Preferences > Network on your Mac. Set the port to 9001. Make sure your Mac and iPhone are connected to the same WiFi.
4. Mount the phone on your headphones, and connect your guitar to your audio interface. You can have better guitar playing experience by plugging in a pre-amp simulator before the plugin. 
5. Have fun!
