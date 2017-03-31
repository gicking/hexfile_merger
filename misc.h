/**
  \file misc.h
   
  \author G. Icking-Konert
  \date 2008-11-02
  \version 0.1
   
  \brief declaration of misc routines
   
  declaration of routines not really fitting anywhere else

*/

// for including file only once
#ifndef _MISC_H_
#define _MISC_H_


/// define common sleep(ms) routine
#if defined(WIN32)
  #define SLEEP(a)    Sleep(a)
#endif // WIN32
#if defined(__APPLE__) || defined(__unix__)
  #define SLEEP(a)    usleep((int32_t) a*1000L)
#endif // __APPLE__ || __unix__


/// extract major / minor / build revision number from 16b identifier
void        get_version(uint16_t vers, uint8_t *major, uint8_t *minor, uint8_t *build, uint8_t *status);

/// print application name and version
void        get_app_name(char *in, uint16_t vers, char *out);


/// terminate program after cleaning up (close logfile etc.)
void        Exit(uint8_t code);

/// check for C whitespace (similar to isspace())
uint8_t     is_whitespace(char c);

/// check if string is a constant
uint8_t     is_number(char *s);

/// get index of function 
uint8_t     idx_function(char *s);

/// calculate expression in RPN (for set, if, while)
int32_t     eval_exp(uint16_t numTarget, uint8_t idxTarget[]);


/// concatenate 2*8b buffer -> uint16_t (MSB first)
uint16_t    concat_u16(uint8_t *buf);

/// concatenate 2*8b args -> uint16_t (MSB first)
uint16_t    concat2_u16(uint8_t hb, uint8_t lb);

/// concatenate 2*8b buffer -> int16_t (MSB first)
int16_t     concat_s16(uint8_t *buf);

/// concatenate 4*8b buffer -> uint32_t (MSB first)
uint32_t    concat_u32(uint8_t *buf);

/// concatenate 4*8b buffer -> uint32_t (MSB first)
int32_t     concat_s32(uint8_t *buf);

/// get byte idx out of u16b argument (0=LSB)
uint8_t     get_byte_u16(uint16_t value, uint8_t idx);

/// get byte idx out of s16b argument (0=LSB)
uint8_t     get_byte_s16(int16_t value, uint8_t idx);

/// get byte idx out of u32b argument (0=LSB)
uint8_t     get_byte_u32(uint32_t value, uint8_t idx);

/// get byte idx out of s32b argument (0=LSB)
uint8_t     get_byte_s32(int32_t value, uint8_t idx);

/// simple calculation of log2(x); missing in Cosmic "math.h"
#if defined(EXECUTE_uC) || defined(PERFORMANCE_uC)
uint16_t    log2(float arg);
#endif // EXECUTE_uC || PERFORMANCE_uC

/// set title of console window
#if defined(WIN32) || defined(__APPLE__) || defined(__unix__)
void        setConsoleTitle(const char *title);
#endif // WIN32 || __APPLE__ || __unix__

/// set console text color
void        setConsoleColor(uint8_t color);

/// replace substrings in string
uint8_t stringReplace(char *search, char *replace, char *string);

#endif // _MISC_H_

// end of file
