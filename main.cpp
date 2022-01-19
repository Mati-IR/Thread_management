#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <mutex>
#include <semaphore.h>
#include <synchapi.h>
#include <stdlib.h>
#include <processthreadsapi.h>




#define MIN_THREAD 3
#define MAX_THREAD 100

int semaphoreChangeValue;//Value used to change value of semaphore after closing thread
sem_t threadClosingSynchronization;//
std::mutex allowClosingThreads;
std::mutex allowNewThreadCreation;

void semaphoreChange();//changing value of semaphore by 1 or -1 according to main() arguments
int test(int argc, char *argv[] );//input validation
int semaphoreInitValue(char *argv1, char *argv2);//initialize semaphore value according to main() arguments
DWORD WINAPI threadRoutine(LPVOID tID);

using std::cout;

int main(int argc, char *argv[])
{
    if (test(argc, argv) == -1 )
    {
        printf("Incorrect input data\n");
        exit(EXIT_FAILURE);
    }
    HANDLE threadsHandle[atoi(argv[1])];


    sem_init(&threadClosingSynchronization,
             0,//used within one process
             semaphoreInitValue(argv[1], argv[2]));

    int semValue;// variable for presentation purposes
    sem_getvalue(&threadClosingSynchronization, &semValue);
    cout << "Semaphore value: " << semValue << "\n";
    cout << "argv[1] = " << argv[1] << "\n\n";
    if( allowNewThreadCreation.try_lock() == 0 || allowClosingThreads.try_lock() == 0)
    {
        cout << "Could not lock semaphore\n";
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < atoi(argv[1]); i++)
    {
        threadsHandle[i] = CreateThread(NULL,
                     1024,//  stack size
                     threadRoutine,//   function to run by thread
                     &i,//              argument passed to thread
                     0, // runs immediately after creation, more flags on https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createthread
                     NULL);

        do{} while (allowNewThreadCreation.try_lock() == 0);
        /*wait until thread is done creating
          implemented it because WaitForSingleObject() was insufficient
          in extreme cases, like 100 threads
         */
    }

    allowClosingThreads.unlock();
    if (semaphoreChangeValue == -1)
        WaitForSingleObject(threadsHandle[0], atoi(argv[1]) * 3000);
    else if (semaphoreChangeValue == 1)
        WaitForSingleObject(threadsHandle[ atoi(argv[1]) - 1 ], atoi(argv[1]) * 3000);
    exit(EXIT_SUCCESS);
}

int test(int argc, char *argv[] )
{
    if (argc != 3)
        return -1;

    for (int i = 0; i < strlen(argv[1]); i++)
    {
        if (argv[1][i] < '0' || argv[1][i] > '9')
            return -1;
    }

    char *a = argv[1];
    int nOfThreads = atoi(a);

    if (nOfThreads > MAX_THREAD || nOfThreads < MIN_THREAD || strlen(argv[1]) > 3
        || (strcmp(argv[2], "dec") != 0 &&  strcmp(argv[2], "inc") != 0))
        return -1;

    return 0;
}

DWORD threadRoutine WINAPI(LPVOID tID)
{
    int threadID = *static_cast<int*>(tID);//thread id which is "i" from for loop in main()
    cout << "Thread no. " << threadID << " started\nwaiting for semaphore...\n";
    int semValue;

    allowNewThreadCreation.unlock();
    do
    {
        sem_getvalue(&threadClosingSynchronization, &semValue);
    } while (semValue != threadID);

    cout << "Thread " << threadID << " Waiting for mutex\n";
    do{}
    while (allowClosingThreads.try_lock() == 0);


    cout << "Shutting down thread no. " << threadID << "\n";
    semaphoreChange();
    allowClosingThreads.unlock();
    return 0;
}

int semaphoreInitValue(char *argv1, char *argv2)
{
    if (strcmp(argv2, "inc") == 0) {
        semaphoreChangeValue = 1;
        return 0;//id of first opened thread
    }
    else if(strcmp(argv2, "dec") == 0) {
        semaphoreChangeValue = -1;
        return atoi(argv1) - 1;// id of last opened thread
    }

    return -1;
}
void semaphoreChange(){
    if (semaphoreChangeValue == 1)
        sem_post(&threadClosingSynchronization);
    else if (semaphoreChangeValue == -1)
        sem_trywait(&threadClosingSynchronization);
    else {
        cout << "Incorrect semaphoreChangeValue\nsemaphoreChangeValue = " << semaphoreChangeValue;
        exit(EXIT_FAILURE);
    }
}