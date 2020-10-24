#!/bin/bash

adb start-server
emulator @pixel -no-audio -no-boot-anim -no-window -accel on -gpu off &
adb wait-for-device shell 'while [[ -z $(getprop sys.boot_completed) ]]; do sleep 1; done;'
