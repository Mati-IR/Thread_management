# Thread_management

Hi!
This program created as training of thread management first opens given number of threads, and using synchronization methods ( mutex and semaphore ) closes them in either inclining or declining order, 

run program with main arguments:
argv[1] - number of threads to be created between 3 - 100
argv[2] - either "inc" (incline) or "dec" (decline)

# example:
./main.exe 42 dec

42 threads will be created and closed in declining order 

Program utilizes 2 mutexes and 1 semaphore for synchronization of creating and closing the threads


c++ 20 is recommended, works on Windows 10
