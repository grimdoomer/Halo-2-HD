# Halo 2 HD
Halo 2 HD is a mod for the original Xbox version of Halo 2 to add support for HD resolutions as well as some quality of life updates. This mod will let you play the game in 480p (wide), 720p, and 1080i (1080p internally).

<img width="1920" height="1080" alt="coag_1080p" src="https://github.com/user-attachments/assets/be7831a7-8bb9-4278-ae5d-d521a418ee15" />

## Features
This mod adds the following features to the game:
- Support for 480p (wide), 720p, and 1080i (1080p) resolutions.
- Triple buffering support to increase FPS performance.
- Utilizes extra RAM if you have 128MB RAM upgrade.
- GPU overclocking for performance improvements.
- Increased HDD transfer speed for faster load times.
- Customizable field of view.
- Customizable split screen divider (horizontal or vertical).

All of these features are customizable through an ini config file and are explained in detail in the wiki: [Configuration File](https://github.com/grimdoomer/Halo-2-HD/wiki/Configuration-File).

#### HD Resolutions
This patch allows you to play the game in 480p (wide), 720p and 1080i as well as disable anamorphic scaling. On the stock version of the game when 480p and widescreen are enabled on the console the game will always render in 640x480. This patch adds support for the 480p "wide" resolution 720x480. 

When 1080i mode is enabled the game is actually rendering at 1080p internally before being encoded into interlaced video signal. The 1080i mode is not considered "playable" as it puts an extreme load on the console GPU (even with overclocking), and typically
plays at single digit FPS at best. I added support for it mostly because I wanted to see if I could get it to work and also so I could take some sweet screenshots before the frame buffer gets converted into interlaced video.

<img width="1920" height="1080" alt="hd_resolutions" src="https://github.com/user-attachments/assets/72c5349f-8b56-40d6-a19e-8db831334eaa" />

#### RAM Upgrades
If your console has 128MB of RAM this patch will utilize the extra RAM available which will enable use of 720p and 1080i video modes as well as increase the size of in-memory caches for textures and geometry. The size increase for the
texture and geometry caches will significantly reduce pop-in issues to the point of being almost non-existent.

#### GPU Overclocking
In order to maximize performance of the game when running in higher resolutions I've added support to overclock the GPU on startup. This is customizable and can be fine tuned to better suit each console's unique GPU qualities. This is
highly recommended for 720p mode as it can increase FPS by as much as 10 FPS. However, overclocking the GPU comes with risks and precautions should be taken to ensure you don't cause your GPU to overheat and burn out. The risks are
outlined in the wiki and should be thoroughly read and understood before attempting to enable and configure GPU overclocking.

#### HDD Transfer Speeds
This patch can increase the speed at which the HDD transfers data to the console which will improve loading times and help reduce pop-in. This value is customizable via the config file and is explained in detail [here](https://github.com/grimdoomer/Halo-2-HD/wiki/Configuration-File#hddspeed). In order to 
use transfer speeds faster than UDMA 3 you will need an 80-wire IDE cable. Trying to use UDAM 4 or 5 with the stock 40-wire cable will result in read errors and the game crashing on startup.

#### Quality of Life Improvements
A few quality of life improvements have been added such as customizable field of view and options to control whether split screen divides the screen horizontally or vertically.

## Compatibility
Due to how this patch works it is not compatible with the Xbox 360 back-compat emulator, nor will it ever be. It would require complex changes to the emulator itself and I have no plans on ever investigating it further. This patch has also not been tested on
XQEMU or any other Xbox emulator for PC. It has only been tested on real hardware and I have no plans for providing additional support for any other means of running original Xbox games.

## Hardware Requirements
This patch can be used on any soft or hard modded Xbox console with no additional hardware modifications. However, if you don't have the 128MB RAM mod you will only be able to play the game in 480p wide mode. There is NOT enough RAM to play the game
in 720p or 1080i without the RAM upgrade. Additionally you will need an 80-wire IDE cable if you want to use HDD transfer speeds faster than UDMA 3. You do not need a CPU upgraded console to use this patch and having one does not provide any additional
performance gains that I've been able to measure during testing.

## Known Issues/Limitations
- Campaign checkpoints may not work between this patch and other versions of the game. When loading a campaign save game you may have to start the most recent level from the beginning instead of from the last checkpoint reached.
- You will not be able to take screenshots using Xbox Neighborhood or other tools based on Xbdm when running with 128MB of RAM. You will need to use a tool I wrote that can handle taking screenshots when using 128MB of RAM: [XboxImageGrabber](https://github.com/grimdoomer/XboxImageGrabber)
- HUD elements will get smaller when using higher resolutions.
- Campaign loading screen, player sign-in screen, and pre-game lobbies still render in 640x480, adjusting this is non-trivial and most likely won't be fixed.
- Fog will have a "scrolling" effect in higher resolutions and will bleed into other player screens in split screen mode. This will be fixed in a future update.

## Installation & Compiling
You can find a precompiled version of the patch in the Releases section. To apply the patch you'll need to use the XDelta tool provided in the zip file:
1. Open XDelta and select your clean default.xbe file. For the 1.0 version of the game this would be the default.xbe on the game disc, for the 1.5 title update this would be the default.xbe file in the title update folder.
2. Select the patch file that corresponds to the xbe you choose in step 1. For the 1.0 version this would be the "halo2_hd_1.0.xdelta" file, for the 1.5 title update use the "halo2_hd_1.5.xdelta" file.
3. Click the gear icon and uncheck "Checksum validation".
<img width="487" height="280" alt="image" src="https://github.com/user-attachments/assets/5e453d48-d069-4a94-b6d5-852972627db6" />

4. Click apply patch.

Once the patch is applied you will
need to copy the patched xbe file and the hd_config.ini file into your Halo 2 game folder on your Xbox console. You'll also want to edit the config file and adjust any settings you'd like to change. Additional information for what the settings are and how to change them can
be found in the wiki: [Configuration File](https://github.com/grimdoomer/Halo-2-HD/wiki/Configuration-File).

Compiling the patch from scratch requires a few different tools. The process is explained in detail in the wiki: [Compiling](https://github.com/grimdoomer/Halo-2-HD/wiki/Compiling).

## FAQ
**Q**: Do I need a CPU upgraded console in order to use this patch? \
**A**: No, having a CPU upgrade is not required nor does it provide any benefits over a non-CPU upgraded console.

**Q**: Will I get banned on Insignia using this patch? \
**A**: No Insignia will not ban you for playing with this patch.

**Q**: Will this work with such-n-such BIOS? \
**A**: The patch does not require any specific BIOS to be used and all features should work on all BIOS images. If you have 128MB of RAM you will need a BIOS image that supports the RAM upgrade or the patch will not be able to make use of the additional RAM.

**Q**: Does this work on the Xbox 360 back-compat emulator? \
**A**: Due to how this patch works it does not work on the Xbox 360 back-compat emulator, nor will it ever. I have no plans to make it compatible with the emulator.

**Q**: Does this work on XQEMU/other Xbox emulators for PC? \
**A**: This patch is most likely not compatible with Xbox emulators on PC and I have no plans for providing support for them.

**Q**: I'm no longer able to take screenshots with Xbox Neighborhood and it just gives me a white image? \
**A**: Due to how the patch utilizes the extra RAM for video memory you will no longer be able to use Xbox Neighborhood or other apps that utilize Xbdm to take screenshots. You'll need to use a separate tool I wrote for this: [XboxImageGrabber](https://github.com/grimdoomer/XboxImageGrabber).
