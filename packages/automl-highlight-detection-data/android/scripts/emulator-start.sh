#!/bin/bash

set -ex

avdmanager list device -c

devices=("pixel" "Nexus 6" "pixel_xl" "Nexus 10" "Nexus 4" "Nexus 7")
devices_size=${#devices[@]}
index=$(($RANDOM % $devices_size))

avdmanager create avd --device "${devices[$index]}" --name pixel -k "system-images;android-30;google_apis;x86" --force

set -ex
rm -f /root/.android/avd/pixel.avd/*.lock

adb -a -P 5037 server nodaemon &
emulator @pixel -no-audio -no-boot-anim -no-window -accel on -gpu off -verbose -memory 4096 -no-snapshot
