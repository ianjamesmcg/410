#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <string.h>
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
    cout << "Normalized turnaround time: " << ((d-arriveTime)/timeSpentRunning) << endl;
    cout << endl;
  }

  void setState(int s, int t)
  {
    request = 0;
    state = s;
    timeIn = t;
  }

  private:
    int pid;
    int priority;
    int numBursts;
    int trackBursts;
    int burstTime;
    int blockTime;
    int arriveTime;

    int timeIn;
    int request; //1 for halt, 2 for block;
    int state; //1new 2ready 3running 4 blocked 5 exit

    int timeSpentReady;
    int timeSpentRunning;
    int timeSpentBlocked;
};

class Simulation
{
  public:
    Simulation()
    {
      timer = 0;
      runclock = 0;

      running = NULL;
      for(int i = 0; i < (sizeof(blocked)/sizeof(blocked[0])); i++)
      {
        blocked[i] = NULL;
        ready[i] = NULL;
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
        pList[0]->Check(timer);
        if(running != NULL)
        {
          if(running->request == 1)
          {
            running->setState(5, timer);
            running = NULL; 
          }
          else if (running->request == 2)
          {
            running->setState(4, timer);
            blocked[0] = running;
            running = NULL;
          }
        }

        if(blocked[0] != NULL)
        {
          if(blocked[0]->state != 4)
          {
            blocked[0]->setState(2, timer);
            ready[0] = blocked[0];
            blocked[0] = NULL;
          }
        } 
         
        if(pList[0]->arriveTime == timer)
        {
          pList[0]->setState(2, timer);
          ready[0] = pList[0];
        }

        if((running == NULL)&&(ready[0] != NULL))
        {
          ready[0]->setState(3, timer);
          running = ready[0];
          ready[0] = NULL;
        }  

        timer++;
        
        if(nDisplay && timer && !(timer % (nDisplay)))
        {
         cout << endl << "Processes in ready state: ";
         if (ready[0] != NULL){cout << ready[0]->pid;} 
         cout << endl; 
         cout << "Processes in running state: ";
         if (running != NULL){cout << running->pid;} 
         cout << endl; 
         cout << "Processes in blocked state: ";
         if (blocked[0] != NULL ){cout << blocked[0]->pid;}
         cout<< endl << endl; 
        }

      }
    }

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

    void new_process(int p, int pr,int nB, int brt, int blt, int art)
    {
      pList[0] = new Process(p, pr, nB, brt, blt, art);
    }


  private:
    int timer;
    int nDisplay;
    int simLength;
    int cpuNum;
    string schAlg;

    int runclock;

    //FOR PROJECT #9
    //Implement Dynamic Allocation
    Process *running;
    Process *ready[8];
    Process *blocked[8];
    Process *newPr[8];
    Process *pList[8];
};

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

  if (!inf.is_open())
  {
    //check if input file can be opened
    cerr << "Error: connot open input file \"" << argv[2] << "\"." << endl;
    return 0;
  }
  else
  {
    string line;
    
    if (getline(inf, line)) //first line simulation length
    {
      mysim.set_simLength(isPositiveNum(line.c_str()));
      if (!mysim.get_simLength()) 
      {
        cerr << "Error: 1st line of input file must be positive int." << endl;
        return 0;
      }
    }
    else{ cerr << "Error: empty input file" << endl; return 0; }

    if (getline(inf, line)) //second line number of CPU's
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
   
    if (getline(inf, line)) //third line scheduling algorithm
    {
      if (line == "FCFS")
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

    //initial display
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

    if (getline(inf, line))
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

      //check line and add simulation to list
      mysim.new_process(atoi(simArgs[0]),
                        atoi(simArgs[1]),
                        atoi(simArgs[2]),
                        atoi(simArgs[3]),
                        atoi(simArgs[4]),
                        atoi(simArgs[5]));
    }
    else{ cerr << "Error: invalid input file" << endl; return 0; }
  }

  return 1;
}

int isPositiveNum(const char* txtIn)
{
  int returnNum;
  returnNum = atoi(txtIn);
  return returnNum;
}
