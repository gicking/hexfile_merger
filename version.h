/**
  \file version.h
   
  \author G. Icking-Konert
  \date 2008-11-02
  \version 0.1
   
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
#define VERSION     ((1<<14) | (3<<6) | (0<<1) | 0)     // -> v1.3.0b

#endif // _SW_VERSION_H_


/********************

Revision History
----------------

1.3 (2018-12-24): 
  - changed commandline keywords (became too complex)
  - added more verbose information
  - added binary import and export format
  - added printing to console
  - added image manipulation (clip, clear, copy, move)
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
      
********************/

// end of file
