## Supported IDE versions
DxCore requires a version of the IDE later than 1.6.3; - it may require a much more recent version of the IDE. 1.8.13 is recommended. 

## Boards Manager Installation

This board package can be installed via the board manager. The boards manager URL is:

`http://drazzy.com/package_drazzy.com_index.json`

1. File -> Preferences, enter the above URL in "Additional Boards Manager URLs"
2. Tools -> Boards -> Boards Manager...
3. Select "DxCore by Spence Konde" and click "Install".

## Manual Installation
Manual installation allows the latest version of the core to be installed, with fixes that may not yet be available in the board manager version of the core. **Unless using Arduino IDE 1.8.13, you must use board manager to install the latest version of the Official Arduino AVR board package** for manual installation to work. Manual installation is recommended if you are working on developing or modifying the core, or if you need the latest fixes which are not in the released version. 

For installing DxCore, there are two options:

Option 1: Download the .zip package (either the "released" version, or by downloading the .zip of master repo), extract, and place in the hardware folder inside your sketchbook folder (if there is no hardware folder, create it). You can find/set the location of the sketchbook folder in the Arduino IDE at File > Preferences -> Sketchbook location.

Option 2: Download the github client, and sync this repo to the hardware subfolder of your sketchbook folder.
