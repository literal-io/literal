#!/bin/bash

set -x

ADB_EMULATOR_HOST="tcp:${ANDROID_EMULATOR_HOSTNAME}:${ANDROID_EMULATOR_PORT}"

until adb -L $ADB_EMULATOR_HOST devices | grep -q "emulator"
do
  echo "waiting for adb to connect"
  sleep 5
done

echo "waiting for device to boot"
adb -L $ADB_EMULATOR_HOST wait-for-device shell 'while [[ -z $(getprop sys.boot_completed) ]]; do sleep 1; done;'
echo "device boot completed"
