/**
  \file main.c

  \author G. Icking-Konert

  \brief implementation of main routine

  this is the main file containing browsing input parameters, 
  calling the import and export routines

  \note program not yet fully tested!
*/

// include files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <errno.h>
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
  char            appname[STRLEN];    // name of application without path
  char            version[100];       // version as string
  int             verbose;            // verbosity level (0=MUTE, 1=SILENT, 2=INFORM, 3=CHATTY)
  MemoryImage_s   image;              // memory image buffer as list of (addr, value)
  bool            printHelp;          // flag for printing help page
  char            tmp[STRLEN+106];    // misc string buffer


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


  // initialize memory image
  MemoryImage_init(&image);

  // set optional image debug level
  #if defined(MEMIMAGE_DEBUG)
    MemoryImage_setDebug(&image, 2);
  #endif

  // initialize defaults
  g_pauseOnExit         = false;      // no wait for <return> before terminating (dummy)
  g_backgroundOperation = false;      // assume foreground application
  verbose               = INFORM;     // verbosity level medium
  

  // debug: set memory image debug level
  #if defined(MEMIMAGE_DEBUG)
    MemoryImage_setDebug(&image, 2);
  #endif


  // get app name & version, and change console title
  get_app_name(argv[0], VERSION, appname, version);
  snprintf(tmp, sizeof(tmp), "%s (%s)", appname, version);
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
      if (i+1<argc) {
        i++;
        if ((!isDecString(argv[i])) || (sscanf(argv[i],"%d", &verbose) <= 0) || (verbose < 0) || (verbose > 3))
        {
          printf("\ncommand '-v/-verbose' requires a decimal parameter (0..3)\n");
          printHelp = true;
          break;
        }
      }
      else {
        printf("\ncommand '-v/-verbose' requires a decimal parameter (0..3)\n");
        printHelp = true;
        break;
      }
      if (verbose < MUTE)   verbose = MUTE;
      if (verbose > CHATTY) verbose = CHATTY;

    } // verbosity


    // skip file import. Just check parameter number and offset (bin only)
    else if (!strcmp(argv[i], "-import")) {

      // get file name
      if (i+1<argc) {
        i+=1;
        char *p = strrchr(argv[i], '.');
        if ((p != NULL ) && (!strcmp(p, ".bin"))) {   // for binary file assert additional address
          if (i+1<argc) {
            i+=1;
            if (!isHexString(argv[i])) {
              printf("\ncommand '-import' requires a hex offset for binary\n");
              printHelp = true;
              break;
            }
          }
          else {
            printf("\ncommand '-import' requires a hex offset for binary\n");
            printHelp = true;
            break;
          }
        }
      }
      else {
        printf("\ncommand '-import' requires a filename\n");
        printHelp = true;
        break;
      }

    } // import


    // skip file export. Just check parameter number
    else if (!strcmp(argv[i], "-export")) {
      if (i+1<argc) {
        i+=1;
      }
      else {
        printf("\ncommand '-export' requires a filename\n");
        printHelp = true;
        break;
      }
    } // export


    // skip print
    else if (!strcmp(argv[i], "-print")) {
      
      // dummy

    } // print


    // skip checksum
    else if (!strcmp(argv[i], "-checksum")) {
      
      // dummy

    } // checksum


    // skip memory filling. Just check parameter type
    else if (!strcmp(argv[i], "-fill")) {
      if (i+3<argc) {
        if ((!isHexString(argv[i+1])) || (!isHexString(argv[i+2])) || (!isHexString(argv[i+3]))) {
          printf("\ncommand '-fill' requires three hex parameters\n");
          printHelp = true;
          break;
        }
        i+=3;
      }
      else {
        printf("\ncommand '-fill' requires three hex parameters\n");
        printHelp = true;
        break;
      }
    } // fill


    // skip memory filling. Just check parameter type
    else if (!strcmp(argv[i], "-fillRand")) {
      if (i+2<argc) {
        if ((!isHexString(argv[i+1])) || (!isHexString(argv[i+2]))) {
          printf("\ncommand '-fillRand' requires two hex parameters\n");
          printHelp = true;
          break;
        }
        i+=2;
      }
      else {
        printf("\ncommand '-fillRand' requires two hex parameters\n");
        printHelp = true;
        break;
      }
    } // fillRand


    // skip memory clipping. Just check parameter type
    else if (!strcmp(argv[i], "-clip")) {
      if (i+2<argc) {
        if ((!isHexString(argv[i+1])) || (!isHexString(argv[i+2]))) {
          printf("\ncommand '-clip' requires two hex parameters\n");
          printHelp = true;
          break;
        }
        i+=2;
      }
      else {
        printf("\ncommand '-clip' requires two hex parameters\n");
        printHelp = true;
        break;
      }
    } // clip


    // skip cutting out memory range. Just check parameter type
    else if (!strcmp(argv[i], "-cut")) {
      if (i+2<argc) {
        if ((!isHexString(argv[i+1])) || (!isHexString(argv[i+2]))) {
          printf("\ncommand '-cut' requires two hex parameters\n");
          printHelp = true;
          break;
        }
        i+=2;
      }
      else {
        printf("\ncommand '-cut' requires two hex parameters\n");
        printHelp = true;
        break;
      }
    } // cut


    // skip memory copy. Just check parameter number
    else if (!strcmp(argv[i], "-copy")) {
      if (i+3<argc) {
        if ((!isHexString(argv[i+1])) || (!isHexString(argv[i+2])) || (!isHexString(argv[i+3]))) {
          printf("\ncommand '-copy' requires three hex parameters\n");
          printHelp = true;
          break;
        }
        i+=3;
      }
      else {
        printf("\ncommand '-copy' requires three hex parameters\n");
        printHelp = true;
        break;
      }
    } // copy


    // skip memory move. Just check parameter number
    else if (!strcmp(argv[i], "-move")) {
      if (i+3<argc) {
        if ((!isHexString(argv[i+1])) || (!isHexString(argv[i+2])) || (!isHexString(argv[i+3]))) {
          printf("\ncommand '-move' requires three hex parameters\n");
          printHelp = true;
          break;
        }
        i+=3;
      }
      else {
        printf("\ncommand '-move' requires three hex parameters\n");
        printHelp = true;
        break;
      }
    } // move


    // else print help
    else {
      printf("\nunknown command '%s' \n", argv[i]);
      printHelp = true;
      break;
    }

  } // 1st pass over commandline arguments


  // on request (-h) or in case of error print help page
  if ((printHelp==true) || (argc == 1)) {
    printf("\n");
    printf("\n%s (%s)\n\n", appname, version);
    printf("Import files of various formats, apply simple manipulations, and merge them to a single output file.\n");
    printf("For more information see https://github.com/gicking/hexfile_merger\n");
    printf("\n");
    printf("usage: %s with following options/commands:\n", appname);
    printf("    -h/-help                            print this help\n");
    printf("    -v/-verbose [level]                 set verbosity level 0..3 (default: 2)\n");
    printf("    -import [infile [addr]]             import from file to image. For binary file (*.bin) provide start address (in hex)\n");
    printf("    -export [outfile]                   export image to file\n");
    printf("    -print                              print image to console\n");
    printf("    -checksum                           print CRC32-IEEE checksum over data ranges in image\n");
    printf("    -fill [addrStart addrStop val]      fill specified range with fixed value (addr & val in hex)\n");
    printf("    -fillRand [addrStart addrStop]      fill specified range with random values in 0-255 (addr in hex)\n");
    printf("    -clip [addrStart addrStop]          clip image to specified range (addr in hex)\n");
    printf("    -cut  [addrStart addrStop]          cut specified data range from image (addr in hex)\n");
    printf("    -copy [fromStart fromStop toStart]  copy data within image (addr in hex). Keep old data\n");
    printf("    -move [fromStart fromStop toStart]  move data within image (addr in hex). Unset old data\n");
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
    printf("  - Intel Hex (*.hex, *.ihx)\n");
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


    // import next file into memory image
    else if (!strcmp(argv[i], "-import")) {

      // intermediate variables
      char      infile[STRLEN]="";     // name of input file
      uint64_t  addrStart;             // address offset for binary file

      // get file name
      strncpy(infile, argv[++i], STRLEN-1);

      // for binary file also get starting address
      char *p = strrchr(infile, '.');
      if ((p != NULL ) && (strstr(p, ".bin"))) {
        strncpy(tmp, argv[++i], STRLEN-1);
        sscanf(tmp, "%" SCNx64, &addrStart);
      }

      // import file to memory image, depending on type
      if ((p != NULL ) && ((!strcmp(p, ".s19")) || (!strcmp(p, ".S19")))) {        // Motorola S-record format
        import_file_s19(infile, &image, verbose);
      }
      else if ((p != NULL ) && (!strcmp(p, ".hex") || (!strcmp(p, ".HEX")) || (!strcmp(p, ".ihx")) || (!strcmp(p, ".IHX")))) {  // Intel hex format
        import_file_ihx(infile, &image, verbose);
      }
      else if ((p != NULL ) && ((!strcmp(p, ".txt")) || (!strcmp(p, ".TXT")))) {   // text table (hex addr / data)
        import_file_txt(infile, &image, verbose);
      }
      else if ((p != NULL ) && ((!strcmp(p, ".bin")) || (!strcmp(p, ".BIN")))) {   // binary file
        import_file_bin(infile, addrStart, &image, verbose);
      }
      else {
        MemoryImage_free(&image);
        Error("Input file %s has unsupported format (*.s19, *.hex, *.ihx, *.txt, *.bin)", infile);
      }

    } // import file


    // export RAM image to file
    else if (!strcmp(argv[i], "-export")) {

      // intermediate variables
      char      outfile[STRLEN]="";     // name of export file

      // get file name
      strncpy(outfile, argv[++i], STRLEN-1);

      // export to file with format depending on extension
      char *p = strrchr(outfile, '.');
      if ((p != NULL ) && ((!strcmp(p, ".s19")) || (!strcmp(p, ".S19"))))          // Motorola S-record format
        export_file_s19(outfile, &image, verbose);
      else if ((p != NULL ) && ((!strcmp(p, ".hex")) || (!strcmp(p, ".HEX")) || (!strcmp(p, ".ihx")) || (!strcmp(p, ".IHX"))))  // Intel hex format
        export_file_ihx(outfile, &image, verbose);
      else if ((p != NULL ) && ((!strcmp(p, ".txt")) || (!strcmp(p, ".TXT"))))     // text table (hex addr / data)
        export_file_txt(outfile, &image, verbose);
      else if ((p != NULL ) && ((!strcmp(p, ".bin")) || (!strcmp(p, ".BIN"))))     // binary file
        export_file_bin(outfile, &image, verbose);
      else {
        MemoryImage_free(&image);
        Error("Output file %s has unsupported format (*.s19, *.hex, *.ihx, *.txt, *.bin)", outfile);
      }

    } // export file


    // print memory image to console
    else if (!strcmp(argv[i], "-print")) {

      // print to stdout
      export_file_txt("console", &image, verbose);

    } // print memory image


    // print CRC32 checksum over image
    else if (!strcmp(argv[i], "-checksum")) {

      if (MemoryImage_isEmpty(&image)) {
        printf("  CRC32 chk skipped for empty image\n");
        break;
      }

      // for each consecutive memory range print CRC32 checksum to stdout
      MEMIMAGE_ADDR_T address = 0x00;
      size_t          idxStart, idxEnd;
      printf("  CRC32-IEEE:\n");
      while (MemoryImage_getMemoryBlock(&image, address, &idxStart, &idxEnd)) {
        MEMIMAGE_ADDR_T  addrStart = image.memoryEntries[idxStart].address;
        MEMIMAGE_ADDR_T  addrEnd   = image.memoryEntries[idxEnd].address;
        uint32_t         chk = MemoryImage_checksum_crc32(&image, idxStart, idxEnd);
        printf("    [0x%04" PRIX64 "; 0x%04" PRIX64 "]: 0x%08" PRIX32 "\n", (uint64_t) addrStart, (uint64_t) addrEnd, chk);
        address = addrEnd + 1;
      }

    } // print checksum


    // fill memory range with fixed value
    else if (!strcmp(argv[i], "-fill")) {

      // get start and stop adress of address window, and value to fill with
      uint64_t  addrStart, addrStop, value;
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &addrStart);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &addrStop);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &value);

      // fill specified memory range
      fill_image(&image, addrStart, addrStop, (uint8_t) value, verbose);

    } // fill memory range


    // fill memory range with random values in 0..255
    else if (!strcmp(argv[i], "-fillRand")) {

      // get start and stop adress of address window, and value to fill with
      uint64_t  addrStart, addrStop;
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &addrStart);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &addrStop);

      // fill specified memory range
      fill_image_random(&image, addrStart, addrStop, verbose);

    } // randomly fill memory range


    // clip memory image. Set values outside given window to unset
    else if (!strcmp(argv[i], "-clip")) {

      // get start and stop adress of address window
      uint64_t  addrStart, addrStop;
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &addrStart);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &addrStop);

      // clear all data outside specified window
      clip_image(&image, addrStart, addrStop, verbose);

    } // clip memory image


    // cut data range from memory image. Set values within given window to "undefined"
    else if (!strcmp(argv[i], "-cut")) {

      // get start and stop adress of address window
      uint64_t  addrStart, addrStop;
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &addrStart);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &addrStop);

      // cut all data inside specified window
      cut_image(&image, addrStart, addrStop, verbose);

    } // cut data range from memory image


    // copy data within in memory image
    else if (!strcmp(argv[i], "-copy")) {

      // get start and stop adress of address window
      uint64_t  sourceStart, sourceStop, targetStart;
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &sourceStart);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &sourceStop);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &targetStart);

      // clear all data inside specified window
      copy_image(&image, sourceStart, sourceStop, targetStart, verbose);

    } // copy data in memory image


    // move data within in memory image
    else if (!strcmp(argv[i], "-move")) {

      // get start and stop adress of address window
      uint64_t  sourceStart, sourceStop, targetStart;
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &sourceStart);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &sourceStop);
      strncpy(tmp, argv[++i], STRLEN-1);  sscanf(tmp, "%" SCNx64, &targetStart);

      // clear all data inside specified window
      move_image(&image, sourceStart, sourceStop, targetStart, verbose);

    } // move data in memory image


    // dummy parameter: skip, is treated in 1st pass
    else { ; }

  } // 2nd pass over commandline arguments


  // print message
  if (verbose != MUTE)
    printf("finished\n\n");

  // release memory image
  MemoryImage_free(&image);

  // avoid compiler warnings
  return(0);

} // main


// end of file
