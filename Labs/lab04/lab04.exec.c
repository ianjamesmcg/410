
/******************************************************************************
  Demonstrate the use of "execle" and "execlp"
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

const char* env_init[] = { "USER=unknown", "PATH=/tmp", NULL };

int main()
{
  pid_t pid;
  int flag, status;


  /***** Create child process #1 *****/

  pid = fork();

  if (pid < 0)
  {
    perror( "fork error" );
  }
  else if (pid == 0)
  { 
    /* Child process -- replace with different program */

    flag = execle( "/bin/ls",
      "ls", "-lF", NULL, env_init );

    if (flag < 0)
    {
      perror( "execle error" );
    }
  }
  else
  {
    /* Parent process -- wait for child to terminate */

    if (wait( &status ) != pid)
    {
      perror( "wait error" );
    }
  }


  /***** Create child process #2 *****/

  pid = fork();

  if (pid < 0)
  {
    perror( "fork error" );
  }
  else if (pid == 0)
  { 
    /* Child process -- replace with different program */
  
    char *args[4];
    args[0] = (char*)"lab04.script";
    args[1] = (char*)"lab03";
    args[2] = (char*)"lab04";
    args[3] = NULL;

    flag = execvp( args[0], args );

    if (flag < 0)
    {
      perror( "execlp error" );
    }
  }
  else
  {
    /* Parent process -- wait for child to terminate */

    if (wait( &status ) != pid)
    {
      perror( "wait error" );
    }
  }

  exit( 0 );
}

