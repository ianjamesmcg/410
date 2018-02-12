/****************************************************************
* Ian McGregor                                                  *
* CSE410 FS17                                                   *
* Computer Project #7                                           *
****************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>
#include <queue>

using namespace std;

/****************************************************************
*

*
****************************************************************/

class pageTable
{
  public:
    pageTable( int f, string ra )
    {
      allNum = f;

      replacementAlg = ra;

      for( int i=0; i < 16; i++ )
      {
        pages[i] = new page( i );
      }

      for( int j=0; j < f; j++ )
      {
        frames.push_back( 0x20 + j );
      }
      fin = 0;
    }
   
    void Display()
    {
      printf( "\nPage# | V | R | M | Frame#\n" );
      printf( "----------------------------\n" );
      for( int i=0; i < 16; i++ )
      {
        printf( "0x%-6x%-4i%-4i%-4i0x%02x\n", pages[i]->index,\
                                  pages[i]->v,\
                                  pages[i]->r,\
                                  pages[i]->m,\
                                  pages[i]->frameNum );
      }
      printf( "\n" );
    } 

    int Reference( int pgNum, char op, int &pfFlag, int &wbFlag )
    {
      if ( !(pages[pgNum]->isValid()) )
      {
        pages[pgNum]->v = 1;
        pfFlag = 1;
        
        if (allNum == 0)
        {
          if(replacementAlg == "Clock")
          {
            ClockVic( pgNum );
          }
          else
          {
            pages[pgNum]->frameNum = pages[victim]->frameNum;
          }
          wbFlag = 1;
          pages[pgNum]->m = 0;
        }
        else
        {
          pages[pgNum]->frameNum = frames[fin];
          allNum -= 1;
        }
       
        getFin( pgNum );
      }
      else
      { 
        if (replacementAlg == "LRU"){ LRUReplace( pgNum ); }
      }
      pages[pgNum]->r = 1;
      if (op == 'W'){pages[pgNum]->m = 1;} 

      return pages[pgNum]->frameNum;
    }
    
    //
    //
    //

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

        int isValid()
        {
          if (v == 1){ return 1; }
          else{ return 0; }
        }

        void Clear()
        {
          v = 0;
          r = 0;
          m = 0;
          frameNum = 0;  
        }
    };

    //
    //
    //

  private:
    int allNum;
    string replacementAlg;
    page *pages[16];
    vector<int> frames;
    int fin; 
    int victim;

    queue<int> FIFO; //push back when page added, pop front when removed
    int LRU[16]; 
    int Clock[16];


    //
    // For when referenced page is already loaded in LRU
    //
 
    void LRUReplace( int pgNum )
    {
      int found = 0;
      for( int i=0; i < fin; i++ )
      {
        if (!found){
          if (LRU[i] == pgNum)
          {
            found = 1;
          }
        }
        else
        {
          LRU[i-1] = LRU[i];
        }  
      }
      LRU[fin-1] = pgNum;
    }
   
    //
    //
    //

    void ClockVic( int pgNum )
    {
      int j  = 0;
      while (true)
      {
        if (pages[Clock[j%fin]]->v){ pages[Clock[j%fin]]->v = 0; }
        else{ victim = Clock[j%fin]; break; }
        j += 1;
      }

      pages[pgNum]->frameNum = pages[victim]->frameNum;
      pages[victim]->Clear();

      int found = 0;
      for( int i=0; i < fin; i++ )
      {
        if (!found)
        {
          if (Clock[i] == victim)
            {
              found = 1;
            }
        }
        else
        {
          Clock[i-1] = Clock[i];
        }  
      }
      Clock[fin-1] = pgNum;
    }

    //                             
    //
    //

    void getFin( int pgNum )
    {
      if (allNum == 0)
      {
        if (replacementAlg == "FIFO")
        {
          if (fin+1 == frames.size())
          {
            fin += 1; 
            victim = FIFO.front();
            FIFO.push( pgNum );
          }
          else
          {
            pages[victim]->Clear();       //clear old victim
            FIFO.pop();                   //remove it from queue
            
            FIFO.push( pgNum );           //add new page to queue
            victim = FIFO.front(); //assign next victim
          }
        }
        else if (replacementAlg == "LRU")
        {
          if (fin+1 == frames.size())
          {
            LRU[fin] = pgNum;
            fin += 1;
            victim = LRU[0];
          }
          else{
            pages[victim]->Clear();
            for ( int i=1; i<fin; i++ )
            {
              LRU[i-1] = LRU[i];
            }
            LRU[fin-1] = pgNum;
            victim = LRU[0];
          }
        }
        else
        {
          if (fin+1 == frames.size())
          {
            Clock[fin] = pgNum;
            fin += 1;
          } 
        }
      }
      else
      {
        FIFO.push( pgNum );
        LRU[fin] = pgNum;
        Clock[fin] = pgNum;
        fin += 1;
      }
    }
};

int processArgs( int argc, char *argv[], int &n, ifstream &infile );

/****************************************************************
*                                                               *
*                                                               *
*                                                               *
****************************************************************/

int main( int argc, char *argv[] )
{ 
  int n;
  ifstream infile( argv[2] );

  if ( processArgs( argc, argv, n, infile ) ){ printf("\n"); return 1;}
 
  int refCount = 0;
  int rdCount = 0;
  int wCount = 0;

  string line;

  //  Get first two lines of file
  //    -Replacement Algorithm
  //    -Number of Frames to Allocate

  string replaceAlg;
  string framesAllocatedStr;
  int framesInt;
  getline( infile, replaceAlg );
  getline( infile, framesAllocatedStr );

  //replacement agl validity
  if (replaceAlg == "FIFO")
  {
  }
  else if (replaceAlg == "LRU")
  {
  }
  else if (replaceAlg == "Clock")
  {
  }
  else
  {
    printf("Invalid replacement algorithm in input file, defaulting to FIFO.");
    replaceAlg = "FIFO";
  }

  //frames allocated validity
  framesInt = atoi(framesAllocatedStr.c_str());
  if ((framesInt <= 0)||(framesInt > 16))
  {
    printf("Invalid frame allocation in input file, defaulting to 16.");
    framesInt = 16;
  }
 
  // create page table and display initial
  pageTable myPT( framesInt, replaceAlg );
  if(n){ myPT.Display(); }  
  
  //
  //  Main line read loop
  //
  
  string addressStr;
  int address;
  int pageNum;
  int offset;
  char op;
  int pfFlag;
  int wbFlag;
  int physical;

  while ( getline( infile,line ) )
  {
    //reset flags
    pfFlag = 0;
    wbFlag = 0;

    //  Parse line
    addressStr.assign( line, 0, 4 ); 
    address = strtol( addressStr.c_str(), NULL, 16 );
    pageNum = ( address >> 12 ); 
    offset = ( address & 0x0FFF );
    op = line[5];
 
    // Track reference
    refCount += 1;
    if ( op == 'R' ){ rdCount += 1; }
    else{ wCount += 1; }

    // execute 
    physical = myPT.Reference( pageNum, op, pfFlag, wbFlag );

    //  Display reference info
    printf( "0x%04x 0x%01x 0x%03x %c ", address, pageNum, offset, op ); 
    
    if ( pfFlag ){ printf("F "); }
    else{ printf("_ ") ;}

    if ( wbFlag ){ printf("WB "); }
    else{ printf("__ "); }
    printf("0x%02x%03x\n", physical, offset);
    

    if ( n && !( refCount%n ) ){ myPT.Display(); }
  }

  //
  // End program displays
  //

  if (n && refCount%n)
  {
    myPT.Display(); 
  }
  else{ printf("\n"); }

  printf( "Page Replacement Algorithm: %s\n", replaceAlg.c_str() );
  printf( "Number of Frames Allocated: %i\n", framesInt );
  printf( "total number of memory references: %i\n", refCount );
  printf( "total number of read operations:   %i\n", rdCount );
  printf( "total number of write operations:  %i\n\n", wCount );

  infile.close();
  return 0;
}

/****************************************************************
*                                                               *
*                                                               *
*                                                               *
****************************************************************/

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
      printf( "\nPage table will not be displayed\n\n" );
      n = 0;
    }
    else
    {
      //file is open
    }
  }

  return stat;
}
