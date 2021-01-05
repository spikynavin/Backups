#!/bin/sh
echo "Start casting"
adb devices
adb shell screenrecord --output-format=h264 -| ffplay -
