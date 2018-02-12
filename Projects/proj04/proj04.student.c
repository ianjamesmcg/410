/**************************
  Ian McGregor
  CSE410
  Computer Project #3
**************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <sys/wait.h>
#include <vector>
#include <ctime>
#include <errno.h>

using namespace std;

int main( int argc, char *argv[], char *env[] )
{
  printf("\n");

  /* intialize prompt variables */
  int seq = 1;
  string input = ""; 
  char *username = getenv( "USER" );  


  /* start repeated shell prompt, ends when command "quit" is entered as first arg */
  while( 1 ){
  
    /***** prompt and get new user input *****/
    input = "";
    printf( "<%i %s>", seq, username );
    getline( cin, input );

    /***** Split input into tokens *****/
    char *inChar = new char[ input.length() + 1 ];
    strcpy( inChar, input.c_str() );
    
    //will save to char array and string vector
    char *outChar[64];
    memset( outChar, 0, sizeof( outChar ) );
    
    vector<string> args;
    args.clear();
    
    char *tokens;
    int x = 0;
    tokens = strtok( inChar, "  " );
    while (tokens != NULL)
    {
      outChar[x] = tokens;
      args.push_back( tokens );
      
      tokens = strtok( NULL, "  " );
      x += 1;
    }
    /***********************************/

    printf( outChar[1] );
    printf( "\n" );

    /***********************************
      Summarize if/else block
      *********************************/

    if (args.size() == 0)
    {
      //do nothing
    }

    else if ( args[0] == "quit" )
    {
      if (args.size() > 1){
        printf("quit takes no arguments\n");
      }
        else{ 
        printf( "exiting shell...\n\n");
        return 0;
      }
    }

    else if( args[0] == "date" )
    {
      if (args.size() > 1){
        printf("date takes no argumentsi\n");
      }
        else{ 
        time_t now = time(0);
        cout << ctime(&now);
      }
    }
  
    else if( args[0] == "env" )
    {
      if (args.size() > 1){
        printf("env takes no arguments");
      } 
      else{
      for( int i=0; env[i]!=NULL; i++ ){
        printf( "%s\n", env[i] );
      }
      }
    
    }
  
    else if( args[0] == "curr" )
    {
      if(args.size() > 1){
        printf("curr takes no arguments\n");
      }
      else{ 
        char directory[256];
        getcwd( directory, sizeof(directory) );
        printf( "%s\n", directory );
      }
    }
  
    else if( args[0] == "cd" )
    {
  
      if( (args.size() ==  1) || args[1] == "~"  )
      {
        int newDir;
        newDir = chdir( getenv( "HOME" ) );

        if( newDir != 0 ){
          printf( "Failed to open directory: %s\n", strerror(errno) );
        }       
      }    
      else if( args.size() == 2 )
      {
        int newDir;
      
        if(( args[1].find("~") == 0 )&&(args[1].find("/") != 1 ) ){
          args[1].erase(0,1);
          string dir = "/user/" + args[1];
          newDir = chdir( dir.c_str() );
        }
        else{
          newDir = chdir( args[1].c_str() );
        }

        if( newDir != 0 ){
          printf( "Failed to open directory: %s\n", strerror(errno) );
        }      
      }
      else
      {
        printf( "cd takes zero or one arguments\n" );
      }
    
    }
  
    else 
    {
      pid_t pid;
      int flag, status;
     
      pid = fork();

      if ( pid < 0 )
      {
        perror( "fork error" );
        flag = -1;
        exit( flag );
      }

      else if( pid == 0 )
      {
        /* Child Process */
        if ( args.back() == "&" ) { outChar[args.size() - 1] = '\0'; }
        flag = execvp( outChar[0], outChar ); 
        //check for file run error
        if ( flag < 0 )
        {
          perror( "error external command" );
        }
      }

      else
      {
        /* Parent Process */
        if ( args.back() != "&" )
        {
          if ( waitpid( pid , &status, 0 ) != pid )
          {
            perror ( "wait error" );
          }
        }
      }

    };

    seq += 1;

  };

  printf("\n");
  exit( 0 );
}
