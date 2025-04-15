# Gallery: TI-84 Plus CE Image Viewer

## Overview
Gallery is an image viewer for the TI-84 Plus CE that reads images from a USB thumb drive. It serves as a precursor to a larger planned project — a video player named Cinema. Because of this foundation, expect to see speed improvements in future releases. Currently, Gallery is in an early development stage.

## What You Need
* The nightly version of CLibs may be required
* USB thumb drive (performance varies by model)
  * See speed comparison table: https://ce-programming.github.io/toolchain/libraries/msddrvce.html
* USB adapter (A to Mini B)
* fbin tool to convert images
  * https://github.com/will-dabeast09/fbin
* HDD Raw Copy Tool
  * https://hddguru.com/software/HDD-Raw-Copy-Tool/

## USB Set-Up
In Windows Command Prompt:
```
diskpart
list disk               # Find your USB (e.g., Disk 2)
select disk 2           # Replace with your actual disk number
clean                   # Removes all partitions — now the drive is "unformatted"
exit
```
Then use HDD Raw Copy Tool to transfer the file.

## Instructions
1. Put all of your images into one folder
2. Run fbin to convert the images
3. Use HDD Raw Copy Tool to transfer the file
4. Send the program 8xp to your calculator
5. Run the program and plug in the USB thumb drive

## Features
* Use the Up/Down keys to scroll through images
* Press Clear to exit

## Future Improvements
* Improved display speed
