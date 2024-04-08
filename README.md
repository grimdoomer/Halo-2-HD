# Halo 2 HD
Halo 2 HD is a mod for the original Xbox version of Halo 2 to add support for HD resolutions as well as some quality of life updates. This mod will let you play the game in 480p (proper), 720p, and 1080i (1080p internally).

![](/_images/coag_1080p.png)

## Features
This mod adds the following features to the game:
- Support for 480p (proper), 720p, and 1080i (1080p) resolutions.
- Triple buffering support to increase FPS performance.
- Utilizes extra RAM if you have 128MB RAM upgrade.
- GPU overclocking for performance improvements.
- Increased HDD transfer speed for faster load times.
- Customizable field of view.
- Customizable split screen divider (horizontal or vertical).

All of these features are customizable through an ini config file and are explained in detail in the wiki: [Configuration File](https://github.com/grimdoomer/Halo-2-HD/wiki/Configuration-File).

#### HD Resolutions
Halo 2 claims it supports 480p on the box but this isn't entirely accurate. It DOES support 480p output but it does NOT render in 480p natively. Regardless of what video settings are set on the console the stock game will always
render in 640x480. When 480p output is enabled the console will upscale the final 640x480 image to 480p for display on TV. If you enable widescreen mode the game will also apply anamorphic scaling to the image which causes it to
appear "compressed" width wise.

This patch addresses those issues by rendering in proper 480p resolution and disabling the anamorphic scaling that would normally get applied. It also allows you to play the game in 720p and 1080i. When 1080i mode is enabled
the game is actually rendering at 1080p internally before being encoded into interlaced video signal. The 1080i mode is not considered "playable" as it puts an extreme load on the console GPU (even with overclocking), and typically
plays at single digit FPS at best. I added support for it mostly because I wanted to see if I could get it to work and also so I could take some sweet screenshots before the frame buffer gets converted into interlaced video.

![](/_images/hd_resolutions.png)

#### RAM Upgrades
If your console has 128MB of RAM this patch will utilize the extra RAM available which will enable use of 720p and 1080i video modes as well as increase the size of in-memory caches for textures and geometry. The size increase for the
texture and geometry caches will significantly reduce pop-in issues to the point of being almost non-existent.

#### GPU Overclocking
In order to maximize performance of the game when running in higher resolutions I've added support to overclock the GPU on startup. This is customizable and can be fine tuned to better suit each console's unique GPU qualities. This is
highly recommended for 720p mode as it can increase FPS by as much as 10 FPS. However, overclocking the GPU comes with risks and precautions should be taken to ensure you don't cause your GPU to overheat and burn out. The risks are
outlined in the wiki and should be thoroughly read and understood before attempting to enable and configure GPU overclocking.

#### HDD Transfer Speeds
This patch can increase the speed at which the HDD transfers data to the console which will improve loading times and help reduce pop-in. This value is customizable via the config file and is explained in detail [here](https://github.com/grimdoomer/Halo-2-HD/wiki/Configuration-File#hddspeed). In order to 
use transfer speeds faster than UDMA 3 you will need an 80-pin IDE cable. Trying to use UDAM 4 or 5 with the stock 40-pin cable will result in read errors and the game crashing on startup.

#### Quality of Life Improvements
A few quality of life improvements have been added such as customizable field of view and options to control whether split screen divides the screen horizontally or vertically.

## Compatibility
Due to how this patch works it is not compatible with the Xbox 360 back-compat emulator, nor will it ever be. It would require complex changes to the emulator itself and I have no plans on ever investigating it further. This patch has also not been tested on
XQEMU or any other Xbox emulator for PC. It has only been tested on real hardware and I have no plans for providing additional support for any other means of running original Xbox games.

## Hardware Requirements
This patch can be used on any soft or hard modded Xbox console with no additional hardware modifications. However, if you don't have the 128MB RAM mod you will only be able to play the game in 480p mode. There is NOT enough RAM to play the game
in 720p or 1080i without the RAM upgrade. Additionally you will need an 80-pin IDE cable if you want to use HDD transfer speeds faster than UDMA 3. You do not need a CPU upgraded console to use this patch and having one does not provide any additional
performance gains that I've been able to measure during testing.

## Known Issues/Limitations
This patch currently only works with the base version of the game (1.0) and does not work with the 1.5 update. I plan to port the patch to the 1.5 update at some point in the future. In addition to that there are a couple limitations/known issues such as:
- Campaign checkpoints may not work between this patch and other versions of the game. When loading a campaign save game you may have to start the most recent level from the beginning instead of from the last checkpoint reached.
- You will not be able to take screenshots using Xbox Neighborhood or other tools based on Xbdm when running with 128MB of RAM. You will need to use a tool I wrote that can handle taking screenshots when using 128MB of RAM: [XboxImageGrabber](https://github.com/grimdoomer/XboxImageGrabber)
- HUD elements will get smaller when using higher resolutions. Scaling the HUD elements is non-trivial and will most likely not be fixed.
- Bink videos (intro, attraction, credits) still play in 640x480 resolution. I may look into fixing this in a future update but will most likely result in the videos being stretched.
- Campaign loading screen, player sign-in screen, and pre-game lobbies still render in 640x480, adjusting this is non-trivial and most likely won't be fixed.

## Installation & Compiling
You can find a precompiled version of the patch in the Releases section, you'll need [XDelta](https://www.romhacking.net/utilities/598/) to apply the patch to the clean v1.0 xbe file (SHA1: 78BCC597D9A30AE91DF6FC5EB44FDC62A28EE7F2). 

Once the patch is applied you will
need to copy the patched xbe file and the hd_config.ini file into your Halo 2 game folder on your Xbox console. You'll also want to edit the config file and adjust any settings you'd like to change. Additional information for what the settings are and how to change them can
be found in the wiki: [Configuration File](https://github.com/grimdoomer/Halo-2-HD/wiki/Configuration-File).

Compiling the patch from scratch requires a few different tools. The process is explained in detail in the wiki: [Compiling](https://github.com/grimdoomer/Halo-2-HD/wiki/Compiling).

## FAQ
**Q**: Do I need a CPU upgraded console in order to use this patch? \
**A**: No, having a CPU upgrade is not required nor does it provide any benefits over a non-CPU upgraded console.

**Q**: Does this work with Insignia? \
**A**: Currently no, the patch only works with the 1.0 version of the game and Insignia requires the 1.5 version to connect. I plan to update the patch in the future to support the 1.5 version of the game.

**Q**: Will this work with such-n-such BIOS? \
**A**: The patch does not require any specific BIOS to be used and all features should work on all BIOS images. If you have 128MB of RAM you will need a BIOS image that supports the RAM upgrade or the patch will not be able to make use of the additional RAM.

**Q**: Does this work on the Xbox 360 back-compat emulator? \
**A**: Due to how this patch works it does not work on the Xbox 360 back-compat emulator, nor will it ever. I have no plans to make it compatible with the emulator.

**Q**: Does this work on XQEMU/other Xbox emulators for PC? \
**A**: This patch is most likely not compatible with Xbox emulators on PC and I have no plans for providing support for them.

**Q**: I'm no longer able to take screenshots with Xbox Neighborhood and it just gives me a white image? \
**A**: Due to how the patch utilizes the extra RAM for video memory you will no longer be able to use Xbox Neighborhood or other apps that utilize Xbdm to take screenshots. You'll need to use a separate tool I wrote for this: [XboxImageGrabber](https://github.com/grimdoomer/XboxImageGrabber).
