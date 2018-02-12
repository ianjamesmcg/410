
/******************************************************************************
  Lab Exercise #5 -- Part B, Question 1
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void* sub_function( void* arg )
{
  printf( "Executing in thread %ld (PID %d)\n", (long) pthread_self(),
    (int) getpid() );

  sleep( 1 );

  return NULL;
}

int main()
{
  char hostname[256];
  int i;

  gethostname( hostname, 256 );

  printf( "\nSystem name: %s", hostname );
  printf( "  Number of processors: %ld\n\n", sysconf( _SC_NPROCESSORS_ONLN ) );

  printf( "Starting master thread %ld (PID %d)\n", (long) pthread_self(),
    (int) getpid() );

  for (i=0; i<5; i++)
  {
    sub_function( NULL );
  }

  printf( "Halting master thread %ld (PID %d)\n", (long) pthread_self(),
    (int) getpid() );

  exit( 0 );
}

