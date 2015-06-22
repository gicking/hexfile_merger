/**
   \file hexfile.c
   
   \author G. Icking-Konert
   \date 2014-03-16
   \version 0.1
   
   \brief implementation of routines to import HEX and S19 files
   
   implementation of routines for importing and interpreting hexfiles in
   Motorola s19 and Intel hex format 
   (description under http://en.wikipedia.org/wiki/SREC_(file_format) or
   http://www.keil.com/support/docs/1584.htm). 
*/

// include files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <ctype.h>
#include "hexfile.h"
#include "misc.h"



/**
   \fn char *get_line(char **buf, char *line)
   
   \brief read next line from RAM buffer
   
   \param[in]  buf        pointer to read from (is updated)
   \param[out] line       pointer to line read (has to be large anough)
   
   read line (until LF, CR, or EOF) from RAM buffer and advance buffer pointer.
   memory for line has to be allocated externally
*/
char *get_line(char **buf, char *line) {
  
  char  *p = line;
  
  // copy line
  while ((**buf!=10) && (**buf!=13) && (**buf!=0)) {
    *line = **buf;
    line++;
    (*buf)++;
  }
  
  // skip CR + LF in buffer
  while ((**buf==10) || (**buf==13))
    (*buf)++;
    
  // terminate line
  *line = '\0';
  
  // check if data was copied
  if (p == line)
    return(NULL);
  else
    return(p);
  
} // get_line

  

/**
   \fn load_hexfile(char *filename, char *buf)
   
   \brief read hexfile into memory buffer
   
   \param[in]  filename   name of hexfile to read
   \param[out] buf        memory buffer containing file content (0-terminated)
   \param[in]  bufsize    max size of memory buffer
   
   read hexfile from file to memory buffer. Don't interpret (is done
   in separate routine)
*/
void load_hexfile(char *filename, char *buf, uint32_t bufsize) {

  FILE      *fp;
  char      *tmp2;
  uint32_t  len;
  
  // print message (if present, strip path)
  tmp2 = strrchr(filename, '/');
  if (tmp2)
    printf("  load file '%s' ... ", tmp2+1);
  else
    printf("  load file '%s' ... ", filename);
  fflush(stdout);
  
  // open file to read
  if (!(fp = fopen(filename, "rb"))) {
    fprintf(stderr, "\n\nerror in 'load_hexfile()': cannot open file, exit!\n\n");
    Exit(1);
  }
     
  // get filesize
  fseek(fp, 0, SEEK_END);
  len = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  // read file to buffer
  fread(buf, len, 1, fp);
  
  // close file again
  fclose(fp);

  // attach 0 to buffer to detect EOF
  buf[len++] = 0;
  
  // print message
  if (len>2048)
    printf("done (%1.1fkB)\n", (float) len/1024.0);
  else if (len>0)
    printf("done (%dB)\n", len);
  else
    printf("done, no data read\n");
  fflush(stdout);

} // load_hexfile

  

/**
   \fn void convert_s19(char *buf, uint32_t *addrStart, uint32_t *numBytes, uint16_t *image, uint8_t verbose)
   
   \brief convert s19 format in memory buffer to memory image
   
   \param[in]  buf        memory buffer to read from (0-terminated)
   \param[out] addrStart  start address of image (=lowest address in hexfile)
   \param[out] numBytes   number of bytes in image
   \param[out] image      RAM image of hexfile (high byte != 0 indicates value is set)
   \param[in]  verbose    level of verbosity and if application exits on fail 
   
   convert memory buffer containing s19 hexfile to memory buffer. For description of 
   Motorola S19 file format see http://en.wikipedia.org/wiki/SREC_(file_format)
*/
void convert_s19(char *buf, uint32_t *addrStart, uint32_t *numBytes, uint16_t *image, uint8_t verbose) {
  
  char      line[1000], tmp[1000], *p;
  int       linecount, idx, i;
  uint8_t   type, len, chkRead, chkCalc;
  uint32_t  addr, addrMin, addrMax, val;
  
  // print message (if present, strip path)
  if (verbose) {
    printf("  convert s19 ... ");
    fflush(stdout);
  }
  
  // 1st run: check syntax and extract min/max addresses
  linecount = 0;
  addrMin = 0xFFFFFFFF;
  addrMax = 0x00000000;
  p = buf;
  while (get_line(&p, line)) {
  
    // increase line counter
    linecount++;
    chkCalc = 0x00;
    
    // check 1st char (must be 'S')
    if (line[0] != 'S') {
      fprintf(stderr, "\n\nerror in 'convert_s19()': line %d does not start with 'S', exit!\n\n", linecount);
      Exit(1);
    }
    
    // record type
    type = line[1]-48;
    
    // skip if line contains no data
    if ((type==0) || (type==8) || (type==9))
      continue; 
    
    // record length (address + data + checksum)
    sprintf(tmp,"0x00");
    strncpy(tmp+2, line+2, 2);
    sscanf(tmp, "%x", &val);
    len = val;
    chkCalc += val;              // increase checksum
    
    // address (S1=16bit, S2=24bit, S3=32bit)
    addr = 0;
    for (i=0; i<type+1; i++) {
      sprintf(tmp,"0x00");
      tmp[2] = line[4+(i*2)];
      tmp[3] = line[5+(i*2)];
      sscanf(tmp, "%x", &val);
      addr *= 256;
      addr += val;    
      chkCalc += val;
    } 

    // read record data
    idx=6+(type*2);                // start at position 8, 10, or 12, depending on record type
    len=len-1-(1+type);            // substract chk and address length
    for (i=0; i<len; i++) {
      sprintf(tmp,"0x00");
      strncpy(tmp+2, line+idx, 2);    // get next 2 chars as string
      sscanf(tmp, "%x", &val);        // interpret as hex data
      chkCalc += val;                 // increase checksum
      idx+=2;                         // advance 2 chars in line
    }

    // checksum
    sprintf(tmp,"0x00");
    strncpy(tmp+2, line+idx, 2);
    sscanf(tmp, "%x", &val);
    chkRead = val;
    
    // assert checksum (0xFF xor (sum over all except record type)
    chkCalc ^= 0xFF;                 // invert checksum
    if (chkCalc != chkRead) {
      fprintf(stderr, "\n\nerror in 'convert_s19()': line %d has wrong checksum (0x%02x vs. 0x%02x), exit!\n\n", linecount, chkRead, chkCalc);
      Exit(1);
    }
    
    // store min/max address
    if (addr < addrMin)
      addrMin = addr;
    if (addr+len-1 > addrMax)
      addrMax = addr+len-1;
    
  } // while !EOF
      
  // store base address and image size
  *addrStart = addrMin;
  if ((addrMin != 0xFFFFFFFF) || (addrMax != 0x00000000))
    *numBytes  = addrMax-addrMin+1;
  else
    *numBytes  = 0;
       
  
  // 2nd run: store data to image
  if (*numBytes != 0) {
    p = buf;
    while (get_line(&p, line)) {
    
      // record type
      type = line[1]-48;
    
      // skip if line contains no data
      if ((type==0) || (type==8) || (type==9))
        continue; 
    
      // record length (address + data + checksum)
      sprintf(tmp,"0x00");
      strncpy(tmp+2, line+2, 2);
      sscanf(tmp, "%x", &val);
      len = val;
    
      // address (S1=16bit, S2=24bit, S3=32bit)
      addr = 0;
      for (i=0; i<type+1; i++) {
        sprintf(tmp,"0x00");
        strncpy(tmp+2, line+4+(i*2), 2);
        sscanf(tmp, "%x", &val);
        addr *= 256;
        addr += val;    
      }
    
      // read record data
      idx=6+(type*2);                // start at position 8, 10, or 12, depending on record type
      len=len-1-(1+type);            // substract chk and address length
      for (i=0; i<len; i++) {
        sprintf(tmp,"0x00");
        strncpy(tmp+2, line+idx, 2);                // get next 2 chars as string
        sscanf(tmp, "%x", &val);                    // interpret as hex data
        image[addr+i] = (uint16_t) val | 0xFF00;    // store data byte in buffer and set high byte
        idx+=2;                                     // advance 2 chars in line
      }
    
    } // while !EOF
    
  } // if numBytes!=0
  
  /*
  printf("\n");
  for (i=0; i<(*numBytes); i++)
    printf("%3d   0x%04x   0x%02x\n", i, addrMin+i, (int) (image[addrMin+i]) & 0xFF);
  printf("\n");
  Exit(1);
  */

  // print message
  if (verbose) {
    if ((*numBytes)>2048)
      printf("done (%1.1fkB @ 0x%04x)\n", (float) (*numBytes)/1024.0, addrMin);
    else if ((*numBytes)>0)
      printf("done (%dB @ 0x%04x)\n", *numBytes, addrMin);
    else
      printf("done, no data read\n");
    fflush(stdout);
  }
  
} // convert_s19

  

/**
   \fn void convert_hex(char *buf, uint32_t *addrStart, uint32_t *numBytes, uint16_t *image, uint8_t verbose)
   
   \brief convert intel hex format in memory buffer to memory image
   
   \param[in]  buf        memory buffer to read from (0-terminated)
   \param[out] addrStart  start address of image (=lowest address in hexfile)
   \param[out] numBytes   number of bytes in image
   \param[out] image      RAM image of hexfile (high byte != 0 indicates value is set)
   \param[in]  verbose    level of verbosity and if application exits on fail 
   
   convert memory buffer containing intel hexfile to memory buffer. For description of 
   Intel hex file format see http://en.wikipedia.org/wiki/Intel_HEX
*/
void convert_hex(char *buf, uint32_t *addrStart, uint32_t *numBytes, uint16_t *image, uint8_t verbose) {
  
  char      line[1000], tmp[1000], *p;
  int       linecount, idx, i;
  uint8_t   type, len, chkRead, chkCalc;
  uint32_t  addr, addrMin, addrMax, addrOff, val;
  
  // print message (if present, strip path)
  if (verbose) {
    printf("  convert hex ... ");
    fflush(stdout);
  }
  
  // 1st run: check syntax and extract min/max addresses
  linecount = 0;
  addrMin = 0xFFFFFFFF;
  addrMax = 0x00000000;
  addrOff = 0x00000000;
  p = buf;
  while (get_line(&p, line)) {
  
    // increase line counter
    linecount++;
    chkCalc = 0x00;
    
    // check 1st char (must be ':')
    if (line[0] != ':') {
      fprintf(stderr, "\n\nerror in 'convert_hex()': line %d does not start with 'S', exit!\n\n", linecount);
      Exit(1);
    }
    
    // record length (address + data + checksum)
    sprintf(tmp,"0x00");
    strncpy(tmp+2, line+1, 2);
    sscanf(tmp, "%x", &val);
    len = val;
    chkCalc += len;              // increase checksum
    
    // 16b address
    addr = 0;
    sprintf(tmp,"0x0000");
    strncpy(tmp+2, line+3, 4);
    sscanf(tmp, "%x", &val);
    chkCalc += (uint8_t) (val >> 8);
    chkCalc += (uint8_t)  val;
    addr = val + addrOff;         // add offset for >64kB addresses

    // record type
    sprintf(tmp,"0x00");
    strncpy(tmp+2, line+7, 2);
    sscanf(tmp, "%x", &val);
    type = val;
    chkCalc += type;              // increase checksum
    
    // record contains data
    if (type==0) {
      idx = 9;                          // start at index 9
      for (i=0; i<len; i++) {
        sprintf(tmp,"0x00");
        strncpy(tmp+2, line+idx, 2);    // get next 2 chars as string
        sscanf(tmp, "%x", &val);        // interpret as hex data
        chkCalc += val;                 // increase checksum
        idx+=2;                         // advance 2 chars in line
      }
    } // type==0

    // EOF indicator
    else if (type==1)
      continue; 
    
    // extended address (=upper 16b of address for following data records)
    else if (type==4) {
      sprintf(tmp,"0x0000");
      strncpy(tmp+2, line+9, 4);        // get next 4 chars as string
      sscanf(tmp, "%x", &val);        // interpret as hex data
      chkCalc += (uint8_t) (val >> 8);
      chkCalc += (uint8_t)  val;
      addrOff = val << 16;
    } // type==4
    
    else {
      fprintf(stderr, "\n\nerror in 'convert_hex()': line %d has unsupported type %d, exit!\n\n", linecount, type);
      Exit(1);
    }
    
    // checksum
    sprintf(tmp,"0x00");
    strncpy(tmp+2, line+idx, 2);
    sscanf(tmp, "%x", &val);
    chkRead = val;
    
    // assert checksum (0xFF xor (sum over all except record type)
    chkCalc = 255 - chkCalc + 1;                 // calculate 2-complement
    if (chkCalc != chkRead) {
      fprintf(stderr, "\n\nerror in 'convert_hex()': line %d has wrong checksum (0x%02x vs. 0x%02x), exit!\n\n", linecount, chkRead, chkCalc);
      Exit(1);
    }
    
    // store min/max address
    if (addr < addrMin)
      addrMin = addr;
    if (addr+len-1 > addrMax)
      addrMax = addr+len-1;
    
  } // while !EOF
    
  // store base address and image size
  *addrStart = addrMin;
  if ((addrMin != 0xFFFFFFFF) || (addrMax != 0x00000000))
    *numBytes  = addrMax-addrMin+1;
  else
    *numBytes  = 0;
       
  
  // 2nd run: store data to image
  addrOff = 0x00000000;
  if (*numBytes != 0) {
    p = buf;
    while (get_line(&p, line)) {
    
      // record length (address + data + checksum)
      sprintf(tmp,"0x00");
      strncpy(tmp+2, line+1, 2);
      sscanf(tmp, "%x", &val);
      len = val;
      
      // 16b address
      addr = 0;
      sprintf(tmp,"0x0000");
      strncpy(tmp+2, line+3, 4);
      sscanf(tmp, "%x", &val);
      addr = val;         // add offset for >64kB addresses

      // record type
      sprintf(tmp,"0x00");
      strncpy(tmp+2, line+7, 2);
      sscanf(tmp, "%x", &val);
      type = val;
      
      // record contains data
      if (type==0) {
        idx = 9;                          // start at index 9
        for (i=0; i<len; i++) {
          sprintf(tmp,"0x00");
          strncpy(tmp+2, line+idx, 2);                        // get next 2 chars as string
          sscanf(tmp, "%x", &val);                            // interpret as hex data
          image[addr+addrOff+i] = (uint16_t) val | 0xFF00;    // store data byte in buffer and set high byte
          idx+=2;                                             // advance 2 chars in line
        }
      } // type==0

      // EOF indicator
      else if (type==1)
        continue; 
    
      // extended address (=upper 16b of address for following data records)
      else if (type==4) {
        sprintf(tmp,"0x0000");
        strncpy(tmp+2, line+9, 4);        // get next 4 chars as string
        sscanf(tmp, "%x", &val);        // interpret as hex data
        addrOff = val << 16;
      } // type==4
    
      else {
        fprintf(stderr, "\n\nerror in 'convert_hex()': line %d has unsupported type %d, exit!\n\n", linecount, type);
        Exit(1);
      }
    
    } // while !EOF
    
  } // if numBytes!=0
  
  /*
  printf("\n");
  for (i=0; i<(*numBytes); i++)
    printf("%3d   0x%04x   0x%02x\n", i, addrMin+i, (int) (image[addrMin+i]) & 0xFF);
  printf("\n");
  Exit(1);
  */
  
  // print message
  if (verbose) {
    if ((*numBytes)>2048)
      printf("done (%1.1fkB @ 0x%04x)\n", (float) (*numBytes)/1024.0, addrMin);
    else if ((*numBytes)>0)
      printf("done (%dB @ 0x%04x)\n", *numBytes, addrMin);
    else
      printf("done, no data read\n");
    fflush(stdout);
  }
  
} // convert_hex

  

/**
   \fn void convert_txt(char *buf, uint32_t *addrStart, uint32_t *numBytes, uint16_t *image, uint8_t verbose)
   
   \brief convert plain text table (hex addr / data) in memory buffer to memory image
   
   \param[in]  buf        memory buffer to read from (0-terminated)
   \param[out] addrStart  start address of image (=lowest address in hexfile)
   \param[out] numBytes   number of bytes in image
   \param[out] image      RAM image of hexfile (high byte != 0 indicates value is set)
   \param[in]  verbose    level of verbosity and if application exits on fail 
   
   convert memory buffer containing plain table (hex address / value) to memory buffer
*/
void convert_txt(char *buf, uint32_t *addrStart, uint32_t *numBytes, uint16_t *image, uint8_t verbose) {
  
  char      line[1000], *p;
  int       linecount, val;
  uint32_t  addr, addrMin, addrMax;
  
  // print message (if present, strip path)
  if (verbose) {
    printf("  convert table ... ");
    fflush(stdout);
  }
  
  // read data and store in RAM image
  linecount = 0;
  addrMin = 0xFFFFFFFF;
  addrMax = 0x00000000;
  p = buf;
  while (get_line(&p, line)) {
  
    // increase line counter
    linecount++;
    
    // read address & data
    sscanf(line, "%x %d", &addr, &val);

    // store data byte in buffer and set high byte
    image[addr] = (uint16_t) val | 0xFF00;    

    // store min/max address
    if (addr < addrMin)
      addrMin = addr;
    if (addr > addrMax)
      addrMax = addr;
    
  } // while !EOF
      
  // store base address and image size
  *addrStart = addrMin;
  if ((addrMin != 0xFFFFFFFF) || (addrMax != 0x00000000))
    *numBytes  = addrMax-addrMin+1;
  else
    *numBytes  = 0;
       
  
  /*
  printf("\n");
  for (i=0; i<(*numBytes); i++)
    printf("%3d   0x%04x   0x%02x\n", i, addrMin+i, (int) (image[addrMin+i]) & 0xFF);
  printf("\n");
  Exit(1);
  */

  // print message
  if (verbose) {
    if ((*numBytes)>2048)
      printf("done (%1.1fkB @ 0x%04x)\n", (float) (*numBytes)/1024.0, addrMin);
    else if ((*numBytes)>0)
      printf("done (%dB @ 0x%04x)\n", *numBytes, addrMin);
    else
      printf("done, no data read\n");
    fflush(stdout);
  }
  
} // convert_txt

  

/**
   \fn void export_s19(char *outfile, uint16_t *image, uint32_t addrStart, uint32_t addrStop)
   
   \brief export RAM image to file in Motorola s19 format
   
   \param[in]  outfile    filename to output to
   \param[in]  image      RAM image of all imported hexfiles
   \param[in]  addrStart  first address to export
   \param[in]  addrStop   last address to export
   
   export RAM image to file in s19 hexfile format. For description of 
   Motorola S19 file format see http://en.wikipedia.org/wiki/SREC_(file_format)
*/
void export_s19(char *outfile, uint16_t *image, uint32_t addrStart, uint32_t addrStop) {

  int       addr, i;              // generic variable
  uint8_t   data;
  uint32_t  chk;                  // checksum
  int       idxFirst, idxLast;    // first and last data in line
  int       numData;              // number of bytes in line
  FILE      *fp;                  // file pointer

  /*
  printf("\n");
  for (i=addrStart; i<=addrStop; i++)
    printf("  0x%04X   0x%02X\n", i, (int)(image[i]) & 0xFF);
  */

  // open output file
  fp=fopen(outfile,"w");
  if (!fp) {
    fprintf(stderr, "\n\nerror in 'export_s19()': cannot create file '%s', exit!\n\n", outfile);
    Exit(1);
  }

  // store in lines of max. 32B
  for (addr=addrStart; addr<=addrStop; addr+=32) {

    // get last entry in 32B block with data (high byte != 0 indicates data)
    idxFirst = idxLast = -1;
    for (i=0; i<32; i++) {
      if ((image[addr+i] & 0xFF00)!= 0x00) {
        if (idxFirst == -1)
          idxFirst = i;
        idxLast = i;
      }
    }

    // get number of bytes in line
    if (idxFirst != -1)
      numData = (idxLast-idxFirst) + 1;
    else
      numData = 0;
      
    // if line contains data save it to file
    if (numData > 0) {

      // save line (see http://en.wikipedia.org/wiki/SREC_(file_format) )
      fprintf(fp, "S1%02X%02X", numData+3, addr+idxFirst); // 2B addr + data + 1B chk
      chk = (uint8_t) (numData+3) + (uint8_t) (addr+idxFirst) + (uint8_t) ((addr+idxFirst) >> 8);
      for (i=idxFirst; i<=idxLast; i++) {
        data = (uint8_t) (image[addr+i] & 0xFF);    // skip high byte
        chk += data;
        fprintf(fp, "%02X", data);
      }
      chk = ((chk & 0xFF) ^ 0xFF);
      fprintf(fp, "%02X\n", chk);
    }

  } // loop over addresses

  // attach generic EOF line
  fprintf(fp, "S903FFFFFE\n");
  
  // close output file
  fflush(fp);
  fclose(fp);

} // export_s19

  

/**
   \fn void export_hex(char *outfile, uint16_t *image, uint32_t addrStart, uint32_t addrStop)
   
   \brief export RAM image to file in Intel hex format
   
   \param[in]  outfile    filename to output to
   \param[in]  image      RAM image of all imported hexfiles
   \param[in]  addrStart  first address to export
   \param[in]  addrStop   last address to export
   
   export RAM image to file in intel hexfile format. For description of 
   Intel hex file format see http://en.wikipedia.org/wiki/Intel_HEX
*/
void export_hex(char *outfile, uint16_t *image, uint32_t addrStart, uint32_t addrStop) {

  // to be done
  fprintf(stderr, "\n\nerror in 'export_hex()': Intel Hex output not yet supported, exit!\n\n");
  Exit(1);

} // export_hex

  

/**
   \fn void export_txt(char *outfile, uint16_t *image, uint32_t addrStart, uint32_t addrStop)
   
   \brief export RAM image to file with plain text table (hex addr / data)
   
   \param[in]  outfile    filename to output to
   \param[in]  image      RAM image of all imported hexfiles
   \param[in]  addrStart  first address to export
   \param[in]  addrStop   last address to export
   
   export RAM image to file with plain text table (hex addr / data)
*/
void export_txt(char *outfile, uint16_t *image, uint32_t addrStart, uint32_t addrStop) {

  int       addr;              // address
  FILE      *fp;                  // file pointer

  // open output file
  fp=fopen(outfile,"w");
  if (!fp) {
    fprintf(stderr, "\n\nerror in 'export_s19()': cannot create file '%s', exit!\n\n", outfile);
    Exit(1);
  }

  // store each value in own line
  for (addr=addrStart; addr<=addrStop; addr++) {

    if ((image[addr] & 0xFF00)!= 0x00)
      fprintf(fp, "0x%04x	%d\n", addr, (int) (image[addr] & 0xFF));

  } // loop over addresses
  
  // close output file
  fflush(fp);
  fclose(fp);

} // export_txt



/**
   \fn void save_memory_image(char *(char *outfile, uint8_t *image, uint32_t addrStart, uint32_t addrStop);, uint8_t *image, uint32_t addrStart, uint32_t addrStop);

   \brief save memory image (for debug)

   \param[in] outfile   of file to dump image to
   \param[in]  filename name of file to dump image to
   \param[in]  filename name of file to dump image to

   save memory image to file for debugging
*/
void save_memory_image(char *outfile, uint16_t *image, uint32_t addrStart, uint32_t addrStop) {

  FILE      *fp;
  uint32_t  i, j;

  // open file to write
  if (!(fp = fopen(outfile, "w"))) {
    printf("error in save_memory_image(): cannot create file '%s', exit!\n\n", outfile);
    Exit(1);
  }

  // loop over image
  for (i=addrStart; i<=addrStop; i+=32) {
    fprintf(fp, "0x%08x: ", (int) i);
    for (j=0; j<32; j++)
      fprintf(fp, "0x%02x ", (uint8_t) (image[i+j] & 0xFF));
    fprintf(fp, "\n");
  } // flash
  fprintf(fp, "\n\n");

  // close file
  fflush(fp);
  fclose(fp);

} // save_memory_image


// end of file
