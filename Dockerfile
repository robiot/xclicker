# https://hub.docker.com/r/robiot/xclicker
FROM ubuntu:18.04

RUN apt-get update -y
RUN apt-get install -y meson pkg-config gtk+-3.0 libx11-dev libxi-dev libxtst-dev \
    python3-pip python3-setuptools patchelf desktop-file-utils libgdk-pixbuf2.0-dev fakeroot strace fuse wget \
    build-essential manpages-dev software-properties-common --fix-missing

RUN add-apt-repository ppa:ubuntu-toolchain-r/test -y
RUN apt-get install -y gcc-11
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 30

# AppImageTool
RUN wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage -O /opt/appimagetool

# Workaround AppImage issues with Docker
RUN cd /opt/; chmod +x appimagetool; sed -i 's|AI\x02|\x00\x00\x00|' appimagetool; ./appimagetool --appimage-extract
RUN mv /opt/squashfs-root /opt/appimagetool.AppDir
RUN ln -s /opt/appimagetool.AppDir/AppRun /usr/local/bin/appimagetool
