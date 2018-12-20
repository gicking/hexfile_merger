hexfile_merger
-------------------

Import multiple files of various formats and merge them to a single output file.

`usage: hexfile_merger [-h] [-i infile [addr]] [-c start stop] [-o outfile] [-p] [-v level]`

    -h / --help     print this help
    -i / --input    name of input file. For binary file (*.bin) add start address in hex
    -c / --clip     clip memory image to specified memory range (in hex)
    -o / --output   name of output file
    -p / --print    print memory image to console
    -v / --verbose  verbosity level 0..3 (default: 2)

Supported import formats:
  - Motorola S19 (*.s19), for a description see https://en.wikipedia.org/wiki/SREC_(file_format)
  - Intel Hex (*.hex, *.ihx), for a description see https://en.wikipedia.org/wiki/Intel_HEX
  - ASCII table (*.txt) consisting of lines with 'hexAddr  value'. Lines starting with '#' are ignored
  - Binary (*.bin) with an additional starting address

Supported export formats:
  - Motorola S19 (*.s19)
  - ASCII table (*.txt) with 'hexAddr  hexValue'
  - Binary (*.bin) without starting address

Files are imported and exported in the specified order, i.e. later imports may
overwrite previous imports. Also outputs only contain the previous imports, i.e.
intermediate exports only contain the merged content up to that point in time.

Notes:
  - this tool is written in ANSI-C, it should be compatible with any platform supporting e.g. GCC
  - buffer sizes are set to 10MByte. If this is insufficient, increase LENFILEBUF and LENIMAGEBUF in hexfile.h

If you find any bugs or for feature requests, please drop me a note.

Have fun!
Georg

====================================

# Revision History

1.3 (2018-12-20): 
  - added more verbose information
  - added binary import and export format
  - added options to print and clip memory map
  - fixed S19 export bugs for >16bit addresses and small images
  - fixed IHX import bug for record type 5
  - harmonized files with https://github.com/gicking/stm8gal

----------------

1.2 (2018-12-08): 
  - remove (non-functional) Intel hexfile export
  - fixed s19 export bug for 16bit addresses
  - added optional hex support in table import
  - added comment option '#' in table import

----------------

1.1 (2017-03-31): 
  - fixed wrong handling of extended linear address records

----------------

1.0 (2015-06-18):
  - initial release by Georg Icking-Konert under the Apache License 2.0
