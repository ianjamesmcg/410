/************************
  Ian McGregor
  CSE410 FS17
  Computer Project #6
************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>

using namespace std;

class pageTable
{
  public:
    pageTable( int s )
    {
      size = s;

      for( int i=0; i < s; i++ )
      {
        frames[i] = new page( i );
      }
    }
   
    void Display()
    {
      printf( "\nPage# | V | R | M | Frame#\n" );
      printf( "----------------------------\n" );
      for( int i=0; i < size; i++ )
      {
        printf( "0x%-6x%-4i%-4i%-4i0x%02x\n", frames[i]->index,\
                                  frames[i]->v,\
                                  frames[i]->r,\
                                  frames[i]->m,\
                                  frames[i]->frameNum );
      }
      printf( "\n" );
    } 

    class page
    {
      friend class pageTable;
      public:
        page( int i )
        {
          index = i;
          v = 0;
          r = 0;
          m = 0;
          frameNum = 0;  
        }

      private:
        int index;
        int v;
        int r;
        int m;
        int frameNum;
    };

  private:
    int size;
    page *frames[16]; 
};


int processArgs( int argc, char *argv[], int &n, ifstream &infile );

int main( int argc, char *argv[] )
{ 
  int n;
  ifstream infile( argv[2] );

  if ( processArgs( argc, argv, n, infile ) ){ printf("\n"); return 1;}
 
  pageTable myPT( 16 );
  myPT.Display();
  
  int refCount = 0;
  int rdCount = 0;
  int wCount = 0;

  string line;
  string addressStr;
  int address;
  int pageNum;
  int offset;
  char op;

  while ( getline( infile,line ) )
  {
    addressStr.assign( line, 0, 4 ); 
    address = strtol( addressStr.c_str(), NULL, 16 );
    pageNum = ( address >> 12 ); 
    offset = ( address & 0x0FFF );
    op = line[5];
 
    refCount += 1;
    if ( op == 'R' ){ rdCount += 1; }
    else{ wCount += 1; }

    printf( "%04x %01x %03x %c\n", address, pageNum, offset, op ); 

    if ( n && !( refCount%n ) ){ myPT.Display(); }
  }

  if ( (n && refCount%n) || !n )
  {
    myPT.Display(); 
  }

  printf( "total number of memory references: %i\n", refCount );
  printf( "total number of read operations:   %i\n", rdCount );
  printf( "total number of write operations:  %i\n\n", wCount );

  infile.close();
  return 0;
}

/******************************************************************************/

int processArgs( int argc, char *argv[], int &n, ifstream &infile )
{
  int stat = 0;

	if ( argc != 3 )
	{
		printf( "\nInvalid arguments: wrong #\n" );
		printf( "Usage: %s N(a positive integer) input_file\n", argv[0] );
	  stat = 1;
  }
  else if ( !infile.is_open() )
  {
    printf( "\nUnable to open file\n" );
    stat=1;
  }
  else
  {
    n = atoi( argv[1] );
    if ( n < 1 )
    {  
      printf( "\nInvalid arguments: second token is a positive int\n" );
		  printf( "Usage: %s N(a positive integer) input_file\n", argv[0] );
      printf( "\nPage table will not be displayed between mem references\n" );
      n = 0;
    }
    else
    {
      //file is open
    }
  }

  return stat;
}
