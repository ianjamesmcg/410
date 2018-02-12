#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/*
Changes current directory to the supplied command line argument
afterword displays full pathname of current directory
*/


int main( int argc, char *argv[] )
{
  char directory[1024];
  getcwd( directory, sizeof(directory) );
  printf( "Started in %s\n", directory );

  int newDir;
  newDir = chdir( argv[1] );
  
  if ( newDir == 0 ) {
    printf( "success\n" );
  }
  else {
    int myerror;
    myerror = errno;
    printf( "Failure, errno is %i\n", errno );
    printf ( "Error code: %s\n", strerror(errno) );
  };

  getcwd( directory, sizeof(directory) );
  printf( "After in %s\n", directory );
}
