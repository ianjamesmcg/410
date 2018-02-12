#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <queue>
using namespace std;

class Process
{
  public:
    Process(int p, int pr,int nB, int brt, int blt, int art)
    {
      pid = p;
      priority = pr;
      numBursts = trackBursts = nB;       
      burstTime = brt;
      blockTime = blt;
      arriveTime = art; 

      request = 0;
      state = 0;
      timeSpentReady = 0;
      timeSpentRunning = 0;
      timeSpentBlocked = 0;
    }

  friend class Simulation;

  //run for every process every tick
  //keeps state record 
  //controls state duration and signaling to simulation
  void Check(int t)
  {
    if(state == 2){timeSpentReady++;}
    else if(state == 3)
    {
      timeSpentRunning++;
      if((t - timeIn) == burstTime)
      {
        request = 2;
        trackBursts--;
      }
      if(trackBursts == 0){request = 1; Exit(t);}
    }
    else if(state == 4){
      timeSpentBlocked++;
      if((t - timeIn) == blockTime)
      {
        state = 2;
      }
    }
  }

  //display upon process completion
  //d is timer at departure
  void Exit(int d)
  {
    cout << endl;
    cout << "pid: " << pid << endl;
    cout << "priority: " << priority << endl;
    cout << "# of CPU bursts: " << numBursts << endl;
    cout << "CPU burst duration: " << burstTime << endl;
    cout << "Block duration: " << blockTime << endl;
    cout << "Arrival time: " << arriveTime << endl;
    cout << "Departure time: " << d << endl;
    cout << "Cumulative time in ready state: " << timeSpentReady << endl;
    cout << "Cumulative time in running state: " << timeSpentRunning << endl;
    cout << "Cumulative time in blocked state: " << timeSpentBlocked << endl;
    cout << "Turnaround time: " << (d-arriveTime) << endl;
    
    float norm = (((float)d-(float)arriveTime)/(float)timeSpentRunning); 
    printf("Normalized turnaround time: %.2f", norm);

    cout << endl << endl;
  }

  //change state as directed by the simulation
  //and reset the state control variables
  void setState(int s, int t)
  {
    request = 0;
    state = s;
    timeIn = t;
  }

  private:
    //parameters
    int pid;
    int priority;
    int numBursts; 
    int burstTime; //#ticks foreach burst
    int blockTime; //#ticks foreach block state
    int arriveTime; //#ticks from start of sim the process enters new
    //

    //control
    int trackBursts; //decrements at each burst to decide when the process should exit 
    int timeIn; //time of entry ocurrent state
    int state; //1new 2ready 3running 4 blocked 5 exit
    int request; //1 for halt, 2 for block;
    //
    
    //records
    int timeSpentReady;
    int timeSpentRunning;
    int timeSpentBlocked;
    //
};

class Simulation
{
  public:
    Simulation()
    {
      timer = 0;
      runclock = 0;

      running = NULL;
      for(unsigned i = 0; i < (sizeof(newPr)/sizeof(Process)); i++)
      {
        pList[i] = NULL;
        blocked[i] = NULL;
        ready[i] = NULL;
        ready2[i] = NULL;
        ready3[i] = NULL;
        newPr[i] = NULL;
      }
    }

    Simulation(int n, int sl, int cn, string sa)
    {
      timer = 0;
      nDisplay = n;
      simLength = sl;
      cpuNum = cn;
      schAlg = sa;
    }

    void Run()
    {
      while(timer < simLength)
      {
        //tag processes
        Tag();

        //recognize request from running process
        RunningReq();

        //recognized blocked process that has become unblocked
        Unblock();

        //recognize arrival of new process
        Arrival();        

        //move ready process to running state
        ReadyRun();
        
        timer++;
        
        //display time and process states after every N ticks and at end of simulation
        NDisplay();
      }
    }

    //tag processes
    void Tag()
    {
      for (unsigned i = 0; i < sizeof(pList)/sizeof(Process); i++)
      {
        if (pList[i] != NULL)
        {
          pList[i]->Check(timer);
        }
        else
        {
          break;
        }
      }
    }

    void RunningReq()
    {
      if(running != NULL)
      {
        if(running->request == 1) //halt
        {
          //move to exit
          running->setState(5, timer);
          for (unsigned i = 0; i < sizeof(exit)/sizeof(exit[0]); i++)
          {
            if (exit[i] == NULL)
            {
              exit[i] = running; 
              break;
            }
          }
          running = NULL; 
        }
        else if (running->request == 2) //block
        {
          //move to blocked
          running->setState(4, timer);
          for (unsigned i = 0; i < sizeof(blocked)/sizeof(blocked[0]); i++)
          {
            if (blocked[i] == NULL)
            {
              blocked[i] = running; 
              break;
            }
            //keep 'blocked' sorted for the sake of tie breaking unblock by pid
            else if (running->pid < blocked[i]->pid)
            {
              Process *tmp1 = blocked[i];
              Process *tmp2;
              for (unsigned j = 0; blocked[j] != NULL; j++)
              {
                tmp2 = blocked[j + 1];
                blocked[j + 1] = tmp1;
                tmp1 = tmp2;
              }
              blocked[i] = running;
              break;
            }
          }
          running = NULL;
        }
      }
    }

    //recognized blocked process that has become unblocked
    void Unblock()
    {
      vector<int> send;
      for (unsigned i = 0; i < sizeof(blocked)/sizeof(blocked[0]); i++)
      {
        if(blocked[i] != NULL)
        {
          //process will change it's own state when block time is up
          if(blocked[i]->state != 4)
          {
            //if unblocked move to ready
            blocked[i]->setState(2, timer);
            send.push_back(i);
          }
        }
        else
        {
          break;
        }
      }
      unsigned stop = send.size();
      for (unsigned j = 0; j < stop; j++)
      {
        toReady(blocked, (send[j]) - j); //add to ready
        //each call of toReady will remove the item from newPr
        //so the index of each succesive call needs to decrease
        //by the # of items before it, which is 'j'
      }
    }

    //recognize arrival of a new process
    void Arrival()
    {
      //check for new process that has arrived
      if (newPr[0] != NULL)
      {
        vector<int> send;
        for (unsigned i = 0; newPr[i] != NULL; i++)
        {
          if (newPr[i]->arriveTime == timer)
          {   
            send.push_back(i); //save index
          }
        }
        unsigned stop = send.size();
        for (unsigned j = 0; j < stop; j++)
        {
          toReady(newPr, (send[j]) - j); //add to ready
          //each call of toReady will remove the item from newPr
          //so the index of each succesive call needs to decrease
          //by the # of items before it, which is 'j'
        }
      }
    } 

    //move ready process to running state
    void ReadyRun()
    {
      if(running == NULL)
      {
        if (ready[0] != NULL)
        {
          ready[0]->setState(3, timer);
          running = ready[0];
          removeProcess(ready, 0);
        }
        else if (ready2[0] != NULL)
        {
          ready2[0]->setState(3, timer);
          running = ready2[0];
          removeProcess(ready2, 0);
        }
        else if (ready3[0] != NULL)
        {    
          ready3[0]->setState(3, timer);
          running = ready3[0];
          removeProcess(ready3, 0);
        }
      }  
    }

    //display time and process states after every N ticks and at end of simulation
    void NDisplay()
    {
      if((nDisplay && timer && !(timer % (nDisplay)))||(timer==simLength))
      {
        cout << endl << "current tick: " << timer << endl;
        
        //processes in ready state 
        cout << "Processes in ready state: ";
        for (unsigned i = 0; i < sizeof(ready)/sizeof(ready[0]); i++)
        {
          if (ready[i] != NULL){cout << ready[i]->pid << " ";} 
          else {break;}
        }
        for (unsigned i = 0; i < sizeof(ready)/sizeof(ready[0]); i++)
        {
          if (ready2[i] != NULL){cout << ready2[i]->pid << " ";} 
          else {break;}
        }
        for (unsigned i = 0; i < sizeof(ready)/sizeof(ready[0]); i++)
        {
          if (ready3[i] != NULL){cout << ready3[i]->pid << " ";} 
          else {break;}
        }
        cout << endl; 
        //running process
        cout << "Processes in running state: ";
        if (running != NULL){cout << running->pid;} 
        cout << endl; 
        //in blocked state
        cout << "Processes in blocked state: ";
        for (unsigned i = 0; i < sizeof(blocked)/sizeof(blocked[0]); i++)
        {
          if (blocked[i] != NULL){cout << blocked[i]->pid << " ";} 
          else {break;}
        }
        cout<< endl << endl; 
      }
    }

    //move process at from[i] to ready state
    void toReady(Process *from[], unsigned i)
    {

      from[i]->setState(2, timer);

      if (schAlg == "FCFS")
      {
        //put in first available ready state slot
        moveProcess(from, i, ready);       
      }
      else
      {
        if (from[i]->priority == 1)
        {
          moveProcess(from, i ,ready);
        }
        else if (from[i]->priority == 2)
        {
          moveProcess(from, i, ready2);
        }
        else
        {
          moveProcess(from, i, ready3);
        }
      }
    }
  
    //move Process at origin[i] to first open spot in dest
    void moveProcess(Process *origin[], int o, Process *dest[], int d = -1)
    {
      if (d < 0) //no destination index given, place proces at end of dest
      { 
        int i = 0;
        while (dest[i] != NULL){ i++; } //stop at first open spot
        dest[i] = origin[o];
      } 
      else 
      {
        if (dest[d] == NULL){dest[d] = origin[o];}
        else
        { 
          Process *tmp1 = dest[d];
          Process *tmp2;
          for (unsigned j = d; dest[j] != NULL; j++)
          {
            tmp2 = dest[j+1];
            dest[j+1] = tmp1;
            tmp1 = tmp2;  
          }
          dest[d] = origin[o];
        }
      }
      //remove from origin state
      removeProcess(origin, o);
    }

    //remove process at index i from array state
    void removeProcess(Process *state[], int i)
    {
      //remove from origin state
      int k = i;
      while (state[k] != NULL)
      { 
        state[k] = state[k+1]; 
        k++;
        if (k == 255){state[k] = NULL;}
      }
    }

    //
    //public access to variables
    //
    int set_nDisplay(int in)
    {
      nDisplay = in;
      return in;
    }    
    int set_simLength(int in)
    {
      simLength = in;
      return in;
    }
    int set_cpuNum(int in)
    {
      cpuNum = in;
      return in;
    }
    string set_schAlg(string in)
    {
      schAlg = in;
      return in;
    }
    int get_nDisplay()
    {
      return nDisplay;
    }    
    int get_simLength()
    {
      return simLength;
    }
    int get_cpuNum()
    {
      return cpuNum;
    }
    string get_schAlg()
    {
      return schAlg;
    }
    //
    //
    //

    //create new process and add to process list and new
    void new_process(int p, int pr,int nB, int brt, int blt, int art)
    {
      for(unsigned i = 0; i < sizeof(newPr)/sizeof(Process); i++)
      {
        if (newPr[i] == NULL)
        {
          newPr[i] = pList[i] = new Process(p, pr, nB, brt, blt, art);
          break;
        }
      }
    }
    
  private:
    int timer;
    int nDisplay;
    int simLength;
    int cpuNum;
    string schAlg;

    int runclock;

    Process *pList[256];
    Process *newPr[256];
    Process *ready[256];
    Process *ready2[256];
    Process *ready3[256];
    Process *running;
    Process *blocked[256];
    Process *exit[256];
};

//defined below main
int processArgs(int argc, char* argv[], Simulation &mysim);
int isPositiveNum(const char* txtIn);

//
//
//
int main(int argc, char* argv[])
{
  Simulation mysim;

  if (!processArgs(argc, argv, mysim)){return EXIT_FAILURE;}

  mysim.Run();

  cout << endl;
  return EXIT_SUCCESS;
}
//
//
//

int processArgs(int argc, char* argv[], Simulation &mysim)
{
  if (argc != 3)
  {
    cerr << "Error: " << argv[0] << " accepts 2 arguments." << endl;
    return 0;
  }

  mysim.set_nDisplay(atoi(argv[1]));
  ifstream inf(argv[2]);

  //check if input file can be opened
  if (!inf.is_open())
  {
    cerr << "Error: connot open input file \"" << argv[2] << "\"." << endl;
    return 0;
  }

  string line;

  //first line simulation length
  if (getline(inf, line)) 
  {
    mysim.set_simLength(isPositiveNum(line.c_str()));
    if (!mysim.get_simLength()) 
    {
      cerr << "Error: 1st line of input file must be positive int." << endl;
      return 0;
    }
  }
  else{ cerr << "Error: empty input file" << endl; return 0; }


  //second line number of CPU's
  if (getline(inf, line))
  {
    int cpuNum = mysim.set_cpuNum(isPositiveNum(line.c_str()));
    if (!cpuNum || cpuNum < 1 || cpuNum > 3) 
    {
      cerr << "Error: 2nd line of input file must be positive int(# of CPU's).";
      cerr << endl;
      return 0;
    }
  }
  else{ cerr << "Error: invalid input file" << endl; return 0; }
 

  //third line scheduling algorithm
  if (getline(inf, line))
  {
    if (line == "FCFS" || line == "Priority")
    {
      mysim.set_schAlg(line);
    }
    else
    {
      cerr << "Error: unrecognized scheduling algorithm \"" << line << "\".";
      cerr << endl;
      return 0;
    }
  } 
  else{ cerr << "Error: invalid input file" << endl; return 0; }


  //initial sim parameter display
  printf(
"\n\
------------------ Input ------------------\n\
\n\
N: %s\n\
File: %s\n\
\n\
---------- Simulation Parameters ----------\n\
\n\
Length of Simulation (in ticks):  %i\n\
Number of CPU's in the system:    %i\n\
Short-term scheduling Algorithm:  %s\n\
\n\
-------------------------------------------\n\
\n"
  , argv[1], argv[2]
  , mysim.get_simLength()
  , mysim.get_cpuNum()
  , mysim.get_schAlg().c_str()
  );



  //loop to read processes defintions from input file
  while (getline(inf, line))
  {
    char* cLine = &line[0];
    const char* ch;
    const char* simArgs[6];
    int index = 0;

    ch = strtok(cLine, " ");
    while(ch != NULL)
    {
      simArgs[index] = ch;
      index++;
      ch = strtok(NULL, " ");
    }

    if (!atoi(simArgs[0])){break;}
    //check line and add simulation to list
    mysim.new_process(atoi(simArgs[0]),
                      atoi(simArgs[1]),
                      atoi(simArgs[2]),
                      atoi(simArgs[3]),
                      atoi(simArgs[4]),
                      atoi(simArgs[5]));
  }
  
  return 1;
}

int isPositiveNum(const char* txtIn)
{
  int returnNum;
  returnNum = atoi(txtIn);
  return returnNum;
}
