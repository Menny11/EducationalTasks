/**
 * Author: Mykola Fedorenko, email: stnikolay@ukr.net
 *
 * Program implements a semaphore for control of predefined number of threads, 
 * created using pthread library (POSIX threads).
 * Semaphore is set to the current available number of cores.
 * Predefined number of threads perform sorting of arrays and
 * at the same time can work only number of thread equal 
 * to the number of cores. To make this happen semaphore is used.
 * 
 */

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include <random>
#include <algorithm>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>

class ExTimer
{
  public:
    ExTimer(std::string mes = "Execution time: ") : mMes(mes)
    {
        sTime = std::chrono::high_resolution_clock::now();
    }
    ~ExTimer()
    {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto durTime =
            std::chrono::duration_cast<std::chrono::milliseconds>(endTime - sTime);
        std::cout << mMes << durTime.count() << "ms." << std::endl;
    }

  private:
    std::chrono::high_resolution_clock::time_point sTime;
    std::string mMes;
};

// RAII class for managing a semaphore
class SemaphoreHandler
{
  public:
    explicit SemaphoreHandler(sem_t *semGlobal) : mSem(semGlobal) { sem_wait(mSem); }
    ~SemaphoreHandler() { sem_post(mSem); }

  private:
    sem_t *mSem;
};

int getCoresNumber()
{
    int res = sysconf(_SC_NPROCESSORS_ONLN);
    if (res == 0)
        res = 2; // info not available
    return res;
}

sem_t sortSem;

void *sortArraysMt(void *argm)
{
    std::vector<int> *arr = (std::vector<int> *)argm;
    {
        SemaphoreHandler semManager(&sortSem);
        // printf("Some thread started sorting.\n");
        std::sort(arr->begin(), arr->end());
    }
    return nullptr;
}

void randFillArrays(std::vector<std::vector<int>> &arrs)
{
    // setting random engine, range for random numbers
    int lowest = -10000, biggest = 10000;
    std::uniform_int_distribution<int> distrRnd(lowest, biggest);
    std::default_random_engine genRnd;
    for (auto &arr : arrs)
        for (auto &el : arr)
            el = distrRnd(genRnd);
}

int main(int argc, char *argv[])
{

    if (argc != 3){
        std::cout << "Input should be:"
                  << " <Program> <Arrays number> <Arrays size>" << std::endl;
        return 1;
    }
    int nArrays = std::stoi(argv[1]), nThreads = nArrays;
    int nElems = std::stoi(argv[2]);
    if (nArrays < 1 || nArrays > 2000 || nElems < 1 || nElems > 500000000){
        std::cout << "Bad input values" << std::endl;
        return 1;
    }
    int nCores = getCoresNumber();
    // int nElems = 50000, nCores = 4;
    // int nArrays = 50, nThreads = nArrays;
    std::vector<std::vector<int>> arraysToSort;
    try{
        std::vector<std::vector<int>> arr(nArrays, std::vector<int>(nElems));
        arraysToSort = std::move(arr);
    }
    catch (...){
        std::cout << "Error, cannot allocate memory for arrays!\n"
                  << "Program terminated!" << std::endl;
        return 1;
    }
    // sorting in single thread
    randFillArrays(arraysToSort);
    {
        ExTimer singleTimer("Single thread sorting time: ");
        for (auto &arr : arraysToSort)
            std::sort(std::begin(arr), std::end(arr));
    }
    // sorting in multithread with semaphore
    if (nCores == 1){
        std::cout << "Available only 1 core!" << std::endl;
        return 0;
    }
    randFillArrays(arraysToSort);

    ExTimer multiTimer("Multithread sorting time: ");
    sem_init(&sortSem, 0, nCores);
    // pthread_attr_t thrAttr;
    // pthread_attr_init(&thrAttr);
    // pthread_attr_setdetachstate(&thrAttr, PTHREAD_CREATE_JOINABLE);
    std::vector<pthread_t> thrObj(nThreads - 1);
    // printf is faster than cout, that compensates its' thread unsafeness
    for (int i = 0; i < nThreads - 1; ++i){
        if (pthread_create(&thrObj[i], nullptr, sortArraysMt, &arraysToSort[i]))
        {
            printf("Error creating thread %i. Program terminated.\n", i);
            return 1;
        }
        // printf("Thread %i created.\n", i);
    }
    // one of the arrays will be sorted in the main thread
    sortArraysMt(&arraysToSort[nArrays - 1]);

    for (int i = 0; i < nThreads - 1; ++i){
        if (pthread_join(thrObj[i], nullptr)){
            printf("Error joining %i thread! Program terminated.\n", i);
            return 1;
        }
        // printf("Joined thread %i.\n", i);
    }
    // bool isAllSorted = true;
    // for (auto &arr : arraysToSort)
    //     isAllSorted &= std::is_sorted(std::begin(arr), std::end(arr));
    // printf(isAllSorted ? "All sorted!\n" : "Some not sorted!\n");
    sem_destroy(&sortSem);
    // pthread_attr_destroy(&thrAttr);
    return 0;
}
