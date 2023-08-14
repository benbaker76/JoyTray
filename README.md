# JoyTray

## Description

A Windows tray application that supports both the Ultimarc ServoStik and UltraStik 360

![](/images/ultrastik.png)

Running JoyTray from command line:
```
JoyTray.exe -servo [joy4way|joy8way]
```
If using Ultrastik 360:
```
JoyTray.exe -u360 [mouse|analog|joy8way|easyjoy8way|joy4way|djoy4way|rdjoy4way|joy2way|vjoy2way]
```

If you add an `-exit` argument the program will exit without staying resident.

Until front-end developers add native support for the ServoStik, this is the way to automate switching. The front-end needs to be configured to run a command line before starting games eg:

- `JoyTray.exe -servo joy8way` before all 8-way games
- `JoyTray.exe -servo joy4way` before all 4-way games

## JoyTray Hotkeys:

When resident: F4 switches to 4-way and F8 switches to 8-way. You could assign I-PAC-connected controls to these keycodes. Maybe use I-PAC shifted codes.
JoyTray GUI Mode:

Launch JoyTray.exe by itself and it will show an icon in the tray which can be right-clicked to show a menu. It will gray out the U360 or ServoStik menu items based on whether you have them attached. There is an option to run the program at startup.
