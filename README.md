***Thanks for the help MateoConLechuga***


You have to compile the program yourself using Toolchain


Overview:

Gallery is an image viewer for the TI-84 Plus CE that reads an image from a thumb drive.
It is a precursor to a larger project I have planned -- a video player named Cinema.
Because of this, expect to see improvements in speed in the future.
Currently, it is in a very early stage; You must compile the program yourself.


What do You Need?

*CE C Toolchain installed and configured (You may need to use the nightly version)
*USB thumbdrive - some are faster than others - there is a table here - https://ce-programming.github.io/toolchain/libraries/msddrvce.html
     -Adapter for the thumb drive (A to Mini B)


Set Up:

*Create a 320 x 240 resoultion image

*Replace image.png located in "src/gfx" with your image. (It must be named image in this version of Gallery)

*In the CE C Toolchain command prompt, with the directory set to gallery, run "make gfx"

*Transfer "image.bin", which will be generated, to your FAT32 formatted drive
     *Make sure the image.bin is in the first partition, as that is where the calculator reads from
     
*Back in the Toolchain command prompt, run "make"

*Using TI Connect CE, transfer the file to your calculator

*You may need to install a shell like Cesium to run the program if you have a newer calculator


How to Use:

*When you run the program, it waits for a USB device to be connected
*Connect your USB thumb drive, and it will display your image.


Possible/Future improvements:

*Menu to choose from multiple image files
*Improved display speed
