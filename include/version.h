/**
  \file version.h
   
  \author G. Icking-Konert
   
  \brief declaration of SW version number
   
  declaration of 2B SW version number. Format is xx.xxxxxxxx.xxxxx.x
  A change major version ([15:14] -> 0..3) indicates e.g. change in the SW architecture. 
  A change in the minor version ([13:6] -> 0..255) indicates e.g. critical bugfixes. 
  A change in build number ([5:1] -> 0..31) indicates cosmetic changes.
  The release status is indicated by bit [0] (0=beta; 1=released)
*/

// for including file only once
#ifndef _SW_VERSION_H_
#define _SW_VERSION_H_

/// 16b SW version identifier 
#define VERSION     ((1<<14) | (6<<6) | (3<<1) | (0<<0))     // -> v1.6.3b

#endif // _SW_VERSION_H_


/********************

Revision History
----------------

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

v1.5.0 (2020-04-09)
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
      
********************/

// end of file
