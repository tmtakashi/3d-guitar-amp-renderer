#!/bin/sh
make -C /Users/kazumawatanabe/3d-guitar-amp-renderer/xcode_build/JUCE/tools -f /Users/kazumawatanabe/3d-guitar-amp-renderer/xcode_build/JUCE/tools/CMakeScripts/ALL_BUILD_cmakeRulesBuildPhase.make$CONFIGURATION OBJDIR=$(basename "$OBJECT_FILE_DIR_normal") all
