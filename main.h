/**
  \file main.h
   
  \author G. Icking-Konert
  \date 2014-03-15
  \version 0.1

  \brief main header and declaration of global variables 

  main header with global macros and global variable declaration. 
  All global variables start with "g_" to indicate their scope.

*/

// for including file only once
#ifndef _MAIN_H_
#define _MAIN_H_


/*******
  global includes
*******/

#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>


/*******
  global macros
*******/

/// max length of strings, e.g. filenames
#define  STRLEN   1000

// activate debug output
//#define DEBUG


/*******
  global typedefs
*******/

/// verbosity level in readable way, from no output to very chatty
typedef enum {MUTE=0, SILENT, INFORM, CHATTY} verbose_t;


/*******
  global variables
*******/

/// define globals only once (with _MAIN_ defined)
#ifdef _MAIN_
  #define global
#else
  #define global extern
#endif

/// wait for \<return\> prior to closing console window
global bool           g_pauseOnExit;

/// optimize for background operation, e.g. skip prompts and console colors
global bool           g_backgroundOperation;

// undefine global keyword
#undef global

#endif // _MAIN_H_

// end of file
