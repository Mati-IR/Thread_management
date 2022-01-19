# Thread_management

run program with main arguments:
argv[1] - number of threads to be created between 3 - 100
argv[2] - either "inc" (incline) or "dec" (decline)

#example:
./main.exe 42 dec

argv[2] represents the order in which the threads are supposed to close

Program utilizes 2 mutexes and 1 semaphore for synchronization of creating and closing the threads
