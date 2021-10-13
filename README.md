<p align="center">
	<img src="https://raw.githubusercontent.com/robiot/XClicker/main/img/banner.png" alt="XClicker">
</p>
<p align="center">
  <strong>Fast gui autoclicker for linux 🐧 | </strong>
  <img alt="Stars" src="https://img.shields.io/github/stars/robiot/XClicker.svg?label=Stars&style=flat" />
  <img alt="GitHub Issues" src="https://img.shields.io/github/issues/robiot/XClicker.svg"/>
  <img alt="GitHub Contributors" src="https://img.shields.io/github/contributors/robiot/XClicker"></a>
</p>

## What is XClicker?
XClicker is a easy to use, feature-rich, **blazing fast** Autoclicker for linux desktops using x11.

![Example image](https://raw.githubusercontent.com/robiot/XClicker/main/img/newexample.png)

## Main features
 * Simple layout;
 * Safe mode, to protect from unwanted behaviour;
 * Autoclick with a specified amount of time between each click;
 * Choose mouse button [Left/Right/Middle];
 * Repeat until stopped or repeat a given amount of times;
 * Click on a specified location only;

### How much cps?
The highest I have got with it was **800**, but that was still with 1 millisecond interval.

Friendly reminder: With 0 millisecond interval your xorg might freeze.

## Building

After cloning the repository, you only have to run this one command. The executable will be placed in **./bin/xclicker**.
```
$ make
```

## Installing

### Recommended
#### Arch
```
yay -S xclicker
```

#### Debian / Ubuntu
 * Go to the latest [release](https://github.com/robiot/XClicker/releases/latest/) and download the .deb file.
 * Run `sudo apt-get install ./xclicker_version_amd64.deb` where **version** is the version of the downloaded release.

Or even easier, with an install script:
```
bash <(curl -s https://raw.githubusercontent.com/robiot/XClicker/main/pkg/deb-install.sh)
```

#### Other
Install with the not recommended way or use the portable version.

### Not recommended
```
make install
```


## License
XClicker is licensed under GPL-3.0 LICENSE.

Dependencies are licensed by their own.
