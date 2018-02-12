
/******************************************************************************
  Demonstrate the use of "exit" and "wait"
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void display_status( int );

int main()
{
  pid_t pid;
  int status;


  /***** Create child process #1 *****/

  pid = fork();

  if (pid < 0)
  {
    perror( "fork error" );
  }
  else if (pid == 0)
  {
    /* Child process -- terminate normally */

    exit( 1 );
  }
  else
  {
    /* Parent process -- wait for child */

    if (wait( &status ) != pid)
    {
      perror( "wait error" );
    }
    display_status( status );
  }


  /***** Create child process #2 *****/
 
  pid = fork(); 
 
  if (pid < 0)
  { 
    perror( "fork error" );
  } 
  else if (pid == 0)
  { 
    /* Child process -- generate SIGABRT */
 
    abort();

    exit( 2 );
  } 
  else
  {
    /* Parent process -- wait for child */

    if (wait( &status ) != pid)
    {
      perror( "wait error" );
    }
    display_status( status );
  }
 

  /***** Create child process #3 *****/
 
  pid = fork(); 
 
  if (pid < 0)
  { 
    perror( "fork error" );
  } 
  else if (pid == 0)
  { 
    /* Child process -- divide by zero to generate SIGFPE */
 
    int top = 20;
    int bot = 0;
    int num = top / bot;

    num++;

    exit( 3 );
  }    
  else
  {
    if (wait( &status ) != pid)
    { 
      perror( "wait error" );
    } 
    display_status( status );
  }
 

  /***** Create child process #4 *****/

  pid = fork();

  if (pid < 0)
  {
    perror( "fork error" );
  }
  else if (pid ==0)
  {
    /* Child process -- segfault */

    raise(SIGSEGV);
    
    exit( 4 );

  }
  else
  {
    if (wait( &status ) != pid)
    {
      perror( "wait error" );
    }
    display_status( status );
  }

  exit( 0 );
}

void display_status( int stat )
{
  if (WIFEXITED(stat))
  {
    printf( "\nNormal termination, exit status %d\n", WEXITSTATUS(stat) );
  }
  else if (WIFSIGNALED(stat))
  {
    printf( "\nAbnoral termination, signal number %d\n", WTERMSIG(stat) );
  }
  else
  {
    printf( "\nSome other problem detected\n" );
  }
}

