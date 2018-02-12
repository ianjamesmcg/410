/*********************
  Ian McGregor
  CSE410 FS17
  Computer Project #5
**********************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

int in = 0;
int out = 0;
int pcount = 0;
int ccount = 0;
int yield;
#define K 20

struct brec
{
  long tid;
  int seq;

  brec(): tid(-1)
  {
  }
} buff[K];

sem_t E;
sem_t N;
sem_t S;

void* prod( void* arg );
void* consume( void* arg );
void* doyield( void* arg );
void processArgs( int &p, int &c, int argc, char *argv[] );

/*********************************************/
int main( int argc, char *argv[] )
{
  cout << endl;

  int p, c;
  processArgs( p, c, argc, argv );

  sem_init( &S, 0, 1 );
  sem_init( &E, 0, K );
  sem_init( &N, 0, 0 );

  pcount = (p*100);
  ccount = c;

  pthread_t prodthread[p];
  pthread_t conthread[c];
  pthread_t randthread;
  
  pthread_create( &randthread, NULL, doyield, NULL);

  for( int i=0; i<p; i++)
  {
    if (pthread_create( &prodthread[i], NULL, prod, NULL))
    {
      printf( "*** Error creating producer thread ***\n" );
      exit( -1 );
    }
  }
 
  for( int i=0; i<c; i++)
  {
    if (pthread_create( &conthread[i], NULL, consume, NULL))
    {
      printf( "*** Error creating consumer thread ***\n" );
      exit( -1 );
    }
  }

  for (int i=0; i<p; i++)
    {
      if (pthread_join( prodthread[i], NULL ))
        {
          printf( "*** Error joining thread ***\n" );
          exit( -2 );
        }
  }

  for (int i=0; i<c; i++)
    {
      if (pthread_join( conthread[i], NULL ))
        {
          printf( "*** Error joining consumer thread ***\n" );
          exit( -2 );
        }
  }

  pthread_join( randthread, NULL ); 

  cout << endl << "... execution complete, exiting ..." << endl << endl;
  return 0;
}
/*********************************************/

void* doyield( void* arg )
{
  while (pcount)
  {
    yield = rand() % 8; //random value 0-7
  }
  pthread_exit( NULL );
}

void* prod( void* arg )
{
  for ( int i = 0; i < 100; i++)
  {
    if( yield < 2 ){ sched_yield(); }

    sem_wait( &E );
    sem_wait( &S );
    /*in critical section*/
    if( yield < 1 ){ sched_yield(); }

    buff[in].tid = pthread_self();
    buff[in].seq = i;
    in = ((in+1) % (K));
    /*out critical section*/
    sem_post( &S );
    sem_post( &N );
  }
 
  pthread_exit( NULL );
}

void* consume( void* arg )
{
  while ( true )
  {  
    if( yield < 2 ){ sched_yield(); }
    
    sem_wait( &N );  
    sem_wait( &S );
    /*in critical section*/
    if( yield < 1 ){ sched_yield(); }
    
    cout << "TID: " << buff[out].tid;
    cout << "   seq: " << buff[out].seq << endl;
    out = ((out+1) % (K));
  
    pcount -= 1; 
    /*out critical section*/
    if ( pcount < ccount )
    {
      sem_post( &S );
      sem_post( &E );
      pthread_exit( NULL ); 
    }
    else
    {
      sem_post( &S );
      sem_post( &E ); 
    }
  }
}

void processArgs( int &p, int &c, int argc, char *argv[] )
{
  if (( argc > 3 )||( argc==1 ))
  {
    printf( "Invalid usage: " );
    printf( "%s takes two integer arguments in range 1-10\n", argv[0] ); 
    printf( "Executing program default: 1 producer and 1 consumer\n" );
    
    p = 1;
    c = 1;
  }
  else if ( argc == 2 )
  {
    printf( "Invalid usage: " );

    p = atoi( argv[1] );
  
    if (( !p )||( p>10))
    {
      printf( "%s takes two integer arguments in range 1-10\n", argv[0] );
      printf( "Executing program default: 1 producer and 1 consumer\n" );
    
      p = 1;
      c = 1;
    } 
    else
    {
      printf( "%s takes two integer arguments in range 1-10\n", argv[0] );
      printf( "Executing with %s producer(s), and program default: 1 consumer\n", argv[1] );

      c = 1;
    }
  }
  else
  {
    p = atoi( argv[1] );
    c = atoi( argv[2] ); 
   
    if ( (!p) && (!c) )
    {  
      printf( "%s takes two integer arguments in range 1-10\n", argv[0] );
      printf( "Executing program default: 1 producer and 1 consumer\n" );
    
      p = 1;
      c = 1;
    }
    else if ( ( p>10 )&&( c>10 )  )
    { 
      printf( "%s takes two integer arguments in range 1-10\n", argv[0] );
      printf( "Executing program default: 1 producer and 1 consumer\n" );
    
      p = 1;
      c = 1;
    }
    else if (( !p )||( p>10))
    {
      printf( "%s takes two integer arguments in range 1-10\n", argv[0] );
      printf( "Executing with %s consumers(s), and program default: 1 producer\n", argv[2]);
    
      p = 1;
    } 
    else if (( !c )||( c>10))
    {
      printf( "%s takes two integer arguments in range 1-10\n", argv[0] );
      printf( "Executing with %s producer(s), and program default: 1 consumer\n", argv[1]); 
      
      c = 1;
    } 
    else 
    { 
      //do nothing 
    }
  }
  cout << "..." << endl;
  sleep(1);
  cout << ".." << endl;
  sleep(1);
  cout << "." << endl;
  sleep(1);
  cout << endl;

}
