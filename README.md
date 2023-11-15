# zmenu
Simple menu program for DOS

## What's the point?
- Just for fun
- Batch menus in DOSBox limited to characters 0-9 and A-Z  (max. ~36 entries)
- No simple way to use the arrow keys to navigate batch menus in DOSBox

## Compiling
~~OpenWatcom: http://openwatcom.org/~~

Use Open Watcom v2:
https://github.com/open-watcom/open-watcom-v2

Run the _wmake_ command to compile.

The 32-bit executable can be compiled with the following command:
_wmake_ zmenu32

## Usage
1. Put all the zmenu files in C:\ZMENU
    - You can change the default path in ZMENU.BAT
2. Update ZMENU.TXT to whatever entries you want in your menu
    - The examples should give you an idea what you need to write
3. Run ZMENU.BAT
4. Keys:
    - Up/down/left/right arrow keys
    - Enter runs the currently selected item
    - Esc to exit
    - You can also type a number to jump to index
        - Enter and escape keys work for accept/exit here as well
