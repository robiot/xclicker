# https://hub.docker.com/r/robiot/xclicker
#te
FROM ubuntu:18.04

RUN apt-get update -y
RUN apt-get install -y meson pkg-config gtk+-3.0 libx11-dev libxi-dev libxtst-dev \
    python3-pip python3-setuptools patchelf desktop-file-utils libgdk-pixbuf2.0-dev fakeroot strace fuse wget

# AppImageTool
RUN wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage -O /usr/local/bin/appimagetool
RUN chmod +x /usr/local/bin/appimagetool
