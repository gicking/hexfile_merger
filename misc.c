/**
  \file misc.c
   
  \author G. Icking-Konert
  \date 2008-11-02
  \version 0.1
   
  \brief implementation of misc routines
   
  implementation of routines not really fitting anywhere else

*/

/////////////
// headers
/////////////

#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include "misc.h"



/**
  \fn void Exit(uint8_t code)
   
  \brief terminate program after cleaning up (close logfile etc.)
   
  \param[in] code    return code of application to commandline

  Terminate program. Replaces standard Exit() routine which doesn't allow
  for a \<return\> request prior to closing of the console window.
*/
void Exit(uint8_t code) {

  // optionally prompt for <return>
  if (g_pauseOnExit) {
    printf("\n\npress <return> to exit");
    fflush(stdout);
    fflush(stdin);
    getchar();
  }
  
  // terminate application
  exit(code);

} // Exit


// end of file
