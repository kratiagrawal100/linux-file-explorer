# Terminal based File Explorer
A file explorer for linux systems that runs on terminal implemented using System calls runs in two modes:
1. Normal mode
2. Command mode
## Prerequisites

**1. Platform:** 
* Linux 

**2. Software Requirement:**
* C++ compiler

## Steps to run the project
- Open your terminal in the folder where main.cpp file present
* run C++ main.cpp
-After successfull compilation 
* run ./a.out

## Assumptions
* Default mode is Normal mode and press **:** to switch to Command mode.
* Press **ESC** key to go back to normal mode from command mode.
* Press **q** key from normal mode to exit from program.
* In Normal Mode, no of entries display will depend on the terminal height.
* Use Arrow keys to scroll up ,down ,previous and after.
* In Command mode, to edit the command use backspace only.
* For files/directories names should not have space.
* Make sure while navigating one file record taking one line , if one record takes more than one line then scrolling won't work correctly.
* if **Enter** is pressed on some file and if there is no default application found then there will be no movement.
