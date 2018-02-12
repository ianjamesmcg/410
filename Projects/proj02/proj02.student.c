/*******************
Ian McGregor
Computer Project #2
********************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <string>

int main( int argc, char *argv[], char *env[] )
{

  printf("\n");

  if ( argc < 2 ){

    printf( "Need a file to look for\n" );
    printf( "<Usage>proj02 fileName1 fileName2 ... fileNameN\n\n" );
    return 0;
  }
  else{

    int foundOne;

    for( int i=1; argv[i]!=NULL; i++ ){
      
      foundOne = 0;

      char temp[100];
      char *fileName = temp;
      
      strcpy( fileName, "/");
      strcat( fileName, argv[i]);

      /*save the search path to a new string*/


      char savePath[1024];
      strcpy( savePath, getenv( "PATH" ) );

      /*
      printf( "Search path is: %s\n\n", savePath );
      */

      /*parse to individual path directories*/

      char *singlePath;
      singlePath = strtok( savePath, ":" );


      /*****initialize permissions list********/

      const char *perm[8];
      perm[0] = "---";
      perm[1] = "--x";
      perm[2] = "-w-";
      perm[3] = "-wx";
      perm[4] = "r--";
      perm[5] = "r-x";
      perm[6] = "rw-";
      perm[7] = "rwx";

      /****************************************/

      int flag;
      char temp2[100];
      char *filePath = temp2;
      struct stat buffer;

      while (singlePath != NULL ){

        strcpy( filePath, singlePath );
        strcat( filePath, fileName );

        //print all searched locations
        //printf( "Token is: %s\n", filePath );
        
        if ( !( flag = stat( filePath, &buffer) ) ){

          foundOne = 1;

          printf( "Entry '%s' found at: %s\n", argv[i], filePath );
          printf( "File size: %lld bytes\n", (long long) buffer.st_size );
          printf( "Owner ID: %ld\n", (long) buffer.st_uid );
          printf( "Group ID: %ld\n", (long) buffer.st_gid );
     
          //Translate permissions

          printf( "Permissions: ");
          if ((buffer.st_mode & S_ISUID)/512){
            printf("d");
          }
          else{
            printf("-");
          };

          printf( "%s", perm[((buffer.st_mode & S_IRWXU)/64)] );
          printf( "%s", perm[((buffer.st_mode & S_IRWXG)/8)] );
          printf( "%s\n", perm[(buffer.st_mode & S_IRWXO)] );

          printf( "Last file read: %s", ctime(&buffer.st_atime) );      
          printf( "Last file write: %s", ctime(&buffer.st_mtime) );      
          printf( "Last status change: %s", ctime(&buffer.st_ctime) );      

        };
    
        //print flag value and errno for current Path searched
        //printf( "   flag is %i, errno is %i \n", flag, errno );
        
        singlePath = strtok( NULL, ":"  ) ;
      };      

      /*
      for (int i=0; env[i]!=NULL; i++)
      {
        printf( "%s\n", env[i] );
      } 
      */
    
      if (!foundOne){
        printf("No instance of '%s' found in search path\n", argv[i] );
      };

      printf( "\n" );

    };
 
  };

}
