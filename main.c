/**
   \file main.c

   \author G. Icking-Konert
   \date 2018-12-14
   \version 0.2
   
   \brief implementation of main routine
   
   this is the main file containing browsing input parameters, calling the import and export routines

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
#include "version.h"
#define _MAIN_
  #include "main.h"
#undef _MAIN_


/**
   \fn int main(int argc, char *argv[])
   
   \param argc      number of commandline arguments + 1
   \param argv      string array containing commandline arguments (argv[0] contains name of executable)
   
   \return dummy return code (not used)
   
   Main routine for import and output
*/
int main(int argc, char ** argv) {
 
  // local variables
  char      appname[STRLEN];      // name of application without path
  char      version[100];         // version as string
  int       verbose;              // verbosity level (0=MUTE, 1=SILENT, 2=INFORM, 3=CHATTY)
  uint16_t  *imageBuf;            // global RAM image buffer (high byte != 0 indicates value is set)
  bool      printHelp;            // flag for printing help page
  char      tmp[STRLEN];          // misc buffer
  

  // initialize defaults
  g_pauseOnExit         = false;  // no wait for <return> before terminating (dummy)
  g_backgroundOperation = false;  // assume foreground application
  verbose               = INFORM; // verbosity level medium

  // debug: print arguments
  /*
  printf("\n\narguments:\n");
  for (i=0; i<argc; i++) { 
    //printf("  %d: '%s'\n", (int) i, argv[i]);
    printf("%s ", argv[i]);
  }
  printf("\n\n");
  exit(1);
  */
  

  // get app name & version, and change console title
  get_app_name(argv[0], VERSION, appname, version);
  sprintf(tmp, "%s (%s)", appname, version);
  setConsoleTitle(tmp);  

  
  /////////////////
  // 1st pass of commandline arguments: set global parameters, no import/export yet
  /////////////////
  
  printHelp = false;
  for (int i=1; i<argc; i++) {

    // print help
    if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "-help"))) {

      // set flag for printing help
      printHelp = true;
      break;

    } // help
    

    // set verbosity level (0..3)
    else if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "-verbose"))) {
      
      // get verbosity level
      if (i+1<argc)
        sscanf(argv[++i],"%d",&verbose);
      else {
        printHelp = true;
        break;
      }
      if (verbose < MUTE)   verbose = MUTE;
      if (verbose > CHATTY) verbose = CHATTY;

    } // verbosity
    

    // skip file import. Just check parameter number
    else if (!strcmp(argv[i], "-import")) {

      // get file name
      if (i+1<argc) {
        if (strstr(argv[++i], ".bin") != NULL) {  // for binary file skip additionaly address
          if (i+1<argc)
            i+=1;
          else {
            printHelp = true;
            break;
          }
        }
      }
      else {
        printHelp = true;
        break;
      }

    } // import
      

    // skip file export. Just check parameter number
    else if (!strcmp(argv[i], "-export")) {
      if (i+1<argc)
        i+=1;
      else {
        printHelp = true;
        break;
      }
    } // export
      

    // skip print
    else if (!strcmp(argv[i], "-print")) {

    } // print
      

    // skip memory clipping. Just check parameter number
    else if (!strcmp(argv[i], "-clip")) {
      if (i+2<argc)
        i+=2;
      else {
        printHelp = true;
        break;
      }
    } // clip
      

    // skip memory clearing. Just check parameter number
    else if (!strcmp(argv[i], "-clear")) {
      if (i+2<argc)
        i+=2;
      else {
        printHelp = true;
        break;
      }
    } // clear
      

    // skip memory copy. Just check parameter number
    else if (!strcmp(argv[i], "-copy")) {
      if (i+3<argc)
        i+=3;
      else {
        printHelp = true;
        break;
      }
    } // copy
      

    // skip memory move. Just check parameter number
    else if (!strcmp(argv[i], "-move")) {
      if (i+3<argc)
        i+=3;
      else {
        printHelp = true;
        break;
      }
    } // move


    // else print help
    else {
      printHelp = true;
      break;
    }

  } // 1st pass over commandline arguments

  
  // on request (-h) or in case of parameter error print help page
  if ((printHelp==true) || (argc == 1)) {
    printf("\n");
    printf("\n%s (%s)\n\n", appname, version);
    printf("Import files of various formats, apply simple manipulations, and merge them to a single output file.\n");
    printf("For more information see https://github.com/gicking/hexfile_merger\n");
    printf("\n");
    printf("usage: %s with following options/commands:\n", appname);
    printf("    -h/-help                            print this help\n");
    printf("    -v/-verbose [level]                 set verbosity level 0..3 (default: 2)\n");
    printf("    -import [infile [addr]]             import from file to image. For binary file (*.bin) add start address in hex\n");
    printf("    -export [outfile]                   export image to file\n");
    printf("    -print                              print image to console\n");
    printf("    -clip [addrStart addrStop]          clip image to specified range (in hex)\n");
    printf("    -clear [addrStart addrStop]         clear image data in specified range (in hex)\n");
    printf("    -copy [fromStart fromStop toStart]  copy data within image (in hex). Keep old data\n");
    printf("    -move [fromStart fromStop toStart]  move data within image (in hex). Unset old data\n");
    printf("\n");
    printf("Supported import formats:\n");
    printf("  - Motorola S19 (*.s19), see https://en.wikipedia.org/wiki/SREC_(file_format)\n");
    printf("  - Intel Hex (*.hex, *.ihx), see https://en.wikipedia.org/wiki/Intel_HEX\n");
    printf("  - ASCII table (*.txt) consisting of lines with 'addr  value' (dec or hex). Lines starting with '#' are ignored\n");
    printf("  - Binary data (*.bin) with an additional starting address\n");
    printf("\n");
    printf("Supported export formats:\n");
    printf("  - print to stdout (-print)\n");
    printf("  - Motorola S19 (*.s19)\n");
    printf("  - ASCII table (*.txt) with 'hexAddr  hexValue'\n");
    printf("  - Binary data (*.bin) without starting address\n");
    printf("\n");
    printf("Files are imported and exported in the specified order, i.e. later imports may\n");
    printf("overwrite previous imports. Also outputs only contain the previous imports, i.e.\n");
    printf("intermediate exports only contain the merged content up to that point in time.\n");
    printf("\n");
    Exit(0,0);
  }


  // print message
  if (verbose != MUTE)
    printf("\n%s (%s)\n", appname, version);


  /////////////////
  // 2nd pass of commandline arguments: execute actions, e.g. import & export files
  /////////////////

  // allocate and init global RAM image (>1MByte requires dynamic allocation)
  if (!(imageBuf = malloc(LENIMAGEBUF * sizeof(*imageBuf))))
    Error("Cannot allocate image buffer, try reducing LENIMAGEBUF");
  memset(imageBuf, 0, LENIMAGEBUF * sizeof(*imageBuf));

  // loop over commandline arguments
  for (int i=1; i<argc; i++) {
    
    // skip print help (already treated in 1st pass)
    if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "-help"))) {
      i += 0;   // dummy
    } // help


    // skip verbosity level and parameters (already treated in 1st pass)
    else if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "-verbose"))) {
      i+=1;
    } // verbose


    // import next ASCII file into RAM
    else if (!strcmp(argv[i], "-import")) {
      
      // intermediate variables
      char      infile[STRLEN]="";     // name of input file
      char      *fileBuf;              // RAM buffer for input file
      uint32_t  lenFile;               // length of file in fileBuf
      uint32_t  addrStart;             // address offset for binary file

      // allocate intermediate buffer (>1MByte requires dynamic allocation)
      if (!(fileBuf = malloc(LENFILEBUF * sizeof(*fileBuf))))
        Error("Cannot allocate file buffer, try reducing LENFILEBUF");

      // get file name
      strncpy(infile, argv[++i], STRLEN-1);
      
      // for binary file also get starting address
      if (strstr(infile, ".bin") != NULL) {
        strncpy(tmp, argv[++i], STRLEN-1);
        sscanf(tmp, "%x", &addrStart);
      }

      // import file into string buffer (no interpretation, yet)
      load_file(infile, fileBuf, &lenFile, verbose);

      // convert to memory image, depending on file type 
      if (strstr(infile, ".s19") != NULL)   // Motorola S-record format
        convert_s19(fileBuf, lenFile, imageBuf, verbose);
      else if ((strstr(infile, ".hex") != NULL) || (strstr(infile, ".ihx") != NULL))   // Intel HEX-format
        convert_ihx(fileBuf, lenFile, imageBuf, verbose);
      else if (strstr(infile, ".txt") != NULL)   // text table (hex addr / data)
        convert_txt(fileBuf, lenFile, imageBuf, verbose);
      else if (strstr(infile, ".bin") != NULL)   // binary file
        convert_bin(fileBuf, lenFile, addrStart, imageBuf, verbose);
      else
        Error("Input file %s has unsupported format (*.s19, *.hex, *.ihx, *.txt, *.bin)", infile);
 
      // debug
      #if defined(DEBUG)
      { 
        uint32_t addr, addrStart, addrStop, numData;
        get_image_size(imageBuf, 0, LENIMAGEBUF, &addrStart, &addrStop, &numData);
        printf("\n\n");
        printf("addr: 0x%04X..0x%04X   %d\n", addrStart, addrStop, numData);
        for (addr=0; addr<LENIMAGEBUF; addr++) {
          if (imageBuf[addr] & 0xFF00)
            printf(" 0x%04x   0x%02x\n", addr, (int) (imageBuf[addr]) & 0xFF);
        }
        printf("\n");
        //Exit(1,0);
      }
      #endif

      // release intermediate buffers
      free(fileBuf);
      
    } // import file


    // export RAM image to file
    else if (!strcmp(argv[i], "-export")) {
  
      // intermediate variables
      char      outfile[STRLEN]="";     // name of export file

      // get file name
      strncpy(outfile, argv[++i], STRLEN-1);
      
      // export in format depending on file extension 
      if (strstr(outfile, ".s19") != NULL)   // Motorola S-record format
        export_s19(outfile, imageBuf, verbose);
      else if (strstr(outfile, ".txt") != NULL)   // text table (hex addr / hex data)
        export_txt(outfile, imageBuf, verbose);
      else if (strstr(outfile, ".bin") != NULL)   // binary format
        export_bin(outfile, imageBuf, verbose);
      else
        Error("Unsupported output file extension of '%s' (*.s19, *.txt)", outfile);

    } // export file


    // print RAM image to console
    else if (!strcmp(argv[i], "-print")) {
  
      // print to stdout
      export_txt("console", imageBuf, verbose);

    } // export file/print
      

    // clip memory image. Set values outside given window to unset
    else if (!strcmp(argv[i], "-clip")) {

      // get start and stop adress of address window
      uint32_t  addrStart, addrStop;
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%x", &addrStart);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%x", &addrStop);

      // clear all data outside specified window
      clip_image(imageBuf, addrStart, addrStop, verbose);

    } // clip memory image
      

    // clear data in memory image. Set values within given window to "undefined"
    else if (!strcmp(argv[i], "-clear")) {

      // get start and stop adress of address window
      uint32_t  addrStart, addrStop;
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%x", &addrStart);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%x", &addrStop);

      // clear all data inside specified window
      clear_image(imageBuf, addrStart, addrStop, verbose);

    } // clear data in memory image
      

    // copy data within in memory image
    else if (!strcmp(argv[i], "-copy")) {

      // get start and stop adress of address window
      uint32_t  sourceStart, sourceStop, targetStart;
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%x", &sourceStart);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%x", &sourceStop);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%x", &targetStart);

      // clear all data inside specified window
      copy_image(imageBuf, sourceStart, sourceStop, targetStart, verbose);

    } // copy data in memory image
      

    // move data within in memory image
    else if (!strcmp(argv[i], "-move")) {

      // get start and stop adress of address window
      uint32_t  sourceStart, sourceStop, targetStart;
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%x", &sourceStart);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%x", &sourceStop);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%x", &targetStart);

      // clear all data inside specified window
      move_image(imageBuf, sourceStart, sourceStop, targetStart, verbose);

    } // move data in memory image


    // dummy parameter: skip, is treated in 1st pass
    else {
      // dummy
    }

  } // 2nd pass over commandline arguments


  // print message
  if (verbose != MUTE)
    printf("finished\n\n");

  // release global buffer
  free(imageBuf);

  // avoid compiler warnings
  return(0);
  
} // main


// end of file
