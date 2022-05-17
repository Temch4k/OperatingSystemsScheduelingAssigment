//Artsiom Skarakhod
//Lab 2
//OS

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>

using namespace std;

// the struct that we use to work with out processes and store all of their data
struct process
{
    int pID = 0;
    int arrTime = 0;
    int burst = 0;
    int prio = 0;

    double timeWhenFinished = 0;
    double waitingTime = 0;
    double turnAroundTime = 0;
    double responseTime = 0;
    double noOfContext = 0;

    int roundRobin = 0;

    double priorityMath = 0;

    bool touched = false;
    bool added = false;
    double firstTimeTouched = 0;


    // prints all of the process contents in a nice organized way
    void printProcess() const
    {
        printf("%d\t", pID);
        printf("%d\t", arrTime);
        printf("%d\t", burst);
        printf("%d\t", prio);
        printf("|%.1f\t", timeWhenFinished);
        printf("%.1f\t", waitingTime);
        printf("%.1f\t", turnAroundTime);
        printf("%.1f\t", responseTime);
        printf("%.0f\t\n", noOfContext);
    }
};

// queue that we will use in order to schedule all of our processes
struct Queue {
    int frontOfQ;
    int backOfQ;
    int qSize;

    process* q;

    explicit Queue(int c)
    {
        frontOfQ = backOfQ = 0;
        qSize = c;
        q = new process[qSize];
    }

    ~Queue()
    {
        delete[] q;
    }

    // adds a process to the queue
    void addToQ(process newElement)
    {
        // if the size is the same size as back of the queue then the queue is full
        if (qSize == backOfQ)
        {
            cout << "Queue is full!" << endl;
        }
        // if the queue is not full then we add an element to the back and increase the back
        else
        {
            q[backOfQ] = newElement;
            backOfQ++;
        }
    }

    // removes an element from the beginning of the queue
    void removeFromQ()
    {
        // if the back of the queue is the same as the end of the queue and its empty and theres nothing to remove
        if (backOfQ == frontOfQ)
        {
            cout << "Queue is empty!" << endl;
            return;
        }
        // otherwise just move every element up the queue by one and decrement backOfQ
        else {
            for (int i = 0; i < backOfQ - 1; i++)
            {
                if (i + 1 > backOfQ)
                {
                    return;
                }
                q[i] = q[i + 1];
            }
            backOfQ--;
        }
    }

    // returns the value at the start of the queue
    process whatIsAtStart() const
    {
        return q[frontOfQ];
    }

    // checks if the queue is empty
    bool isEmpty() const
    {
        return frontOfQ == backOfQ;
    }

    // prints the queue and everything inside of it
    void printQ() const
    {
        for (int i =0;i<backOfQ;i++)
        {
            cout<<"Spot"<<i<<" "<<q[i].pID<<" ";
        }
        cout<<endl;
    }

    // checks a priority of the element that we are about to add, includes multiple scenarios
    bool prio(process proc)
    {
        // if the queue is empty we simply add thethe beginning
        if(isEmpty())
        {
            addToQ(proc);
            //printQ();
            return false;
        }
        // if priority of the process we are adding is higher than the priority of the current first element
            // then we simply move every element in the queue to the right by one and then put the new element
                // into the first spot of the queue
        else if(whatIsAtStart().prio > proc.prio)
        {
            for(int i = backOfQ; i>=0; i--)
            {
                q[i+1] =q[i];
            }
            q[0] = proc;
            backOfQ++;
            //printQ();
            return true;
        }
        // if the new elements prio is not higher than our first elements we walk along the queue and add that element
            // into the spot where the process before it has higher prio and the process after it has a lower prio
        else
        {
            bool added = false;
            for(int i = 0; i < backOfQ; i++)
            {
                if(q[i].prio > proc.prio)
                {
                    for (int j = backOfQ; j>=i; j--)
                    {
                        q[j+1] =q[j];
                    }
                    q[i]=proc;
                    added=true;
                    backOfQ++;
                    break;
                }
            }
            // if for some reason the place in the middle of the queue wasn't found then we just add an element the back
                //of the queue
            if(!added)
            {
                addToQ(proc);
            }
            //printQ();
            return false;
        }
    }
};

// simply reads in info form file using sstream
vector<process> readFromFile(vector<process> &procVec, string const &UserFilename, int &procCounter)
{
    ifstream userInput(UserFilename);
    string lineFromFile;
    int value;

    process tempProcess;

    // reads from the file untill theres no lines to read anymore
    // stores all of the info into a temp process and then pushes the process into our main vector
    while (getline(userInput, lineFromFile))
    {
        stringstream infoDivider(lineFromFile);

        infoDivider >> value;
        tempProcess.pID = value;

        infoDivider >> value;
        tempProcess.arrTime = value;

        infoDivider >> value;
        tempProcess.burst = value;

        infoDivider >> value;
        tempProcess.prio = value;

        tempProcess.roundRobin = tempProcess.burst;
        tempProcess.priorityMath = tempProcess.burst;


        procVec.push_back(tempProcess);
        procCounter++;
    }

    // returns the vector when done
    return procVec;
}

// getting all of the numbers from the schedueling function, and when that func is done with the process, we simply
    // calculate everything we can about that process and store it inside it
void calcTheNums(process &proc, double time)
{
    // time when the process is fully completed
    proc.timeWhenFinished = time;

    // time how long it took to service the process from its arrival to when its done whenDone - whenArrived
    proc.turnAroundTime = proc.timeWhenFinished - proc.arrTime;

    // time how long the process had to wait, full service - burst
    proc.waitingTime = proc.turnAroundTime - proc.burst;

    // time how long it took for us to do at least some part of the process first time we touch it - arrival
    proc.responseTime = proc.firstTimeTouched - proc.arrTime;
}

// made a getter function to make bubble sort universal, so far it borks only sorting by burst, but if we add more
    // "get" functions like the one below they will work too
typedef int getter(process);

// returns burst of the process
int getBurst(process p)
{
    return p.burst;
}

// sorts from smaller to larger
void bubbleSort(vector<process>& a, int(*getter)(process))
{
    bool swap;
    process temp;
    // if there is nothing inside the passed array then return the array the way it is
    if (a.size() <= 1)
    {
        return;
    }
    // otherwise sort by whatever you want it to
    do {
        swap = false;
        for (int count = 0; count < a.size() - 1; count++) {
            // compares to values that are inside of the array by whatever the user wants it to
            if (getter(a[count]) > getter(a[count + 1]))
            {
                temp = a[count];
                a[count] = a[count + 1];
                a[count + 1] = temp;
                swap = true;
            }
        }
    } while (swap);
}

// creates first come first serve schedule
void firstComeFirstServe(vector<process> &procVec, int numOfProcess)
{
    //the actual time of the processes
    int time = 0;

    // when the process got touched for the first time
    int serviceTime = 0;

    // how long it takes for the process to finish
    int burstTimeofCurrentProc = 0;

    // queue that holds all of the processes
    Queue q(numOfProcess);

    // bool that makes sure that the queue is empty
    bool FCFS = true;

    // when we are done with proc we switch to a new ones burst time
    bool doneWithProc = true;

    while (FCFS)
    {
        //checks for new arrivals all the time
        for (int i = 0; i < numOfProcess; i++)
        {
            if (time == procVec[i].arrTime)
            {
                q.addToQ(procVec[i]);
            }
        }

        // adds the burstTime of the current proc into a temp variable so we can subtract it
        if (doneWithProc) {
            //update the service time when we start a new process
            serviceTime = time;
            burstTimeofCurrentProc = q.whatIsAtStart().burst;

            for (int s = 0; s < numOfProcess; s++)
            {
                if (q.whatIsAtStart().pID == procVec[s].pID)
                {
                    if (procVec[s].firstTimeTouched == 0)
                    {
                        procVec[s].firstTimeTouched = time;
                        procVec[s].touched = true;
                    }
                    break;
                }
            }

            doneWithProc = false;
        }

        // time goes up then we completed one second of the process
        time++;
        burstTimeofCurrentProc--;

        //checks if we are done with the process
        if (burstTimeofCurrentProc == 0)
        {
            for (int s = 0; s < numOfProcess; s++)
            {
                if (q.whatIsAtStart().pID == procVec[s].pID)
                {
                    calcTheNums(procVec[s], time);
                    break;
                }
            }

            q.removeFromQ();
            doneWithProc = true;
        }

        //if the queue is empty in the end then we are done
        if (q.isEmpty())
        {
            FCFS = false;
        }
    }
}

// creates shortest process first schedule
void shortestProcessFirst(vector<process> &procVec, int numOfProcess)
{
    //the actual time of the processes
    int time = 0;

    // when the process got touched for the first time
    int serviceTime = 0;

    int tnp = numOfProcess;

    // how long it takes for the process to finish
    int burstTimeofCurrentProc = 0;

    // queue that holds all of the processes
    Queue q(numOfProcess);

    vector<process> tempVec;

    // bool that makes sure that the queue is empty
    bool SPF = true;

    // when we are done with proc we switch to a new ones burst time
    bool doneWithProc = true;

    while (SPF)
    {
        // checks for new arrivals all the time if there are...
        for (int i = 0; i < numOfProcess; i++)
        {
            // ...pushes all of the new processes into a vector
            if (time == procVec[i].arrTime)
            {
                tempVec.push_back(procVec[i]);
            }
        }

        // if we are done with the last process we see what other processes have arrived into the vector
        if (doneWithProc) {
            // then we sort the vector
            bubbleSort(tempVec, getBurst);

            // add the sorted vector into the queue
                q.addToQ(tempVec[0]);
            tempVec.erase(tempVec.begin());

            //update the service time when we start a new process
            serviceTime = time;
            burstTimeofCurrentProc = q.whatIsAtStart().burst;

            for (int s = 0; s < numOfProcess; s++)
            {
                if (q.whatIsAtStart().pID == procVec[s].pID)
                {
                    if (procVec[s].firstTimeTouched == 0)
                    {
                        procVec[s].firstTimeTouched = time;
                        procVec[s].touched = true;
                    }
                    break;
                }
            }

            doneWithProc = false;
        }

        // time goes up then we completed one second of the process
        time++;
        burstTimeofCurrentProc--;

        // checks if we are done with the process
        if (burstTimeofCurrentProc == 0)
        {
            for (int s = 0; s < numOfProcess; s++)
            {
                if (q.whatIsAtStart().pID == procVec[s].pID)
                {
                    calcTheNums(procVec[s], time);
                    break;
                }
            }

            // update turn info
            q.removeFromQ();
            tnp--;
            doneWithProc = true;
        }

        //if the queue is empty in the end then we are done
        if ((q.isEmpty()) && (tnp <= 0))
        {
            SPF = false;
        }
    }
}

// creates highest prio first schedule
void highestPriorityFirst(vector<process> &procVec, int numOfProcess)
{
    // variables
    // the actual time of the processes
    double time = 0;

    // counter that makes sure that we are done with all of the processes
    int tnp = numOfProcess;

    // how long it takes for the process to finish
    double burstTimeOfCurrentProc = 0;

    // queue that holds all of the processes
    Queue q(numOfProcess+1);

    // vector that keeps track of the processes that came while our current process has been bursting
    // we will sort this vector to make sure everything goes into the queue at the proper time,
    // aka the shortest process goes first
    vector<process> tempVec;

    // bool that makes sure that the queue is empty
    bool PP = true;

    // when we are done with proc we switch to a new ones burst time
    bool doneWithProc = true;

    int numOfProcessesLeftToAdd = numOfProcess;

    int justFinished = -1;

    bool brek = true;

    // program
    while (PP)
    {

        // var that checks if we've added in this turn
        bool add = false;

        // if there are some processes left to add then we check for procs to add
            // we do this so we don't have to check if every turn if there are more processes left to add
                // if all of the processes have been added
        if (numOfProcessesLeftToAdd > 0)
        {
            if(justFinished == time)
            {
                // goes thru every process in the vector
                for (int i = 0; i < numOfProcess; i++)
                {
                    // if the process hasn't been added yet and the time matches then...
                    if ((!procVec[i].added) && (procVec[i].arrTime <= time))
                    {
                       q.prio(procVec[i]);
                    // decrements number of process to add
                    procVec[i].added = true;
                    add = true;
                    numOfProcessesLeftToAdd--;
                    }
                }
            }

            // goes thru every process in the vector
            for (int i = 0; i < numOfProcess; i++)
            {
                // if the process hasn't been added yet and the time matches then...
                if ((!procVec[i].added) && (procVec[i].arrTime <= time))
                {
                    // ...we check if the priority of the new process is higher than the current process'
                    if(procVec[i].prio < q.whatIsAtStart().prio)
                    {
                        // goes thru the array and the process that was switched to a new one gets
                            // number of context added to it
                        for (int s = 0; s < numOfProcess; s++)
                        {
                            // after that we are done with the process we update the process info
                            if (q.whatIsAtStart().pID == procVec[s].pID)
                            {
                                procVec[s].noOfContext++;
                                break;
                            }
                        }
                    }

                    // updates time and lets the project get updated
                    if(q.prio(procVec[i]))
                    {
                        doneWithProc = true;
                        time = time + 0.5;
                    }

                    // decrements number of process to add
                    procVec[i].added = true;
                    add = true;
                    numOfProcessesLeftToAdd--;
                }
            }
        }
        // exact same thing as above but makes sure that there is a process coming at time 0, if there isnt
            // then we simply move up the time
        if (!add && numOfProcessesLeftToAdd > 0 && q.isEmpty())
        {
            while (!add)
            {
                time++;
                for (int i = 0; i < numOfProcess; i++)
                {
                    if ((!procVec[i].added) && (procVec[i].arrTime <= time))
                    {
                        if(procVec[i].prio<q.whatIsAtStart().prio)
                        {
                            for (int s = 0; s < numOfProcess; s++)
                            {
                                // after that we are done with the process we update the process info
                                if (q.whatIsAtStart().pID == procVec[s].pID)
                                {
                                    procVec[s].noOfContext++;
                                    break;
                                }
                            }
                        }
                        if(q.prio(procVec[i]))
                        {
                            doneWithProc = true;
                            time = time + 0.5;
                        }
                        procVec[i].added = true;
                        add = true;
                        numOfProcessesLeftToAdd--;
                    }
                }
            }
        }
        // updates the process we are going to be editing
        if (doneWithProc)
        {
            // goes thru all of our processes in the vector of processes
            for (int s = 0; s < numOfProcess; s++)
            {
                // if the first spot in queue processID = one of the processes in the vector
                // that means that process is next and we are going to service it
                // therefore update the priority time
                if (q.whatIsAtStart().pID == procVec[s].pID)
                {
                    // saves the burstTime as priority math that will be updated
                    burstTimeOfCurrentProc = procVec[s].priorityMath;

                    // if the processes was never touched before
                    if (procVec[s].touched == false)
                    {
                        // then save this time as service time, meaning that this is the first time this process is beting
                        // touched therefore update the service time
                        // since after updating the time, the process have been touched now, we set touched to
                        // true so it doesn't update anymore.
                        procVec[s].firstTimeTouched = time;
                        procVec[s].touched = true;
                    }
                    // since we've found the process get out of the loop
                    break;
                }
            }
            // tell the program that we are done with being in the done with proc state, and because of that we start
            // working on the new process, and set this to false,
            // we'll only go back here when we are done with this process
            doneWithProc = false;
        }

        // time goes up then we completed one second of the process
        time  = time + 0.5;
        burstTimeOfCurrentProc = burstTimeOfCurrentProc - 0.5;

        for (int s = 0; s < numOfProcess; s++)
        {
            // after that we are done with the process we update the process info
            if (q.whatIsAtStart().pID == procVec[s].pID)
            {
                procVec[s].priorityMath = procVec[s].priorityMath-0.5 ;
                break;
            }
        }

        //checks if we are done with the process
        if (burstTimeOfCurrentProc == 0)
        {
            for (int s = 0; s < numOfProcess; s++)
            {
                // after that we are done with the process we update the process info
                if (q.whatIsAtStart().pID == procVec[s].pID)
                {
                    calcTheNums(procVec[s], time);
                    break;
                }
            }
            // if the process is done we remove it from the queue
            q.removeFromQ();
            tnp--;
            // go back to the done with proc time
            doneWithProc = true;
            justFinished = time;
        }

        //if the queue is empty in the end then we are done
        if ((q.isEmpty()) && (tnp <= 0))
        {
            PP = false;
        }
    }

}

// calcs the averages for the schedueling algos
void CalcAvgs(vector<process> procVec, int processCounter)
{
    double avg=0;
    int cSwitches=0;

    // calc avg cpu time for all processes
    for (int i =0; i<processCounter;i++)
    {
        avg = avg + procVec[i].burst;
    }
    avg = avg / processCounter;
    cout << "Average CPU Burst Time = " << avg << "ms \t";

    // calc avg turnAround time for all processes
    avg = 0;
    for (int i =0; i<processCounter;i++)
    {
        avg = avg + procVec[i].turnAroundTime;
    }
    avg = avg / processCounter;
    cout << "Average TurnAround Time = " << avg << "ms" << endl;

    // calc avg waiting time for all processes
    avg = 0;
    for (int i =0; i<processCounter ;i++)
    {
        avg = avg + procVec[i].waitingTime;
    }
    avg = avg / processCounter;
    cout << "Average Waiting Time = " << avg << "ms\t";

    // calc avg response time for all processes
    avg = 0;
    for (int i =0; i<processCounter;i++)
    {
        avg = avg + procVec[i].responseTime;
    }
    avg = avg / processCounter;
    cout << "Average Response Time = " << avg << "ms" << endl;

    // adds all of the context switches
    for (int i =0; i<processCounter;i++)
    {
        cSwitches = cSwitches + procVec[i].noOfContext;
    }
    cout << "Number of Context Switches = " << cSwitches <<endl;
}

// creates round robin schedule
void roundRobin(vector<process> &procVec, int numOfProcesses, int RRnum)
{
    // all of the variables
    Queue q(numOfProcesses + 1);

    int numOfProcessesLeft = numOfProcesses;
    double time = 0;
    int numOfProcessesLeftToAdd = numOfProcesses;

    int rrNumUnstable = RRnum;
    int rrProcLeft = 0;

    bool addProcChars = true;
    bool doneWithRRound = false;

    // it will stop when we are done with all of the processes
    while (true)
    {
        bool add = false;

        // if there are some processes left to add then we check for procs to add
        // we do this so we don't have to check if every turn if there are more processes left to add
        // if all of the processes have been added
        if (numOfProcessesLeftToAdd > 0)
        {
            for (int i = 0; i < numOfProcesses; i++)
            {
                // adds the processes into the q
                if ((!procVec[i].added) && (procVec[i].arrTime <= time))
                {
                    q.addToQ(procVec[i]);
                    procVec[i].added = true;
                    add = true;
                    numOfProcessesLeftToAdd--;
                }
            }
        }
        // does the same thing as above just makes sure that there is something in time 0
        if (!add && numOfProcessesLeftToAdd > 0 && q.isEmpty())
        {
            while (!add)
            {
                time++;
                for (int i = 0; i < numOfProcesses; i++)
                {
                    if ((!procVec[i].added) && (procVec[i].arrTime <= time))
                    {
                        q.addToQ(procVec[i]);
                        procVec[i].added = true;
                        add = true;
                        numOfProcessesLeftToAdd--;
                    }
                }
            }
        }

        // adds the unfinished round robnin process to the back of the q
        if (doneWithRRound)
        {
            // saves the current proces id
            int procID = q.whatIsAtStart().pID;
            q.removeFromQ();
            // removes that process from the queue
            for (int i = 0; i < numOfProcesses; i++)
            {
                // adds to context num because we are done with rrround
                if (procVec[i].pID == procID)
                {
                    procVec[i].noOfContext = procVec[i].noOfContext + 1;
                    q.addToQ(procVec[i]);
                    break;
                }
            }
            time = time + 0.5;
            doneWithRRound = false;
        }

        // get all of our info for round robin next process
        if (addProcChars)
        {
            // restarts the round robin counter
            rrNumUnstable = RRnum;
            // takes process roundRobin info of whatever is left from the last round
            rrProcLeft = q.whatIsAtStart().roundRobin;

            // if the process hasn't been touched yet then we set it that now it has been touched
            // that's so we know the service time
            for (int s = 0; s < numOfProcesses; s++)
            {
                if (q.whatIsAtStart().pID == procVec[s].pID)
                {
                    if (procVec[s].touched == 0)
                    {
                        procVec[s].firstTimeTouched = time;
                        procVec[s].touched = true;
                    }
                    break;
                }
            }
            addProcChars = false;
        }

        // subtract from round robin counter
        // subtract from the burst time of round robin
        // add to the time
        time++;
        rrProcLeft--;
        rrNumUnstable--;

        // if we reached 0 on rr or proc burst
        if ((rrNumUnstable == 0) || (rrProcLeft == 0))
        {
            // if we are not done with the process, we simply save the progress and then unlock an option of goign
                // to the next process in line and moving this one to the back
            if (rrProcLeft > 0)
            {
                for (int i = 0; i < numOfProcesses; i++)
                {
                    if (procVec[i].pID == q.whatIsAtStart().pID)
                    {
                        procVec[i].roundRobin = rrProcLeft;
                        break;
                    }
                }
                doneWithRRound = true;
            }
            // if the process is done then we just remove it from the queue and forget about it
                // also subtract the amount of processes we have left to do
            else
            {
                for (int i = 0; i < numOfProcesses; i++)
                {
                    if (procVec[i].pID == q.whatIsAtStart().pID)
                    {
                        procVec[i].roundRobin = -1;
                        calcTheNums(procVec[i],time);
                        break;
                    }
                }
                q.removeFromQ();
                numOfProcessesLeft--;
            }
            addProcChars = true;
        }

        // if there are nothing left to do then we are done.
        if (numOfProcessesLeft == 0)
        {
            return;
        }
    }
}


// Driver code
int main(int argc, char **argv)
{
    string filename = argv[1];
    char const *schedType = argv[2];

    //char const *schedType = "2";

    //string filename = "input10";
    vector<process> procVect;
    int userAlgoChoice;
    stringstream strValue;
    int processCounter = 0;

    //string bob = "4";

    readFromFile(procVect, filename, processCounter);

    strValue << schedType;
    strValue >> userAlgoChoice;


    switch (userAlgoChoice)
    {
        case 0:
            firstComeFirstServe(procVect, processCounter);
            cout << "*******************************************************" << endl;
            cout << "*************** First Come First Server ***************" << endl;
            cout << "*******************************************************" << endl;
            cout << "*******************************************************" << endl;

            break;
        case 1:
            shortestProcessFirst(procVect, processCounter);
            cout << "******************************************************" << endl;
            cout << "*************** Shortest Process First  **************" << endl;
            cout << "******************************************************" << endl;
            cout << "******************************************************" << endl;

            break;
        case 2:
            highestPriorityFirst(procVect, processCounter);
            cout << "******************************************************" << endl;
            cout << "*************** Highest Priority First   *************" << endl;
            cout << "******************************************************" << endl;
            cout << "******************************************************" << endl;

            break;
        case 3:
        {
            stringstream rrValue;
            int RRnum;
            //rrValue << bob;
            rrValue << argv[3];
            rrValue >> RRnum;

            roundRobin(procVect, processCounter, RRnum);
            cout << "************************************************" << endl;
            cout << "*************** Round Robin   ******************" << endl;
            cout << "*************** Time Quantum:"<<RRnum<<" *****************"<<endl;
            cout << "************************************************" << endl;

        }
        default:
            break;
    }

    cout << "pID\tarrival\tBurstT\tPrioT\tfinishT\twaitT\tturnAT\trespT\tNoOfContext\t" << endl;
    for (int i = 0; i < processCounter; i++)
    {
        procVect[i].printProcess();
    }
    CalcAvgs(procVect,processCounter);
    return 0;
}