#!/bin/bash

set -ex

rm -f /root/.android/avd/pixel.avd/*.lock
adb -a -P 5037 server nodaemon &
emulator @pixel -no-audio -no-boot-anim -no-window -accel on -gpu off -verbose -memory 4096 -no-snapshot &
npm run start
