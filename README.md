hexfile_merger
-------------------

tool for importing multiple hexfiles of different formats and exporting them to an new merged hexfile. Supported import and export formats: 
  - Motorola S19 (see https://en.wikipedia.org/wiki/SREC_(file_format))
  - Intel Hex (see https://en.wikipedia.org/wiki/Intel_HEX)
  - ASCII table with each line containing "hexAddr  decValue"

Notes:
  - this tool is written in ANSI-C, so it can be compiled on any platform supporting e.g. GCC
  - image buffer size is currently set to 10MByte via parameter BUFSIZE in main.c

If you find any bugs or for feature requests, please drop me a note.

Have fun!
Georg

====================================

Revision History
----------------

1.0 (2015-06-18): initial release by Georg Icking-Konert under the Apache License 2.0
