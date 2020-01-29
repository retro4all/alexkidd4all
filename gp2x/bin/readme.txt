************************
*      AlexKidd2X      *
*         v0.5         *
*     Xmas Edition     *
************************

By: Israel Lopez Fernandez (Puck2099)
Website: http://www.gp32wip.com
Contact and donations (Paypal): puck2099@gmail.com
Based on the work of Charles Mac Donald.
Splash screen by Neostalker.
Special thanks to Rlyeh for his minimal libs and the efforts done to help me with my emulator.

What is AlexKidd2X?
--------------
It's an emulator of SEGA Master Sytem and Game Gear for GP2X.

How must I install it?
----------------------
Unzip the files in any place of your SD card. Copy the roms into any folder. If you use a roms
folder different to the rom subfolder located in the same folder as the executable, you will
have to edit the file alexkidd2x.cfg and set the appropiate absolute path (for example, if you
copy the roms in the folder roms/sms into the root of the SD, you will have to edit the file
and write /mnt/sd/roms/sms). The roms could be uncompressed (.sms and .gg extension) or
compressed in several .zip files.
If you want to use custom marquees, copy them into marquees directory in the same folder as the
executable, but remember to use only .png files.

IF YOU HAVE USED A PREVIOUS VERSION OF MY EMULATOR, REMOVE ALL FILES INTO DATA FOLDER.

Is the source code available to download?
-----------------------------------------
Yes, as it is said by GPL license, you can download the source code from my own website.
By the way, I warn you that the changes realised to the code are not enough "cleaned" nor
documented.
If you want to make any change, please, I would be very grateful if you contact me first
to avoid working two people in fixing the same bug :)

How could I help you?
---------------------
There are a lot of ways to help to the scene, but if you want to help me I would like to know
all bugs you can find, useful ideas to add to new releases or any comment about my work :)
In addiction, if you want to help me to pay the web server from which download my work or my
website, you can do any donation using Paypal (puck2099@gmail.com) or a bank transfer (ask me
for my bank account) and it would be greatly appreciate. By the way, I'm quite interested in
retro systems as Master System games, so if you have any one that don't want, I would like to
receive it in order to get a better emulation :) 

Controls:
----------
Controls of rom selector are:
Up/Down -> Move the cursor a line.
L/R/Left/Right -> Move the cursor a page.
Left/Right -> Change menu option.
B -> Confirm.
Y -> Preview selected marquee (keep the button pressed).
CLICK+X -> Delete selected rom.
CLICK+Y -> Save actual rom configuration as default configuration for all roms/
	   Save actual folder as default roms folder.
START -> Go back/Exit emulator.
SELECT -> Confirm game selection/Scan folder to identify ROMs.

Controls can be remapped from the menu, but by default are the following ones:
Joystick -> Pad.
A -> Button 1.
B -> Button 2.
START -> Pause.
CLICK+VOL+ -> Increase current save slot (up to 10).
CLICK+VOL- -> Decrease current save slot.
CLICK+L -> Save game in current slot.
CLICK+R -> Restore game in current slot.
CLICK+X -> Show/Hide battery level.
CLICK+Y -> Show/Hide FPS.
VOL+ -> Increase volume.
VOL- -> Decrease volume.
L+R -> Go back to main menu, exiting the game.

Changelog AlexKidd2X:
----------------------
v0.5:
- Rewritten most source code to get a more accurate emulation.
- Added Squidge's MMU Hack to increase a bit the speed.
- Added CRC database. Each time you load a game, it is checked its CRC and compare with the
  internal database to get its name and some parameters.
- Folder structure has been changed. Now savestates are located on "save" instead of the roms
  folder and configuration files have .cfg extension. Using this CRC, now the data is saved using
  the appropiatre name so if we have a rom with some different names, the emulator will take them
  internally as the same one, allowing that we can use the savestates and configuration files 
  automatically in all of them.
- Added 10 slots to save/load states instead of the only one used in the previous versions. You
  can change between slots using CLICK+VOL- and CLICK+VOL+
- Added status messages to show information in some situations (information when save/load state,
  slot change, ROMs information...).
- Added the possibility to overclock or downclock the Z80 speed from options menu. It can be useful
  in some games that didn't work 100% speed in the original Master System hardware.
- Internal RAM save/load works fine now.
- Added digitalized voices and working fine (see Street Fighter 2, Gauntlet, Game Gear's Sonic...).
- Added correct FM chipset support. Now you can choose from two different implementations. By the
  way, it is CRC checked so it will be only actived in games that support FM (Wonderboy 3...).
- Added the possibility to choose samplerate between 44100 and 22050. They sound mostly the same,
  so by default it is selected 22050 that is faster.
- Added brighter colour palettes, now it looks better :) 
- Optimized tile cache.
- Changed left vertical bar from background colour to transparent colour.
- Removed different frameskip levels and added autoframeskip.
- GUI rewriteen using 8 bits. It avoids changing video mode between the GUI and emulation avoiding
  some problems.
- Fixed partly screen flickering in ROMs selector.
- Now it is saved CPU speed and LCD configuration when loading the emulator and they are restored
  when exiting, avoiding disturbing LCD configurations when loading another program.
- Some minor changes and tweaks in the GUI (scroll pages using Left and Right, remember cursor
  position in roms list...).
- Folder CRC scan from roms selector pushing SELECT. This will save an information file that will
  be loaded automatically and without pause each time you select that folder.
- Added compatibility mode for some Game Gear roms that didn't work correctly (Castle of Illusion,
  Prince of Persia, Out Run Europa...). They are showed as Master System games.
- Added the possibility to show Game Gear roms using Master System resolution.
- Added support to USB joysticks. Now it is possible to play two players using one or two USB
  joysticks and a breakout board or similar device.
- Changed region mode. Now you can choose the region (Japan or USA/Europe) and video mode (PAL or
  NTSC).
- Added a much better support for PAL mode and its 50 hz.
- Fixed some minor bugs.

v0.4:
- Enhanced rom selector's controls, it responds better to buttons events.
- Fixed bug related to absolute paths.
- Added folder navigation so you can select roms from any folder of the tree (NAND, SD, etc.).
  You can set the current folder as the default roms path pushing CLICK+Y.
- Fixed bug that let you select "blank lines" as it were roms.
- Added constant speed (100 Mhz) on roms selector to avoid flickerings when come back using
  different frecuencies.
- Added the possibility to select custom marquess (only png images) to use them when playing
  at the original resolution. If you are on the marquees menu but don't want to select anyone,
  choose !none.png
- Added a little speed improvement (up to a 10% in some cases).
- Added battery level meter when playing and visual warning of low battery using the red led (you
  can show or hide the battery icon using CLICK+X except when the battery level is almost empty 
  (when the icon flickers) that can't be hidden.
- Fixed Game Gear aspect ratio when using fullscreen mode.
- Fixed minor bugs.

v0.3:
- Changed screen color depth from 16 to 8 bits obtaining up to a 60% of speed increase.
- Added some more speed options to roms selector in order to save batteries.
- Added the possibility to configure the default path where looking for roms (see instalation).
- Added directions to controls remap, so you will be able to assign a button to a pad direction
  (quite useful in games as Bank Panic).
- Improved control in buttons remap, now it is more accurate.
- Added the possibility to change default configuration for all roms (push CLICK+Y in rom
  configuration screen to apply this to all roms not being executed previously).
- Added region selector to limit FPS to 60 (NTSC) or 50 (PAL).
- Added support to SRAM in games that incorporate that memory.
- Added the missing characters (!?[]) to the font used in the roms selector to improve the visual
  aspect.
- Fixed bug that doesn't close sound device at exit.
- Fixed minor bugs.

v0.2:
- Code completely rewritten from SDL to Rlyeh Minilib obtaining a 15% of speed increase (Alex
  Kidd in Miracle World runs almost 100% at 166 Mhz).
- Sound improvements, now runs at 44100 Hz and sounds "clearly".
- Added volume control.
- Added optional FPS board.
- Increased speed in ROM selector.
- Added personalized game configuration (it is saved automatically when launched).
- Saved cursor position on ROMs selector so you don't have to start from the beggining when 
  selecting a new ROM.
- Added combination to delete ROMs from the selector.
- Fixed some minor bugs.

v0.1:
-------
- First release.
