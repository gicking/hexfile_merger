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
  #include "globals.h"
#undef _MAIN_


// max length of filenames
#define  STRLEN   1000



/**
   \fn int main(int argc, char *argv[])
   
   \brief main routine
   
   \param argc      number of commandline arguments + 1
   \param argv      string array containing commandline arguments (argv[0] contains name of executable)
   
   \return dummy return code (not used)
   
   Main routine for import and output
*/
int main(int argc, char ** argv) {
 
  char      appname[STRLEN];                // name of application without path
  char      version[100];                   // version as string
  int       verbose;                        // verbosity level (0..2)
  uint16_t  *imageBuf;                      // global RAM image buffer (high byte != 0 indicates value is set)
  uint32_t  addrStart;                      // start address for image buffer (corresponds to image[0])
  uint32_t  addrStop;                       // highest address in image buffer (corresponds to image[addrStop-addrStart])
  bool      printHelp;                      // flag for printing help page
  char      tmp[STRLEN];                    // misch buffer
  
  // initialize defaults
  g_pauseOnExit           = 0;              // no wait for <return> before terminating (dummy)
  g_backgroundOperation   = 0;              // assume foreground application
  verbose                 = 1;              // verbosity level (0..2)

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

  
  ////////
  // 1st pass of commandline arguments: set global parameters, no import/export yet
  ////////
  printHelp = false;
  for (int i=1; i<argc; i++) {
    
    // skip file import. Just check parameter number
    if ((!strcmp(argv[i], "--input")) || (!strcmp(argv[i], "-i"))) {

      // get file name
      if (i<argc-1) {
        if (strstr(argv[++i], ".bin") != NULL) {  // for binary file skip additionaly address
          if (i<argc-1)
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
    } // input
      
    // skip file export. Just check parameter number
    else if ((!strcmp(argv[i], "--output")) || (!strcmp(argv[i], "-o"))) {
      if (i<argc-1)
        i+=1;
      else {
        printHelp = true;
        break;
      }
    }
      
    // skip printing of RAM image
    else if ((!strcmp(argv[i], "--print")) || (!strcmp(argv[i], "-p"))) {
      // dummy
    }

    // set verbosity level (0..2)
    else if ((!strcmp(argv[i], "--verbose")) || (!strcmp(argv[i], "-v"))) {
      if (i<argc-1)
        sscanf(argv[++i],"%d",&verbose);
      else {
        printHelp = true;
        break;
      }
      if (verbose < 0) verbose = 0;
      if (verbose > 2) verbose = 2;
    }

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
    printf("Import multiple files of various formats and merge them to a single output file.\n");
    printf("For more information see https://github.com/gicking/hexfile_merger\n");
    printf("\n");
    printf("usage: %s [-h] [-i infile] [-i binfile addr] ... [-o outfile] [-p] [-v level]\n", appname);
    printf("    -h / --help     print this help\n");
    printf("    -i / --input    name of input file (for '*.bin' plus starting address, default: none)\n");
    printf("    -o / --output   name of output file (default: outfile.txt)\n");
    printf("    -p / --print    print memory image to console\n");
    printf("    -v / --verbose  verbosity level 0..2 (default: 1)\n");
    printf("\n");
    printf("Supported import formats:\n");
    printf("  - Motorola S19 (*.s19), for a description see https://en.wikipedia.org/wiki/SREC_(file_format)\n");
    printf("  - Intel Hex (*.hex, *.ihx), for a description see https://en.wikipedia.org/wiki/Intel_HEX\n");
    printf("  - ASCII table (*.txt) consisting of lines with 'hexAddr  value'. Lines starting with '#' are ignored\n");
    printf("  - Binary (*.bin) with an additional starting address\n");
    printf("\n");
    printf("Supported export formats:\n");
    printf("  - Motorola S19 (*.s19)\n");
    printf("  - ASCII table (*.txt) with 'hexAddr  hexValue'\n");
    printf("  - Binary (*.bin) without starting address\n");
    printf("\n");
    printf("Files are imported and exported in the specified order, i.e. later imports may\n");
    printf("overwrite previous imports. Also outputs only contain the previous imports, i.e.\n");
    printf("intermediate exports only contain the merged content up to that point in time.\n");
    printf("\n");
    Exit(1,0);
  }


  // print message
  if (verbose)
    printf("\n%s (%s)\n", appname, version);


  ////////
  // 2nd pass of commandline arguments: import & export files
  ////////

  // allocate and init global RAM image (>1MByte requires dynamic allocation)
  if (!(imageBuf = malloc(LENIMAGEBUF * sizeof(*imageBuf))))
    Error("Cannot allocate image buffer, try reducing LENIMAGEBUF");
  memset(imageBuf, 0, LENIMAGEBUF * sizeof(*imageBuf));

  // init global addresses
  addrStart = 0xFFFFFFFF;
  addrStop  = 0x00000000;
  
  // loop over commandline arguments
  for (int i=1; i<argc; i++) {
    
    // import next ASCII file into RAM
    if ((!strcmp(argv[i], "--input")) || (!strcmp(argv[i], "-i"))) {
      
      // intermediate variables
      char      infile[STRLEN]="";     // name of input file
      char      *fileBuf;              // RAM buffer for input file
      uint32_t  lenFile;               // length of file in fileBuf
      uint16_t  *tmpImageBuf;          // intermediate RAM image buffer
      uint32_t  tmpAddrStart;          // start address for image buffer (corresponds to image[0])
      uint32_t  tmpAddrStop;           // highest address in image buffer (corresponds to image[addrStop-addrStart])

      // allocate intermediate buffers (>1MByte requires dynamic allocation)
      if (!(fileBuf = malloc(LENFILEBUF * sizeof(*fileBuf))))
        Error("Cannot allocate file buffer, try reducing LENFILEBUF");
      if (!(tmpImageBuf = malloc(LENIMAGEBUF * sizeof(*tmpImageBuf))))
        Error("Cannot allocate temporary image buffer, try reducing LENIMAGEBUF");

      // get file name
      strncpy(infile, argv[++i], STRLEN-1);
      
      // for binary file also get starting address
      if (strstr(infile, ".bin") != NULL) {
        strncpy(tmp, argv[++i], STRLEN-1);
        sscanf(tmp, "%x", &tmpAddrStart);
      }

      // import file into string buffer (no interpretation, yet)
      load_file(infile, fileBuf, &lenFile, verbose);
    

      // convert to memory image, depending on file type 
      if (strstr(infile, ".s19") != NULL)   // Motorola S-record format
        convert_s19(fileBuf, lenFile, tmpImageBuf, &tmpAddrStart, &tmpAddrStop, verbose);
      else if ((strstr(infile, ".hex") != NULL) || (strstr(infile, ".ihx") != NULL))   // Intel HEX-format
        convert_ihx(fileBuf, lenFile, tmpImageBuf, &tmpAddrStart, &tmpAddrStop, verbose);
      else if (strstr(infile, ".txt") != NULL)   // text table (hex addr / data)
        convert_txt(fileBuf, lenFile, tmpImageBuf, &tmpAddrStart, &tmpAddrStop, verbose);
      else if (strstr(infile, ".bin") != NULL)   // binary file
        convert_bin(fileBuf, lenFile, tmpImageBuf, tmpAddrStart, &tmpAddrStop, verbose);
      else
        Error("Input file %s has unsupported format (*.s19, *.hex, *.ihx, *.txt, *.bin)", infile);

      
      //////
      // merge global and intermediate memory image
      //////
      
      // initial file --> just copy
      if (addrStart > addrStop) {

	// copy data
        memcpy(imageBuf, tmpImageBuf, (tmpAddrStop-tmpAddrStart+1)*sizeof(*imageBuf));
        addrStart = tmpAddrStart;
        addrStop  = tmpAddrStop;

      } // inital file

      // merge images
      else {

        // new start address is lower than global image
        if (tmpAddrStart < addrStart) {

          // shift old data up and pad with 0
          uint32_t offset = addrStart-tmpAddrStart;
          memmove(&(imageBuf[offset]), imageBuf, (LENIMAGEBUF-offset)*sizeof(*imageBuf));
          memset(imageBuf, 0, offset * sizeof(*imageBuf));

          // copy new data "below" old image. Only consider set data
          for (int i=0; i<(tmpAddrStop-tmpAddrStart+1); i++) {
            if (tmpImageBuf[i])
              imageBuf[i] = tmpImageBuf[i];
          }
  
        } // new lower address offset

        // new data is "above" old data (tmpAddrStart>=addrStart)
        else {

          // copy new data to global image. Only consider set data
          uint32_t offset = tmpAddrStart - addrStart;
          for (int i=0; i<(tmpAddrStop-tmpAddrStart+1); i++) {
            if (tmpImageBuf[i])
              imageBuf[i+offset] = tmpImageBuf[i];
          }

        
        } // new data is "above" old data

        // store new global address limits
        if (tmpAddrStart < addrStart)
          addrStart = tmpAddrStart;
        if (tmpAddrStop > addrStop)
          addrStop = tmpAddrStop;
        
      } // merge files

      // debug
      /*
      printf("\nfile: 0x%04X..0x%04X\n", tmpAddrStart, tmpAddrStop);
      for (int i=0; i<tmpAddrStop-tmpAddrStart+1; i++) {
        if (tmpImageBuf[i])
          printf("  %3d   0x%04x   0x%04x   0x%02x\n", i, tmpAddrStart+i, tmpImageBuf[i], tmpImageBuf[i] & 0xFF);
      }
      printf("\nglobal: 0x%04X..0x%04X\n", addrStart, addrStop);
      for (int i=0; i<addrStop-addrStart+1; i++) {
        if (imageBuf[i])
          printf("  %3d   0x%04x   0x%04x   0x%02x\n", i, addrStart+i, imageBuf[i], imageBuf[i] & 0xFF);
      }
      printf("\n");
      //Exit(1,0);      
      */

      // release intermediate buffers
      free(fileBuf);
      free(tmpImageBuf);

    } // import file


    // export current status of RAM image to file
    else if ((!strcmp(argv[i], "--output")) || (!strcmp(argv[i], "-o"))) {
  
      // intermediate variables
      char      outfile[STRLEN]="";     // name of export file

      // get file name
      strncpy(outfile, argv[++i], STRLEN-1);
      
      // export in format depending on file extension 
      if (strstr(outfile, ".s19") != NULL)   // Motorola S-record format
        export_s19(outfile, imageBuf, addrStart, addrStop, verbose);
      else if (strstr(outfile, ".txt") != NULL)   // text table (hex addr / data)
        export_txt(outfile, imageBuf, addrStart, addrStop, verbose);
      else if (strstr(outfile, ".bin") != NULL)   // binary format
        export_bin(outfile, imageBuf, addrStart, addrStop, verbose);
      else
        Error("Unsupported output file extension of '%s' (*.s19, *.txt)", outfile);

    } // export file


    // print RAM image to console
    else if ((!strcmp(argv[i], "--print")) || (!strcmp(argv[i], "-p"))) {
  
      // print to console
      print_console(imageBuf, addrStart, addrStop, verbose);
      
    } // print RAM image to console


    // dummy parameter: skip, is treated in 1st pass
    else {

    }

  } // 2nd pass over commandline arguments

  // print message
  if (verbose)
    printf("finished\n\n");

  // release global buffer
  free(imageBuf);

  // avoid compiler warnings
  return(0);
  
} // main


// end of file
