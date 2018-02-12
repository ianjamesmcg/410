
/* Lab Exercise #3 -- Signals */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handler( int signum )
{
  printf( "\nProcessing a SIGINT signal -- exiting.\n" );
  exit( 0 );
}

int main( int argc, char* argv[] )
{
  /* Define "handler" as the function to process SIGINT signals */

  signal( SIGINT, handler );

  printf( "\nBeginning execution -- touch CTRL-C to exit the program.\n" );

  /* Loop forever */

  while (1)
  {
    /* loop indefinitely -- sleep for 1 second */
    sleep( 1 );
  }

  return 0;
}

