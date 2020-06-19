# zmenu
Simple menu program for DOS

## What's the point?
- just for fun
- batch menus in DOSBox limited to characters 0-9 and A-Z  (max. ~36 entries)
- no simple way to use the arrow keys to navigate batch menus in DOSBox

## Compiling
OpenWatcom: http://openwatcom.org/

## Usage
1. Put all the zmenu files in C:\ZMENU
    - you can change the default path in ZMENU.BAT
2. Update ZMENU.TXT to whatever entries you want in your menu
    - the examples should give you an idea what you need to write
3. run ZMENU.BAT
4. Keys:
    - up/down arrow keys
    - enter runs currently selected item
    - esc to exit
