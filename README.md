hexfile_merger
-------------------

Import files of various formats, apply simple manipulations, and merge them to a single output file.

`usage: hexfile_merger with following options/commands:`

    -h/-help                            print this help
    -v/-verbose [level]                 set verbosity level 0..3 (default: 2)
    -import [infile [addr]]             import from file to image. For binary file (*.bin) provide start address (in hex)
    -export [outfile]                   export image to file
    -print                              print image to console
    -checksum                           print CRC32-IEEE checksum over data ranges in image
    -fill [addrStart addrStop val]      fill specified range with fixed value (addr & val in hex)
    -fillRand [addrStart addrStop]      fill specified range with random values in 0-255 (addr in hex)
    -clip [addrStart addrStop]          clip image to specified range (addr in hex)
    -cut  [addrStart addrStop]          cut specified data range from image (addr in hex)
    -copy [fromStart fromStop toStart]  copy data within image (addr in hex). Keep old data
    -move [fromStart fromStop toStart]  move data within image (addr in hex). Unset old data

Supported import formats:
  - Motorola S19 (*.s19), for a description see https://en.wikipedia.org/wiki/SREC_(file_format)
  - Intel Hex (*.hex, *.ihx), for a description see https://en.wikipedia.org/wiki/Intel_HEX
  - ASCII table (*.txt) consisting of lines with 'addr  value' (dec or hex). Lines starting with '#' are ignored
  - Binary (*.bin) with an additional starting address

Supported export formats:
  - print to stdout (-print)
  - Motorola S19 (*.s19)
  - ASCII table (*.txt) with 'hexAddr  hexValue'
  - Binary (*.bin) without starting address

Files are imported and exported in the specified order, i.e. later imports may
overwrite previous imports. Also outputs only contain the previous imports, i.e.
intermediate exports only contain the merged content up to that point in time.

Notes:
  - this tool is written in ANSI-C, it should be compatible with any platform supporting e.g. GCC
  - file and image buffers sizes are 10MByte. For larger buffers increase LENFILEBUF and LENIMAGEBUF in hexfile.h

If you find any bugs or for feature requests, please drop me a note.

Have fun!
Georg

====================================

# Revision History

v1.6.3b (2023-xx-xx)
  - just a placeholder for now
  
----------------

v1.6.2 (2023-07-28)
  - added some internal helper functions to memory image
  - added CRC32-IEEE checksum function to memory image
  
----------------

v1.6.1 (2023-07-15)
  - migrate to VSCode / PlatformIO file structure for convenience
  
----------------

v1.6.0 (2023-06-29)
  - switch from static RAM buffer to dynamic memory image
  - support for 32-bit addresses (see memory_image.h)
  
----------------

v1.5.1 (2020-12-17)
  - added random fill command
  
----------------

1.5.0 (2020-04-09):
  - improved S19 export for >16bit addresses
  - added IHX export option

----------------

1.4.0 (2019-03-21):
  - added '-fill' command
  - changed '-clear' to '-cut' for clarity (data is removed, not cleared)
  - added support for 64-bit address range

----------------

1.3.0 (2018-12-24):
  - changed commandline keywords (became too complex)
  - added more verbose information
  - added binary import and export format
  - added printing to console
  - added image manipulation (clip, clear, copy, move)
  - fixed S19 export bugs for >16bit addresses and small images
  - fixed IHX import bug for record type 5
  - harmonized files with https://github.com/gicking/stm8gal

----------------

1.2.0 (2018-12-08): 
  - remove (non-functional) Intel hexfile export
  - fixed s19 export bug for 16bit addresses
  - added optional hex support in table import
  - added comment option '#' in table import

----------------

1.1.0 (2017-03-31):
  - fixed wrong handling of extended linear address records

----------------

1.0.0 (2015-06-18):
  - initial release by Georg Icking-Konert under the Apache License 2.0
