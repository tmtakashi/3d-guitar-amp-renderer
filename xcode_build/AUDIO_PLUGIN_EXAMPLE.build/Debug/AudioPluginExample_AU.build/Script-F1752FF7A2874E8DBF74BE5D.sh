#!/bin/sh
make -C /Users/kazumawatanabe/3d-guitar-amp-renderer/xcode_build -f /Users/kazumawatanabe/3d-guitar-amp-renderer/xcode_build/CMakeScripts/AudioPluginExample_AU_cmakeRulesBuildPhase.make$CONFIGURATION OBJDIR=$(basename "$OBJECT_FILE_DIR_normal") all
