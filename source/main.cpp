#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <cstdlib>
#include <algorithm>
#include <windows.h>
#include <time.h>
#include <filesystem>
using namespace std;


enum state_t {
    waiting, ready
};

enum event_t {
    arrival = 0, departure = 1
};

class Job {
    public:
    int job_id;
    int age; // number of cycles process has been through
    int arrival_time;
    int wait_time;
    int run_time;
    int response_time;
    Job(){}
    Job(int pid, int run_time) {
        this->job_id = pid;
        this->run_time = run_time;
        this->response_time = -1;
        this->age = 0;
        this->arrival_time = 0;
        this->wait_time = 0;
    }
};
class Event {
public:
    int processor = -1;
    int event_id = 0;
    Job job;
    event_t event_type;
    int time;
    Event(Job job, int event_id, event_t event_type, float time, int processor) {
        this->job = job;
        this->time = time;
        this->event_id = event_id;
        this->event_type = event_type;
        this->processor = processor;
    }
    
    void setEvent(event_t newEvent) {
        this->event_type = newEvent;
    }
    void setArrivalTime(int newTime) {
        time = newTime;
    }

};

class CPU {
    public:
    int id;
    bool available;
    // Statistics variables here
    float avg_throughput = 0.0; //throughput 
    float avg_tat = 0.0; //turnaround time
    float avg_wait = 0.0; //wait time
    float avg_resp = 0.0; //response time
    CPU(int id) {
        this->id = id;
        available = true;
    }
    
    void updateWaitTime(Job process){
        this->avg_wait = (this->avg_wait + process.wait_time)/2;
    }
    void updateResponseTime(Job process){
        this->avg_resp = (this->avg_resp + process.response_time)/2;
    }
    void updateTurnaroundTime(Job process){
        this->avg_tat = (this->avg_tat + (process.run_time - process.arrival_time))/2;
    }

    void setAvailable(bool available) {
        this->available = available;
    }
};


void createJobs(int);
int getTime(int);
void write_statistics(int*);
void arrivalEvent(Event &);
void departureEvent(Event &);
// Globals Here: 
vector<Event> FEL = {};
queue<Job> test = {};
vector<queue<Job>> readyQueue {{}, {}, {}};
 //Round Robin 1 -> Round Robin 2 -> FCFS
vector<Job> waitingJobs;
vector<Job> completedJobs; //once a job is finished, put it here
int event_ids = 0;
    
int CURRENT_TIME = 0;
int STOP_TIME = 50; // when sim hits this, it stop

int rr1 = 10;
int rr2 = 20;

CPU P1 = CPU(1); 
CPU P2 = CPU(2);
// CPU P3 = CPU(3);
vector<CPU> Processors = {P1, P2};


//// PROGRAM START /////////////////////////////////////////////////
int main() {
    srand(time(NULL));
    createJobs(10);
    queue<Job> roundRobin1 = {};
    queue<Job> roundRobin2 = {};
    queue<Job> FCFS = {};

    readyQueue[0] = roundRobin1;
    readyQueue[1] = roundRobin2;
    readyQueue[2] = FCFS;

    Job dummyJob = waitingJobs[0];
    waitingJobs.erase(waitingJobs.begin());
    Event e = Event(dummyJob, 0, arrival, 0, NULL);
    FEL.push_back(e);
    event_ids++;
    while (CURRENT_TIME < STOP_TIME) {
        Sleep(250);
        // for each item from FEL.begin() to FEL.end():
        // returns a comparison between the E1 and E2 time
        // the list will be re-sorted on every new iteration
        if(FEL.empty() == true) {
            break;
        }
        Event imminentEvent = FEL.front();
        
        CURRENT_TIME = imminentEvent.time;
        FEL.erase(FEL.begin()); // pop_front but for vector (switch to stack/queue?)
        switch(imminentEvent.event_type) {
            case arrival:
                cout << "Time: " << CURRENT_TIME << " : Arrival" << endl;
                Sleep(250);
                arrivalEvent(imminentEvent);
                break;
            case departure: 
                cout << "Time: " << CURRENT_TIME << " : Departure" << endl;
                Sleep(250);
                departureEvent(imminentEvent);
                break;
        }
        if(FEL.size() > 1) {
            for(int i = 1; i < FEL.size(); i++) {
                for(int j = 0; j < FEL.size(); j++) {
                    if(FEL[i].time < FEL[j].time) {
                        swap(FEL[i], FEL[j]);
                    }
                }
            }
        }
    }
}
//// PROGRAM END ///////////////////////////////////////////////////


//generate all jobs/processes
void createJobs(int numJobs) {

    for(int i=0;i<numJobs;i++){
        int run_time = rand() % 25 + 1;
        Job j = Job(i, run_time);
        waitingJobs.push_back(j);
    }
};

void write_statistics(int* data, string fileName) {
    fstream fout;
    fout.open((fileName + ".csv"), ios::out | ios::app);
    ifstream read (fileName + ".csv");
    if(read.peek() == EOF){ //On initial writing to file, we need to add the labels for columns
    fout << "Job ID" << ", " << "Arrival Time" << ", " << "Wait Time" << ", " << "Run Time" << ", " << "Response Time" << "\n";

    }

    fout << data[0] << ", " << data[1] << ", " << data[2] << ", " << data[3] << ", " << data[4] << "\n";


}

// Give it a job (pointer), 
int getTime(int age) {
    if(age == 0) {
        return 10;
    }
    else if(age == 1) {
        return 20;
    }
    else return 9999;
}

void arrivalEvent(Event &nowEvent) {
    Job nextJob;
    int interarrivalTime = rand() % 5 + 1;

    // grab next process
    if(waitingJobs.size() > 0) {
        nextJob = waitingJobs.front();
        nextJob.arrival_time = CURRENT_TIME + interarrivalTime;
        waitingJobs.erase(waitingJobs.begin());
    }
    else {
        nextJob = Job(-1, 0);
    }


    // Schedule next arrival
    if(nextJob.job_id != -1) {FEL.push_back(Event(nextJob, event_ids, arrival, (CURRENT_TIME + interarrivalTime), NULL));}
    event_ids++;
    cout << "Time: " << CURRENT_TIME << " : Scheduled Arrival in arrival()" << endl;                  

    int available_processor_index = -1;
    bool flag = true;
    // store index of first available processor in available_processor_index
    for(int i = 0; i < Processors.size() && flag == true; i++) {
        if(Processors[i].available == true) {
            available_processor_index = i;
            flag = false;
        }
    }

    cout << "PROCESSOR INDEX: " << available_processor_index << endl;
    if(available_processor_index > -1) {
        //make depart event
        Processors[available_processor_index].available = false;
        int tq = getTime(nowEvent.job.age); //time quantum
        if(nowEvent.job.response_time == -1){
        nowEvent.job.response_time = CURRENT_TIME - nowEvent.job.arrival_time;
        }
        nowEvent.job.wait_time += CURRENT_TIME - nowEvent.job.arrival_time;

        FEL.push_back(Event(nowEvent.job, event_ids, departure, CURRENT_TIME + min(tq, nowEvent.job.run_time), available_processor_index));
        event_ids++;
        cout << "Time: " << CURRENT_TIME << " : Scheduled Departure in arrival()" << endl;
    }
    // Pushes to different queues based on age, higher age means lower priority
    else {
        if(nowEvent.job.age == 0) { // push to rr10
            cout << "Time: " << CURRENT_TIME << " : pushing to rr10" << endl;
            nowEvent.job.arrival_time = CURRENT_TIME;
            readyQueue[0].push(nowEvent.job);
        }
        else if(nowEvent.job.age == 1) { // push to rr20
            cout << "Time: " << CURRENT_TIME << " : pushing to rr20" << endl;
            nowEvent.job.arrival_time = CURRENT_TIME;
            readyQueue[1].push(nowEvent.job);
        }
        else {
            cout << "Time: " << CURRENT_TIME << " : pushing to fcfs" << endl;
            nowEvent.job.arrival_time = CURRENT_TIME;
            readyQueue[2].push(nowEvent.job); //push to FCFS
        }
    }
}

void departureEvent(Event &nowEvent) {
    cout << "Departure" << endl;
    Job nextJob;
    int switch_time = 2;
    
    // check queues in ascending order(rr10,rr20,FCFS)
    if(readyQueue[0].empty() == false) { // rr10 queue
        nextJob = readyQueue[0].front();
        readyQueue[0].pop();    //takes next process
        if(nextJob.response_time == -1) {
            nextJob.response_time = CURRENT_TIME - nextJob.arrival_time;
        } // calculates and sets response_time
        nextJob.wait_time += CURRENT_TIME - nextJob.arrival_time;
        FEL.push_back(Event(nextJob, event_ids, departure, CURRENT_TIME + switch_time, nowEvent.processor));
        event_ids++;
        cout << "Time: " << CURRENT_TIME  << " : Scheduled Departure rr10 on departure()" << endl;        
    }
    else if( readyQueue[1].empty() == false) { // rr20 queue
        nextJob = readyQueue[1].front();
        nextJob.wait_time += CURRENT_TIME - nextJob.arrival_time;
        readyQueue[1].pop();
        FEL.push_back(Event(nextJob, event_ids, departure, CURRENT_TIME + switch_time, nowEvent.processor));
        event_ids++;
        cout << "Time: " << CURRENT_TIME  << " : Scheduled Departure rr20 on departure()" << endl;

    }
    else if( readyQueue[2].empty() == false) { // FCFS queue
        nextJob = readyQueue[2].front();
        readyQueue[2].pop();
        nextJob.wait_time += CURRENT_TIME - nextJob.arrival_time;
        FEL.push_back(Event(nextJob, event_ids, departure, CURRENT_TIME + switch_time, nowEvent.processor));
        event_ids++;
        cout << "Time: " << CURRENT_TIME  << " : Scheduled Departure on fcfs departure()" << endl;

    }
    else {
        Processors[nowEvent.processor].available = 0;
    }
    int timeQuantum = getTime(nowEvent.job.age);
    int checkCompletion = nowEvent.job.run_time - timeQuantum;
    
    if(checkCompletion <= 0) {//current job is completed
        int data[5] = {nowEvent.job.job_id, nowEvent.job.arrival_time, nowEvent.job.wait_time , nowEvent.job.run_time, nowEvent.job.response_time};
        write_statistics(data, "data");
        cout << nowEvent.job.arrival_time << nowEvent.job.job_id << nowEvent.job.response_time << nowEvent.job.wait_time << endl;
    }
    else if(checkCompletion > 0) { // quantum timeout
        nowEvent.job.age += 1;
        nowEvent.job.run_time = checkCompletion;
        FEL.push_back(Event(nowEvent.job, nowEvent.event_id, arrival, nowEvent.time, nowEvent.processor));
        event_ids++;
    }
}