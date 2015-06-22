/**
   \file main.c

   \author G. Icking-Konert
   \date 2014-03-14
   \version 0.1
   
   \brief implementation of main routine
   
   this is the main file containing browsing input parameters, calling the import and export routines

   usage: hexfile_merger [-h] -i infile_1 -i infile_2 ... [-o outfile]
      -h    print this help
      -i    name of hexfile to import (default: none)
      -o    name of output hexfile (default: outfile.s19)

   \note program not yet fully tested!
*/

// include files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include "hexfile.h"
#include "misc.h"
#define _MAIN_
  #include "globals.h"
#undef _MAIN_


/// version number (float)
#define VERSION 1.0

/// max length of filenames
#define  STRLEN   1000

/// buffer size [B] for hexfiles and memory map
#define  BUFSIZE  10*1024*1024



/**
   \fn int main(int argc, char *argv[])
   
   \brief main routine
   
   \param argc      number of commandline arguments + 1
   \param argv      string array containing commandline arguments (argv[0] contains name of executable)
   
   \return dummy return code (not used)
   
   Main routine for import and output
*/
int main(int argc, char ** argv) {
 
  char      *appname;                       // name of application without path
  char      infile[STRLEN]="";              // names of input hexfiles
  char      outfile[STRLEN]="outfile.s19";  // name of output hexfile
  char      *buf;                           // buffer for hexfiles
  uint16_t  *image;                         // memory image buffer (high byte != 0 indicates value is set)
  uint32_t  imageStart;                     // starting address of hexfile
  uint32_t  numBytes;                       // number of bytes in hexfile
  uint32_t  addrStart, addrStop;            // first and last adress in memory buffer
  uint32_t  i;                              // generic variable
  
  // initialize globals
  g_pauseOnExit = 1;                        // prompt for <return> prior to exit
  

  // debug: print all commandline arguments
  /*
  printf("\n\n");
  for (i=1; i<argc; i++)
    printf("'%s'\n", argv[i]);
  printf("\n");
  Exit(1);
  */
  
  
  // allocate buffers (>1MByte requires dynamic allocation)
  buf   = malloc(BUFSIZE * sizeof(uint8_t));
  image = malloc(BUFSIZE * sizeof(uint16_t));
  if ((!buf) || (!image)) {
    fprintf(stderr, "\n\nerror: cannot allocate buffers, try reducing BUFSIZE, exit!\n\n");
    Exit(1);
  }

  // get application name
  if (strrchr(argv[0],'\\'))
    appname = strrchr(argv[0],'\\')+1;         // windows
  else if (strrchr(argv[0],'/'))
    appname = strrchr(argv[0],'/')+1;          // Posix
  else
    appname = argv[0];


  // without commandline arguments print help
  if (argc == 1) {
    printf("\n");
    printf("usage: %s [-h] -i infile_1 -i infile_2 ... [-o outfile] [-q]\n\n", appname);
    printf("  -h    print this help\n");
    printf("  -i    name of s19/hex/txt file to import (default: none)\n");
    printf("  -o    name of output s19/hex/txt file (default: outfile.s19)\n");
    printf("  -q    don't prompt for <return> prior to exit (default: promt)\n");
    Exit(1);
  }


  // init memory image (high byte != 0 indicates value is set)
  for (i=0; i<BUFSIZE; i++)
    image[i] = 0x0000;


  // for determining start and stop address
  addrStart = 0xFFFFFFFF;
  addrStop  = 0x00000000;
  

  ////////
  // parse commandline arguments and import hexfiles
  ////////
  printf("\n%s (vers %1.1f)\n\n", appname, VERSION);
  printf("start import\n");
  for (i=1; i<argc; i++) {
    
    // import next hexfile into RAM immediately
    if (!strcmp(argv[i], "-i")) {
      
      // get hexfile name
      strncpy(infile, argv[++i], STRLEN-1);
      
      // import hexfile into string buffer (no interpretation, yet)
      load_hexfile(infile, buf, BUFSIZE);
    
      // convert to memory image, depending on file type 
      if (strstr(infile, ".s19") != NULL)                                             // Motorola S-record format
        convert_s19(buf, &imageStart, &numBytes, image, 0);
      else if ((strstr(infile, ".hex") != NULL) || (strstr(infile, ".ihx") != NULL))  // Intel HEX-format
        convert_hex(buf, &imageStart, &numBytes, image, 0);
      else if (strstr(infile, ".txt") != NULL)                                        // text table (hex addr / data)
        convert_txt(buf, &imageStart, &numBytes, image, 0);
      else {
        fprintf(stderr, "\n\nerror: unsupported input file format for '%s', exit!\n\n", infile);
        Exit(1);
      }

      // store new start & stop addresses
      if (imageStart < addrStart)
        addrStart = imageStart;
      if (imageStart+numBytes > addrStop)
        addrStop = imageStart+numBytes;

    } // -i -> import file


    // store name of output hexfile
    else if (!strcmp(argv[i], "-o"))
      strncpy(outfile, argv[++i], STRLEN-1);


    // don't promt for <return> prior to exit
    else if (!strcmp(argv[i], "-q"))
      g_pauseOnExit = 0;


    // else print help
    else {
      printf("\n");
      printf("usage: %s [-h] -i infile_1 -i infile_2 ... [-o outfile] [-q]\n\n", appname);
      printf("  -h    print this help\n");
      printf("  -i    name of s19/hex/txt file to import (default: none)\n");
      printf("  -o    name of output s19/hex/txt file (default: outfile.s19)\n");
      printf("  -q    don't prompt for <return> prior to exit (default: promt)\n");
      Exit(1);
    }

  } // process commandline arguments
  printf("done\n\n");



  ////////
  // export merged hexfile
  ////////
  printf("export to '%s' ... ", outfile);
  if (strstr(outfile, ".s19") != NULL)                                              // Motorola S-record format
    export_s19(outfile, image, addrStart, addrStop);
  else if ((strstr(outfile, ".hex") != NULL) || (strstr(outfile, ".ihx") != NULL))  // Intel HEX-format
    export_hex(outfile, image, addrStart, addrStop);
  else if (strstr(outfile, ".txt") != NULL)                                         // text table (hex addr / data)
    export_txt(outfile, image, addrStart, addrStop);
  else {
    fprintf(stderr, "\n\nerror: unsupported output file format for '%s', exit!\n\n", outfile);
    Exit(1);
  }
  printf("done\n\n");


  // debug: save in plain format
  //save_memory_image("dump.txt", image, addrStart, addrStop);


  // avoid compiler warnings
  return(0);
  
} // main


// end of file
