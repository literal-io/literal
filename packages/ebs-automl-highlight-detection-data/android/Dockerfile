FROM thyrlian/android-sdk

# install android deps
RUN sdkmanager --update && \
  sdkmanager \
    "emulator" \
    "platform-tools" \
    "platforms;android-30" \
    "system-images;android-30;google_apis;x86" \
    "build-tools;30.0.2" && \
  /opt/license_accepter.sh && \
  avdmanager create avd --device pixel --name pixel -k "system-images;android-30;google_apis;x86"

