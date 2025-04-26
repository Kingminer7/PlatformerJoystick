#! /bin/bash

if [ ! -d "build-android64" ]; then
    geode build -p android64
fi

cmake --build build-android64
adb push build-android64/*.geode /storage/emulated/0/Android/media/com.geode.launcher/game/geode/mods/ 
adb shell am force-stop com.geode.launcher
adb shell am start -n com.geode.launcher/com.geode.launcher.GeometryDashActivity
adb logcat Geode:V \*:S
