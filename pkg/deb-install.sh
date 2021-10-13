#! /bin/bash

echo "Welcome to the XClicker Debian installer"
which curl >/dev/null && echo "Curl installed, moving on..." || sudo apt-get install curl

echo "Getting latest version..."
version=$(curl --silent "https://api.github.com/repos/robiot/XClicker/releases/latest" | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/' | cut -c 2-)
name="xclicker_${version}_amd64.deb"

echo "Found $name"

cd /tmp
sudo rm -rf ./${name} && curl -OL https://github.com/robiot/XClicker/releases/latest/download/${name} && sudo apt-get install ./${name} && sudo rm -rf ./${name}

if [ $? -eq 0 ]; then
    echo "XClicker $version sucessfully installed!"
else
    echo "Failed to install"
fi
cd ~/