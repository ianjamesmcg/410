
/******************************************************************************
  Lab Exercise #5 -- Part B, Question 3
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM 200000000

void* sub_function( void* arg )
{
  int n;

  printf( "Executing in thread %ld (PID %d)\n", (long) pthread_self(),
    (int) getpid() );

  for (n=0; n<NUM; n++);

  pthread_exit( NULL );
}

int main()
{
  char hostname[256];
  pthread_t mythread[5];
  int i;

  gethostname( hostname, 256 );

  printf( "\nSystem name: %s", hostname );
  printf( "  Number of processors: %ld\n\n", sysconf( _SC_NPROCESSORS_ONLN ) );

  printf( "Starting master thread %ld (PID %d)\n", (long) pthread_self(),
    (int) getpid() );

  for (i=0; i<5; i++)
  {
    if (pthread_create( &mythread[i], NULL, sub_function, NULL))
    {
      printf( "*** Error creating thread ***\n" );
      exit( -1 );
    }
  }

  for (i=0; i<5; i++)
  {
    if (pthread_join ( mythread[i], NULL ))
    {
      printf( "*** Error joining thread ***\n" );
      exit( -2 );
    }
  }

  printf( "Halting master thread %ld (PID %d)\n", (long) pthread_self(),
    (int) getpid() );

  exit( 0 );
}

