<p align="center">
	<img src="https://raw.githubusercontent.com/robiot/xclicker/master/img/banner.png" alt="XClicker" />
</p>

<p align="center">
	<img alt="GitHub All Releases" src="https://img.shields.io/github/downloads/robiot/xclicker/total?label=GitHub%20Downloads" />
  	<a href="https://aur.archlinux.org/packages/xclicker"><img alt="AUR version" src="https://img.shields.io/aur/version/xclicker" /></a>
  	<img alt="GitHub Issues" src="https://img.shields.io/github/issues/robiot/XClicker.svg" />
  	<img alt="GitHub Contributors" src="https://img.shields.io/github/contributors/robiot/XClicker" /></a>
</p>

---
<h4 align="center">Oh hey, I'm on Twitter too if you'd like to follow more about what I'm up to. Hopefully I'll see you there! 😊</h4>
<p align="center">
	<a href="https://twitter.com/robiot" target="_blank">
		https://twitter.com/robiot
	</a>
</p>

## What is XClicker?
XClicker is an open-source, easy to use, feature-rich, **blazing fast** Autoclicker for linux desktops using x11.

![Example image](https://raw.githubusercontent.com/robiot/xclicker/master/img/example.png)
*v1.4.0*

![forthebadge](https://forthebadge.com/images/badges/made-with-c.svg) [![forthebadge](https://forthebadge.com/images/badges/check-it-out.svg)](https://xclicker.xyz/downloads)

## Main features
 * Fairly simple layout;
 * Safe mode, to protect from unwanted behaviour;
 * Autoclick with a specified amount of time between each click;
 * Choose mouse button [Left/Right/Middle];
 * Choose click type [Single/Double/Hold];
 * Repeat until stopped or repeat a given amount of times;
 * Click on a specified location only;
 * Randomize the click interval;
 * Specify hold time per click;
 * Click while holding hotkey down;
 * Start / Stop with a custom hotkey;

### How much cps?
The highest I have got with it was around **750cps**, but that was still with 1 millisecond interval.\
With 0 millisecond interval, the focused application may freeze.

## Building

After cloning the repository, you only have to run this one command. The executable will be placed in **./build/release/src/xclicker**.
```
$ make release
```

## Installing
Check out the [Installation Guide](https://github.com/robiot/xclicker/wiki/Installation)

## All repositories
- XClicker: https://github.com/robiot/xclicker
- Web: https://github.com/robiot/xclicker-web

## Contributing
All contributions are welcome <3.
Check out the [Contibuting Guide](https://github.com/robiot/xclicker/wiki/Contibuting) to see how to setup your enviroment.

## License
XClicker is licensed under GPL-3.0 LICENSE.

Dependencies are licensed by their own.
