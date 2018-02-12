
/******************************************************************************
  Demonstrate the use of "fork"
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int A = 47;

int main()
{
  int B = 23;

  pid_t pid;

  printf( "\nPID: %4d  A: %2d  B: %2d\n", getpid(), A, B );

  pid = fork();

  printf( "\nPID: %4d  A: %2d  B: %2d\n", getpid(), A, B );

  if (pid < 0)
  {
    printf( "\n*** Error: fork failed ***\n" );
    exit( 1 );
  }
  else if (pid == 0)
  {
    A = A + 10;
    B = B + 10;
  }
  else
  {
    sleep(2);
    A = A + 25;
    B = B + 25;
  }

  printf( "\nPID: %4d  A: %2d  B: %2d\n", getpid(), A, B );

  exit( 0 );
}

