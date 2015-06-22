/**
   \file hexfile.h
   
   \author G. Icking-Konert
   \date 2014-03-16
   \version 0.1
   
   \brief declaration of routines to import HEX and S19 files
   
   declaration of routines for importing and interpreting hexfiles in
   Motorola s19 and Intel hex format 
   (description under http://en.wikipedia.org/wiki/SREC_(file_format) or
   http://www.keil.com/support/docs/1584.htm). 
*/

// for including file only once
#ifndef _HEXFILE_H_
#define _HEXFILE_H_

// read next line from RAM buffer
char  *get_line(char **buf, char *line);

// read hexfile into memory buffer
void  load_hexfile(char *filename, char *buf, uint32_t bufsize);

// convert Motorola s19 format in memory buffer to memory image
void  convert_s19(char *buf, uint32_t *addrStart, uint32_t *numBytes, uint16_t *image, uint8_t verbose);

// convert Intel hex format in memory buffer to memory image
void  convert_hex(char *buf, uint32_t *addrStart, uint32_t *numBytes, uint16_t *image, uint8_t verbose);

// convert plain text table (hex addr / data) in memory buffer to memory image
void  convert_txt(char *buf, uint32_t *addrStart, uint32_t *numBytes, uint16_t *image, uint8_t verbose);

// export RAM image to file in Motorola s19 format
void export_s19(char *outfile, uint16_t *image, uint32_t addrStart, uint32_t addrStop);

// export RAM image to file in Intel hex format
void export_hex(char *outfile, uint16_t *image, uint32_t addrStart, uint32_t addrStop);

// export RAM image to file with plain text table (hex addr / data)
void export_txt(char *outfile, uint16_t *image, uint32_t addrStart, uint32_t addrStop);

// save memory image (for debug)
void save_memory_image(char *outfile, uint16_t *image, uint32_t addrStart, uint32_t addrStop);

#endif // _HEXFILE_H_

