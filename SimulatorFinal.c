/***********************************************
 * Operating System Term project               *
 * CPU Scheduling Simulator                    *
 *                                             *
 * Worked by 2020120036 경영학과 오윤진         *
 * Last update: 23/05/26                       *
 *                                             *
 *                                             *
 *                                             *
 * 수정/확인 필요한 사항                         *
 * 1. 간트차트 생성 함수 추가 
 * 2. 하나의 프로세스에서 I/O 발생 시, CPU idle 상태가 되는지 타 프로세스로 전환되는지 확인
 * 3. 잡큐, 레디큐, 웨이팅 큐 에서 중복되는 소스코드 정리 (디버깅 필요 시 각 큐의 print 함수 추가)
 * 4. 전역 변수 정리 (코드 곳곳에 퍼져 있어 다소 혼란)
 * 5. JQ Clone 의 역할 정확히 이해
 * 6. timeConsumed, amount, TIME_QUANTUM, time_quantum 변수에 대해 정확히 이해 
 * 7. RR 알고리즘 함수 검토 
 * 8. 시간이 된다면 소스코드에서 중복되는 부분 정리 (혹은 헤더파일로 모듈화)
 *                        
 * 
 * 
***********************************************/


// Header declaration
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Policy
#define MAX_TIME_UNIT 1000
#define MAX_PROCESS_NUM 10
#define MAX_ALGORITHM_NUM 10
#define TIME_QUANTUM 2

// Algorithm's Unique Number
#define FCFS 0
#define SJF 1
#define PRIORITY 2
#define RR 3
#define P_SJF 4    // Preemptive SJF
#define P_PRIORITY 5    // Preemptive PRIORITY

#define TRUE 1
#define FALSE 0


// Process
typedef struct process {

    int pid;
    int priority;
    int arrivalTime;
    int CPUburst;
    int IOburst;
    int CPUremainingTime;
    int IOremainingTime;    
    int waitingTime;    
    int turnaroundTime;
    int responseTime;

} process;
typedef struct process* processPointer;

int computation_start = 0;
int computation_end = 0;
int computation_idle = 0;


// Evaluation score
typedef struct evaluation {  

	int alg;
	int preemptive;
	int startTime;
	int endTime;
	int avg_waitingTime;
	int avg_turnaroundTime;
	int avg_responseTime;
	double CPU_util;
	int completed;

}evaluation;
typedef struct evaluation* evalPointer; 


// Evaluation array
evalPointer evals[MAX_ALGORITHM_NUM]; 

// Current evaluation target algorithm
int cur_eval_num = 0;   


// Initiate evaluation array for all 
void init_evals(){

	cur_eval_num = 0;
	int i;
	for (i=0;i<MAX_ALGORITHM_NUM;i++) 
		evals[i]=NULL; 
}


// Clear evaluation array for all 
void clear_evals() {

	int i;
	for( i=0;i<MAX_ALGORITHM_NUM;i++){
        // memory free
		free(evals[i]);  
		evals[i]=NULL;
	}
	cur_eval_num = 0;
}


//Job Queue (!= Ready queue) - pointer array
processPointer jobQueue[MAX_PROCESS_NUM];

// current # of process in JQ
int cur_proc_num_JQ = 0; 

void init_JQ () {

	cur_proc_num_JQ = 0;
    int i;
    for (i = 0; i < MAX_PROCESS_NUM; i++)
        jobQueue[i] = NULL;
}


// Sort with insertion sort by PID 
void sort_JQ() { 

    processPointer key;
    int i, j;

    for (i = 1; i < cur_proc_num_JQ; i++) {
        key = jobQueue[i];

        for (j = i - 1; (j >= 0) && (jobQueue[j]->pid > key->pid); j--) {
            jobQueue[j+1] = jobQueue[j];
        }
        jobQueue[j+1] = key;
    }
}


// Extract a process idx of given PID in job queue 
int getProcByPid_JQ (int givenPid) { 
    
    for(int i = 0; i < cur_proc_num_JQ; i++) {
        int temp = jobQueue[i]->pid;
        if(temp == givenPid)
            return i;
    }
    
    // if there is no matching process
    return -1;
}


// Insert a process in job queue
void insertInto_JQ (processPointer proc) {
    // possible
    if (cur_proc_num_JQ < MAX_PROCESS_NUM) {

        int temp = getProcByPid_JQ(proc->pid); 
        if (temp != -1) 
            printf("<ERROR> The process with pid: %d already exists in job queue.\n", proc->pid);
        jobQueue[cur_proc_num_JQ++] = proc;
    }

    // impossible(full)
    else
        printf("<ERROR> Job queue is full.\n");
}


// Remove a process from job queue (Extract that removed process)
processPointer removeFrom_JQ (processPointer proc) {

    // Job queue - not empty
    if (cur_proc_num_JQ > 0) { 
        int temp = getProcByPid_JQ(proc->pid); // idx

        if (temp == -1) {
            printf("<ERROR> Cannot find the process with pid: %d.\n", proc->pid);
            return NULL;    
        } 
        
        // Remove
        else {
            processPointer removed = jobQueue[temp];
            for(int i = temp; i < cur_proc_num_JQ - 1; i++)
                jobQueue[i] = jobQueue[i+1];   
            jobQueue[cur_proc_num_JQ - 1] = NULL;
            cur_proc_num_JQ--;
            return removed; // extract
        }
    } 
    
    // Job queue - empty
    else {
        puts("<ERROR> Job queue is empty.");
        return NULL;
    }
}


// Clear job queue (memory free)
void clear_JQ() {

    for(int i = 0; i < MAX_PROCESS_NUM; i++) {
        free(jobQueue[i]);
        jobQueue[i] = NULL;
    }

    cur_proc_num_JQ = 0;
}


// Clone job queue (Copy from job queue to clone)
processPointer cloneJobQueue[MAX_PROCESS_NUM];
int cur_proc_num_clone_JQ = 0;

void clone_JQ() {

    // initiate clone
	for (int i=0; i< MAX_PROCESS_NUM; i++) { 
		cloneJobQueue[i] = NULL;
	}
	
	for (int i=0; i < cur_proc_num_JQ; i++) {
		processPointer newProcess = (processPointer)malloc(sizeof(struct process));

        newProcess->pid = jobQueue[i]->pid;
        newProcess->priority = jobQueue[i]->priority;
        newProcess->arrivalTime = jobQueue[i]->arrivalTime;
        newProcess->CPUburst = jobQueue[i]->CPUburst;
        newProcess->IOburst = jobQueue[i]->IOburst;
        newProcess->CPUremainingTime = jobQueue[i]->CPUremainingTime;
        newProcess->IOremainingTime = jobQueue[i]->IOremainingTime;
        newProcess->waitingTime = jobQueue[i]->waitingTime;
        newProcess->turnaroundTime = jobQueue[i]->turnaroundTime;
        newProcess->responseTime = jobQueue[i]->responseTime;
        
        cloneJobQueue[i] = newProcess;
	}

	cur_proc_num_clone_JQ = cur_proc_num_JQ;
}


// Copy from clone to job queue
void loadClone_JQ() {

	clear_JQ();
	for (int i = 0; i < cur_proc_num_clone_JQ; i++) {
	
		processPointer newProcess = (processPointer)malloc(sizeof(struct process));
	    newProcess->pid = cloneJobQueue[i]->pid;
	    newProcess->priority = cloneJobQueue[i]->priority;
	    newProcess->arrivalTime = cloneJobQueue[i]->arrivalTime;
	    newProcess->CPUburst = cloneJobQueue[i]->CPUburst;
	    newProcess->IOburst = cloneJobQueue[i]->IOburst;
	    newProcess->CPUremainingTime = cloneJobQueue[i]->CPUremainingTime;
	    newProcess->IOremainingTime = cloneJobQueue[i]->IOremainingTime;
	    newProcess->waitingTime = cloneJobQueue[i]->waitingTime;
	    newProcess->turnaroundTime = cloneJobQueue[i]->turnaroundTime;
	    newProcess->responseTime = cloneJobQueue[i]->responseTime;
	    
	    jobQueue[i] = newProcess;
	}
	
	cur_proc_num_JQ = cur_proc_num_clone_JQ;
}


void clearClone_JQ() { 

    for(int i = 0; i < MAX_PROCESS_NUM; i++) {   
        free(cloneJobQueue[i]);
        cloneJobQueue[i] = NULL;
    }
}


// Current running process
processPointer runningProcess = NULL;
// Time consumed so far (연구 필요)
int timeConsumed = 0;


//Ready Queue (Assume that process is created with its arrival time sorted)
processPointer readyQueue[MAX_PROCESS_NUM];
int cur_proc_num_RQ = 0; 

void init_RQ () {
    cur_proc_num_RQ = 0;
    for (int i = 0; i < MAX_PROCESS_NUM; i++)
        readyQueue[i] = NULL;
}


int getProcByPid_RQ (int givenPid) { 

    for (int i = 0; i < cur_proc_num_RQ; i++) {
        int temp = readyQueue[i]->pid;
        if (temp == givenPid)
            return i; // index return
    }
    return -1;
}


void insertInto_RQ (processPointer proc) {

    if (cur_proc_num_RQ < MAX_PROCESS_NUM) {
        int temp = getProcByPid_RQ(proc->pid);
        if (temp != -1) {
            printf("<ERROR> The process with pid: %d already exists in Ready queue\n", proc->pid);
        }
        readyQueue[cur_proc_num_RQ++] = proc;
    }
    else {
        printf("<ERROR> Ready queue is full\n");
    }
}


processPointer removeFrom_RQ (processPointer proc) { 
    
    if (cur_proc_num_RQ > 0) {

        int temp = getProcByPid_RQ(proc->pid);  
        if (temp == -1) {
            printf("<ERROR> Cannot find the process with pid: %d\n", proc->pid);
            return NULL;    
        } 
        
        else {
            processPointer removed = readyQueue[temp];
            
            for(int i = temp; i < cur_proc_num_RQ - 1; i++)
                readyQueue[i] = readyQueue[i+1];   
            readyQueue[cur_proc_num_RQ - 1] = NULL;
            
            cur_proc_num_RQ--;
            return removed;
        }
    } 
    
    else {
        printf("<ERROR> Ready queue is empty\n");
        return NULL;
    }
}


void clear_RQ() {

    for(int i = 0; i < MAX_PROCESS_NUM; i++) {
        free(readyQueue[i]);
        readyQueue[i]=NULL;
    }
    cur_proc_num_RQ = 0;
}


// Waiting Queue
processPointer waitingQueue[MAX_PROCESS_NUM];
int cur_proc_num_WQ = 0; 

void init_WQ () {
	cur_proc_num_WQ = 0;
    for (int i = 0; i < MAX_PROCESS_NUM; i++)
        waitingQueue[i] = NULL;
}


int getProcByPid_WQ (int givenPid) { 

    for(int i = 0; i < cur_proc_num_WQ; i++) {
        int temp = waitingQueue[i]->pid;
        if(temp == givenPid)
            return i;
    }
    return -1;
}


void insertInto_WQ (processPointer proc) {

    if (cur_proc_num_WQ < MAX_PROCESS_NUM) {
        int temp = getProcByPid_WQ(proc->pid);

        if (temp != -1) {
            printf("<ERROR> The process with pid: %d already exists in Waiting queue\n", proc->pid);
        }
        waitingQueue[cur_proc_num_WQ++] = proc;
    }

    else {
        printf("<ERROR> Waiting queue is full\n");
    }
}


processPointer removeFrom_WQ (processPointer proc) { 

    if (cur_proc_num_WQ > 0) {

        int temp = getProcByPid_WQ(proc->pid);

        if (temp == -1) {
            printf("<ERROR> Cannot find the process with pid: %d\n", proc->pid);
            return NULL;    
        } 
        
        else {
            processPointer removed = waitingQueue[temp];
            for (int i = temp; i < cur_proc_num_WQ - 1; i++)
                waitingQueue[i] = waitingQueue[i+1];
            waitingQueue[cur_proc_num_WQ - 1] = NULL;
            cur_proc_num_WQ--;
            return removed;
        }
    } 
    
    else {
        printf("<ERROR> Waiting queue is empty\n");
        return NULL;
    }
}


void clear_WQ() { 

    for(int i = 0; i < MAX_PROCESS_NUM; i++) {
        free(waitingQueue[i]);
        waitingQueue[i] = NULL;
    }
    cur_proc_num_WQ = 0;
}


// Terminated Queue
processPointer terminated[MAX_PROCESS_NUM];
int cur_proc_num_T = 0; 

void init_T () {

	cur_proc_num_T = 0;
    for (int i = 0; i < MAX_PROCESS_NUM; i++)
        terminated[i] = NULL;
}


void clear_T() {

    for (int i = 0; i < MAX_PROCESS_NUM; i++) {
        free(terminated[i]);
        terminated[i] = NULL;
    }
    cur_proc_num_T = 0;
}


void insertInto_T (processPointer proc) {
    if (cur_proc_num_T < MAX_PROCESS_NUM) {
        terminated[cur_proc_num_T++] = proc;
    }
    else {
        printf("<ERROR> Cannot terminate the process\n");
    }
}


// Process creation with random value (return newProcess' address)
processPointer createProcess(int pid, int priority, int arrivalTime, int CPUburst, int IOburst) { 
    
	// Error 1
    if (arrivalTime > MAX_TIME_UNIT || arrivalTime < 0) {
        printf("<ERROR> arrivalTime should be in [0..MAX_TIME_UNIT]\n");
        return NULL;
    }
    
	// Error 2
    if (CPUburst <= 0 || IOburst < 0) {
        printf("<ERROR> CPUburst and should be larger than 0 and IOburst cannot be a negative number.\n");
        return NULL;
    }

    processPointer newProcess = (processPointer)malloc(sizeof(struct process)); 
    newProcess->pid = pid;
    newProcess->priority = priority;
    newProcess->arrivalTime = arrivalTime;
    newProcess->CPUburst = CPUburst;
    newProcess->IOburst = IOburst;
    newProcess->CPUremainingTime = CPUburst;
    newProcess->IOremainingTime = IOburst;
    newProcess->waitingTime = 0; 
    newProcess->turnaroundTime = 0;
    newProcess->responseTime = -1;
        
    // After creation, Insert it to JQ
    insertInto_JQ(newProcess);

    return newProcess;
}


/* Choose next process according to the specific algorithm - return next processPointer */


// FCFS - parameter # 0
processPointer FCFS_alg() { 
        
        // find first-arrive process in ready queue (Assume that ready queue is sorted by arrival time.)
        processPointer earliestProc = readyQueue[0];

        if (earliestProc != NULL){
            
            // if there is any running process
            if(runningProcess != NULL) 
				return runningProcess;
            // if there's nothing running
            else 
				return removeFrom_RQ(earliestProc);
        } 
        // nothing in ready queue
        else 
            return runningProcess;
}


// SJF - parameter # 1
processPointer SJF_alg(int preemptive) {
	
    // Just start from 0 (cannot sure that first process has the shortest burst time)
	processPointer shortestJob = readyQueue[0]; 
	
    // There is any process in ready queue
	if (shortestJob != NULL) {

        // find shortest job(process) in ready queue
        for (int i = 0; i < cur_proc_num_RQ; i++) { 

            // shortest job update (smaller)
            if (readyQueue[i]->CPUremainingTime < shortestJob->CPUremainingTime)
                shortestJob = readyQueue[i];
            // (same)
            else if (readyQueue[i]->CPUremainingTime == shortestJob->CPUremainingTime) {
                // update only if ith process arrive earlier
                if (readyQueue[i]->arrivalTime < shortestJob->arrivalTime) 
					shortestJob = readyQueue[i];
            }
            // (bigger)
            else {
                continue; // nothing happened
            }
        }
		
        // There is a running process
		if (runningProcess != NULL) {

                // Allowing preemption
				if (preemptive) {	

					// Switch
					if (runningProcess->CPUremainingTime > shortestJob->CPUremainingTime) {
						printf("preemption is detected.\n");
						insertInto_RQ(runningProcess); // Insert running process into ready queue 
						return removeFrom_RQ(shortestJob); // Bring shortest job(process) from ready queue
					}

                    // Optional switch
                    else if (runningProcess->CPUremainingTime == shortestJob->CPUremainingTime) {
                        // execute a process with earlier arrival time
                        if (runningProcess->arrivalTime <= shortestJob->arrivalTime)
                            return runningProcess;
                        else {
                            printf("preemption is detected.\n");
						    insertInto_RQ(runningProcess); // Insert running process into ready queue 
						    return removeFrom_RQ(shortestJob); // Bring shortest job(process) from ready queue
                        }
                    }

                    // No switch
                    else 
    					return runningProcess;
				}				

                // Non-allowing preemption
				return runningProcess;
        	} 
        // Nothing is running
        else 
		    return removeFrom_RQ(shortestJob); 
	}

    // nothing in ready queue
	else
		return runningProcess;
}


// PRIORITY - parameter # 2
processPointer PRIORITY_alg(int preemptive) {
	
    // Just start from 0
	processPointer importantJob = readyQueue[0]; 
	
	if (importantJob != NULL) {

        // find the most important job(process) in ready queue
        for (int i = 0; i < cur_proc_num_RQ; i++) { 

            // importantJob update 
            if (readyQueue[i]->priority < importantJob->priority)
                importantJob = readyQueue[i];
            
            // optional update
            else if (readyQueue[i]->priority == importantJob->priority) {
                // update only if ith process arrive earlier
                if (readyQueue[i]->arrivalTime < importantJob->arrivalTime) 
					importantJob = readyQueue[i];
            }
            // No update (nothing happened)
            else {
                continue; 
            }
        }

        // There is a running process
		if (runningProcess != NULL) { 

                // Allowing preemption
				if (preemptive) {  
				
                    // Switch
					if (runningProcess->priority > importantJob->priority) {
						printf("preemption is detected.\n");
						insertInto_RQ(runningProcess);
						return removeFrom_RQ(importantJob);
					}

                    // Optional switch
                    else if (runningProcess->priority == importantJob->priority) {
                        // execute a process with earlier arrival time
                        if (runningProcess->arrivalTime <= importantJob->arrivalTime)
                            return runningProcess;
                        else {
                            printf("preemption is detected.\n");
						    insertInto_RQ(runningProcess);
						    return removeFrom_RQ(importantJob);
                        }
                    }

                    // No switch
                    else 
    					return runningProcess;
				}

	            // Non-allowing preemption
				return runningProcess;
        	} 

        // Nothing is running 
        else {
			return removeFrom_RQ(importantJob);
		}
	}
    // There is nothing in ready queue
    else {
		return runningProcess;
	}
}


// RR - parameter # 3
processPointer RR_alg(int time_quantum){
    
    // find first-arrive process in ready queue (Assume that ready queue is sorted by arrival time.)
	processPointer earliestProc = readyQueue[0]; 
        
    if (earliestProc != NULL){
            
        if (runningProcess != NULL) { 
			// Existing running process - time expired (수정 필요)
	    	if (timeConsumed >= TIME_QUANTUM){ // timeConsumed: running process' running time until amount
				insertInto_RQ(runningProcess);
				return removeFrom_RQ(earliestProc); // 확인 필요
			} 
            else 
			    return runningProcess;	
        }
        else 
	    	return removeFrom_RQ(earliestProc);
    } 
        
    // There is nothing in ready queue
    else { 
        return runningProcess; 
    }
}


// Execute scheduling algorithm under time limit
processPointer schedule(int alg, int preemptive, int time_quantum) { 
	
    processPointer selectedProcess = NULL;
    
    switch(alg) {
        case FCFS:
            selectedProcess = FCFS_alg();
            break;
        case SJF:
        	selectedProcess = SJF_alg(preemptive);
        	break;
        case RR:
        	selectedProcess = RR_alg(time_quantum);
        	break;
        case PRIORITY:
        	selectedProcess = PRIORITY_alg(preemptive);
        	break;
        default:
        return NULL;
    }
    
    return selectedProcess;
}


// Simulate with loop (amount: time passed by)
void simulate(int amount, int alg, int preemptive, int time_quantum) { 

	processPointer tempProcess = NULL;
	
	for (int i = 0; i < cur_proc_num_JQ; i++) {

        // when the amount of time gets to the arrival time of a process
		if (jobQueue[i]->arrivalTime == amount) { 
			tempProcess = removeFrom_JQ(jobQueue[i--]);
			insertInto_RQ(tempProcess);
		}
	}

	processPointer prevProcess = runningProcess;
    // pick up the process that will be executed in this turn (return selected process)
	runningProcess = schedule(alg, preemptive, time_quantum); 
	
	printf("%d: ", amount);

    // Process switch (간트차트 그리기 위해 수정 필요)
	if (prevProcess != runningProcess) {

        // initiate the running time of current process (becasue of switching)
		timeConsumed = 0; 
		
        // Update responseTime of new running process
		if (runningProcess->responseTime == -1) { 
			runningProcess->responseTime = amount - runningProcess->arrivalTime;
		}
	}
	
    // Process in ready queue are waiting for CPU
    for (int i = 0; i < cur_proc_num_RQ; i++) { 
        
        if (readyQueue[i]) {
        	readyQueue[i]->waitingTime++;
        	readyQueue[i]->turnaroundTime++;
    	}
    }
	
    // Process in waiting queue (IO burst)
    for (int i = 0; i < cur_proc_num_WQ; i++) {

		if (waitingQueue[i]) {
			waitingQueue[i]->turnaroundTime++;
			waitingQueue[i]->IOremainingTime--;
			
            // IO completed
			if (waitingQueue[i]->IOremainingTime <= 0 ) {
				printf("(pid: %d) -> IO completed\n", waitingQueue[i]->pid); 
                // Turn that process back to the ready queue
				insertInto_RQ(removeFrom_WQ(waitingQueue[i--]));
			}
		}
	}
	
    // If there is a running process
    if (runningProcess != NULL) {  
        runningProcess->CPUremainingTime--;
        runningProcess->turnaroundTime++;
        timeConsumed++;
        printf("(pid: %d) -> running",runningProcess->pid);
        
        // If the process is 'completed', send it to terminated queue
        if (runningProcess->CPUremainingTime <= 0) { 
			insertInto_T(runningProcess);
			runningProcess = NULL;
			printf("-> terminated");
		} 
        // Not yet completed 
        else { 
            // If it has remaining IO burst, send it to waiting queue
			if (runningProcess->IOremainingTime > 0) {  
				insertInto_WQ(runningProcess);
				runningProcess = NULL;
				printf("-> IO request");	
			}
		}
		
        printf("\n");
    } 
    
    // If there is NO ruuning process
    else { 
        printf("idle\n");
    	computation_idle++;
	}
}


void analyze(int alg, int preemptive) {
	
	int wait_sum = 0;
	int turnaround_sum = 0;
	int response_sum = 0;
	processPointer p = NULL;
	
    // print analyzing result
	printf("===========================================================");
    for (int i=0; i<cur_proc_num_T; i++){
		p = terminated[i];
		printf("(pid: %d)\n",p->pid);
		printf("waiting time = %d, ",p->waitingTime);
		printf("turnaround time = %d, ",p->turnaroundTime);
		printf("CPU remaining time = %d\n",p->CPUremainingTime); // for debug (delete later)
		printf("IO remaining time = %d\n",p->IOremainingTime);   // for debug (delete later)
		printf("response time = %d\n",p->responseTime);
		printf("===========================================================");

		wait_sum += p->waitingTime;
		turnaround_sum += p->turnaroundTime;
		response_sum += p->responseTime;
	}

	printf("start time: %d / end time: %d / CPU utilization : %.2lf%% \n",computation_start, computation_end,
	 (double)(computation_end - computation_idle)/(computation_end - computation_start)*100);

	if (cur_proc_num_T != 0) {
		printf("Average waiting time: %d\n",wait_sum/cur_proc_num_T);
		printf("Average turnaround time: %d\n",turnaround_sum/cur_proc_num_T);
		printf("Average response time: %d\n",response_sum/cur_proc_num_T);
	}	
	
    printf("Number of completed process: %d\n",cur_proc_num_T);
		
	if (cur_proc_num_T != 0) {
		evalPointer newEval = (evalPointer)malloc(sizeof(struct evaluation));
		newEval->alg = alg;
		newEval->preemptive = preemptive;
		
		newEval->startTime = computation_start;
		newEval->endTime = computation_end;
		newEval->avg_waitingTime = wait_sum/cur_proc_num_T;
		newEval->avg_turnaroundTime = turnaround_sum/cur_proc_num_T;
		newEval->avg_responseTime = response_sum/cur_proc_num_T;
		newEval->CPU_util = (double)(computation_end - computation_idle)/(computation_end - computation_start)*100;
		newEval->completed = cur_proc_num_T;
		evals[cur_eval_num++] = newEval;
	}
	printf("===========================================================");
}


// Start Simulation 
void startSimulation(int alg, int preemptive, int time_quantum, int count) {
	
    // load clone to job queue
    loadClone_JQ();

	switch(alg) {
        case FCFS:
            printf("<FCFS Algorithm>");
            break;
        case SJF:
        	if (preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	printf("SJF Algorithm>");
        	break;
        case RR:
        	printf("<Round Robin Algorithm (time quantum: %d)>\n",time_quantum);
        	break;
        case PRIORITY:
        	if(preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	printf("Priority Algorithm>");
        	break;
        default:
        return;
    }
	
    // Save the number of process before simulating 
	int initial_proc_num = cur_proc_num_JQ;  
	
	int i;
	if (cur_proc_num_JQ <= 0) {
		printf("<ERROR> Simulation failed. Process doesn't exist in the job queue");
		return;
	}
	
	int minArriv = jobQueue[0]->arrivalTime;
    // minArriv update
	for (i=0; i < cur_proc_num_JQ; i++) {
		if(minArriv > jobQueue[i]->arrivalTime)
			minArriv = jobQueue[i]->arrivalTime;		
	}

	computation_start = minArriv;
	computation_idle = 0;

    // Repeat simulation in count time (DYNAMIC effect like time passes)
	for (i=0; i < count; i++) {
		simulate(i,alg, preemptive, TIME_QUANTUM);
		if (cur_proc_num_T == initial_proc_num) {
			i++;
			break;
		}
	}
	computation_end = i-1;
	
	analyize(alg, preemptive);
	clear_JQ();
    clear_RQ();
    clear_T();
    clear_WQ();
    free(runningProcess);
    runningProcess = NULL;
    timeConsumed = 0;
    computation_start = 0;
	computation_end = 0;
	computation_idle = 0;
}


void evaluate() {
	
	printf("\n                       <Evaluation>                    \n");
	
    for(int i=0; i < cur_eval_num; i++) {
		
		printf("===========================================================");
		
		int alg = evals[i]->alg;
		int preemptive = evals[i]->preemptive;
		
		switch (evals[i]->alg) {
		
		case FCFS:
            printf("<FCFS Algorithm>");
            break;
        case SJF:
        	if(preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	printf("SJF Algorithm>");
        	break;
        case RR:
        	printf("<Round Robin Algorithm>");
        	break;
        case PRIORITY:
        	if(preemptive) printf("<Preemptive ");
        	else printf("<Non-preemptive ");
        	printf("Priority Algorithm>");
        	break;
        default:
        return;
		}

		printf ("-----------------------------------------------------------");
		printf("start time: %d / end time: %d / CPU utilization : %.2lf%% \n",evals[i]->startTime,evals[i]->endTime,evals[i]->CPU_util);
		printf("Average waiting time: %d\n",evals[i]->avg_waitingTime);
		printf("Average turnaround time: %d\n",evals[i]->avg_turnaroundTime);
		printf("Average response time: %d\n",evals[i]->avg_responseTime);
		printf("Completed: %d\n",evals[i]->completed);
	}
	
	printf("===========================================================");
}


// Set total process number and total IO event number
void createProcesses(int total_num, int io_num) {
	if (io_num > total_num) {
		printf("<ERROR> The number of IO event cannot be higher than the number of processes");
		exit(-1);
	}
	
    // for random number generation
	srand(time(NULL)); 
	
	for (int i = 0; i < total_num; i++) {
		// CPU burst limit: 5-20
		// IO burst limit: 1-10
        // refer to createProcess(int pid, int priority, int arrivalTime, int CPUburst, int IOburst)
		createProcess(rand() % 9000 + 1000, rand() % total_num + 1, rand() % (total_num + 10), rand() % 16 + 5, 0);
	}
	
    // Which process incurs IO event? (random designation)
	for (int i = 0; i <io_num; i++) {
		 
		int randomIdx = rand() % total_num ;

		if(jobQueue[randomIdx]->IOburst == 0) {
		
			int randomIOburst = rand() % 10 + 1;
			jobQueue[randomIdx]->IOburst = randomIOburst;
			jobQueue[randomIdx]->IOremainingTime = randomIOburst;
		
		} 

        // Choose another process (that ith process already be chosen to incur IO event)
        else 
		    i--;
	}

	sort_JQ();
	clone_JQ(); //backup this JQ
	print_JQ();
}


// Main function (FINAL)
void main() {

    init_RQ();
    init_JQ();
    init_T();
    init_WQ();
    init_evals();
    
    // for random number generation
	srand(time(NULL)); 
    int totalProcessNum = 0, totalIOProcessNum = 0;

    while (totalProcessNum < 3) {
        totalProcessNum = rand() % MAX_PROCESS_NUM + 1;
        // IO process number should be smaller than total process number.
        totalIOProcessNum = rand() % totalProcessNum +1;
    }

    // Create process with above information
    createProcesses(totalProcessNum, totalIOProcessNum);
    
    // time limit 
    int amount = 120;
 	startSimulation(FCFS,FALSE,TIME_QUANTUM, amount);     // FCFS - default: non-preemptive
    startSimulation(SJF,FALSE,TIME_QUANTUM, amount);
    startSimulation(SJF,TRUE,TIME_QUANTUM, amount);
	startSimulation(PRIORITY,FALSE,TIME_QUANTUM, amount);
	startSimulation(PRIORITY,TRUE,TIME_QUANTUM, amount);
	startSimulation(RR,TRUE,TIME_QUANTUM, amount);
	
    evaluate();

	clear_JQ();
    clear_RQ();
    clear_T();
    clear_WQ();
    clearClone_JQ();
    
	clear_evals();
}