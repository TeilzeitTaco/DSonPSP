# DSonPSP

DSonPSP v0.8 is an effort by TheMrIron2, z2442 and other PS Homebrew developers to update the proof of concept DS emulator, DSonPSP.

Read below for the background information, credits and instructions.

## Background

DSonPSP was a proof of concept DS emulator for PSP by Yoshihiro. It was based on what is now ancient 2006/2007 Desmume code, and as a result it is very slow and most titles don't work very well. A few versions were released, up to beta v0.7; this repository is based on the source code available for 0.6, and is an attempt to integrate MelonDS into DSonPSP.

## Programming

DSonPSP v0.8 plans to use modern PSP SDK optimisations and newer code to improve DSonPSP on a compatibility level and a performance level, in hopes of getting some games to be playable.

However, there are some obstacles. DSonPSP is based on an extremely old Desmume build which dates to when the emulator was coded in C, whereas it is now in C++. This branch attempts to solve this by replacing Desmume code with the recent MelonDS.

## Instructions

Create a folder (call it whatever you like, but "DSonPSP" works) in /PSP/GAME/, so the structure is as follows: /PSP/GAME/DSonPSP/.
Place the EBOOT.PBP in this folder, and within this folder create another folder called /NDSROM/. Place any legally obtained DS backups into this folder.
Now you can simply boot DSonPSP from your PSP and the ROMs will show up automatically.

## Credits 

My name might be on the repository, but I couldn't have done this on my own. I'll list the credits in no particular order:

z2442: Help updating DSonPSP so it compiles on new PSP SDK and additional compilation help
TheMrIron2: Tweaks
mrneo240: Help with compiling

## Footnote

Note that this project is a hobby for everyone involved, and due to our limited time among other factors we may not be able to achieve everything we set out to, including bug fixes and such. Any help is welcomed, but we are not miracle workers and our only objective is to update and improve some code.

Please share this repository with your friends! Any interest, attention or publicity will give us motivation to continue working on this and it may also find people who are interested in helping us out, as we are a very small project at the moment. 
