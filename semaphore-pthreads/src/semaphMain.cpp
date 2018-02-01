/**
 * Program for Global Logic Basecamp
 * Author: Mykola Fedorenko, email: stnikolay@ukr.net
 * 
 *
 * Program implements a semaphore for control of predefined number of threads, 
 * created using pthread library (POSIX threads).
 * 
 */

#include "semaphMain.h"
#include <pthread.h>
#include <semaphore.h>

#include <random>
#include <algorithm>
#include <iostream>
#include <string>
#include <chrono>

// RAII class for managing a semaphore
class SemaphoreHandler
{
public:
    explicit
    SemaphoreHandler(sem_t *semGlobal) : mSem(semGlobal) { sem_wait(mSem); }
    ~SemaphoreHandler() { sem_post(mSem); }
private:
    sem_t *mSem;
};

struct ThreadArgument
{
    int id;
    int arrSize;
};
std::vector<ThreadArgument> threadArg;

sem_t workSemaphore;

void randFillArray(std::vector<int> &array)
{
    // setting random engine
    //range for random numbers
    int lowest = -1000000, biggest = 1000000;
    std::uniform_int_distribution<int> distrRnd(lowest, biggest);
    std::default_random_engine genRnd;

    for (int i = 0, arrSize = array.size(); i < arrSize; ++i){
        array[i] = distrRnd(genRnd);
    }
}

void processArray(std::vector<int> &array)
{
    std::sort(std::begin(array), std::end(array));
}

void *workingRoutine(void *threadArg)
{
    ThreadArgument* thrArg = (ThreadArgument *)threadArg;

    std::string message;
    message = "Thread # " + std::to_string(thrArg->id) + " started!\n";
    message += "Size of array: " + std::to_string(thrArg->arrSize) + ".";
    std::cout << message<<std::endl;

    std::vector<int> arr{thrArg->arrSize};

    {
        int semVal=0;
        sem_getvalue(&workSemaphore, &semVal);
        message = "Sem_val (before wait)= " + std::to_string(semVal) + ".";
        std::cout << message << std::endl;
        
        // RAII semaphore handling object
        SemaphoreHandler semHandler{&workSemaphore};
        //sem_wait(&workSemaphore);
        
        //lets fill array with some random numbers
        randFillArray(arr);

        // perform some work on array
        processArray(arr);

        sem_getvalue(&workSemaphore, &semVal);
        message = "Sem_val (before post)= " + std::to_string(semVal) + ".";
        std::cout << message << std::endl;

        //sem_post(&workSemaphore);
    }

    message = "Thread # " + std::to_string(thrArg->id) + " ended!";
    std::cout << message << std::endl;

    pthread_exit(NULL);
    //return 0;
}


int main(int argc, char *argv[])
{
    //lets measure the time of execution
    auto startTime = std::chrono::high_resolution_clock::now();

    // this code should parse input params

    // for now lets hardcode some numbers
    int coresNum = 2, workingThreadsNum = coresNum;
    int arrNum = 20, threadsNum=arrNum;
    int arrSize = 100000000;
    threadArg.reserve(threadsNum);
    // setting semaphore value equal to the number of cores
    sem_init(&workSemaphore, 0, workingThreadsNum);

    // setting threads attributes
    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_JOINABLE);

    // creating threads
    std::vector<pthread_t> threads{threadsNum};
    //std::vector<ThreadArgument> threadArg{threadsNum, {0, arrSize}};
    
    for (int i = 0, errCode; i < threadsNum; ++i){
        //threadArg[i].id=i;
        threadArg.push_back({i, arrSize});
        errCode = pthread_create(&threads[i], &threadAttr,
                                 workingRoutine, (void *)(&threadArg[i]));
        if (errCode){
            std::cout << "Cannot create thread # " << i
                      << ", error # " << errCode << ".\n"
                      << "Execution will be terminated!"
                      << std::endl;
            exit(2);
        }
    }

    // joining threads
    for (int i = 0; i < threadsNum; ++i){
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&workSemaphore);
    pthread_attr_destroy(&threadAttr);

    //program duration calculation
    auto endTime = std::chrono::high_resolution_clock::now();
    auto programDuration =
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Work is done, execution time: " << programDuration.count()
              << " ms." << std::endl;

    pthread_exit(NULL);
    //return 0;
}
