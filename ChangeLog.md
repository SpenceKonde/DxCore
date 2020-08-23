1.1.0a
* As 1.1.0, but contains new toolchain. Does not work correctly on Linux platforms.
1.1.0
* Corrected 128DB board definitions.
* Added support for Optiboot Bootloader
* Include prebuilt Optiboot binaries for each size of flash, with 1s and 8s start delay.
* Corrected avrdude.conf for 128KB parts to support programming via bootloader
* Corrected DB board definitions so that 'burn bootloader' does not disable MVIO

1.0.1
* Tried to add support for curiosity nano (unsuccessfully)
* Freshly built toolchain that I'm hosting in S3 to accomodate the 128k DB-series parts and the smaller-flash DA-series. THESE SHOULD NOW WORK!
* Minor doc improvements
