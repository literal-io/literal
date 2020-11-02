#!/bin/bash

set -ex

adb start-server
emulator @pixel -no-audio -no-boot-anim -no-window -accel on -gpu off -verbose -memory 2048 -no-snapshot &
adb wait-for-device shell 'while [[ -z $(getprop sys.boot_completed) ]]; do sleep 1; done;'
