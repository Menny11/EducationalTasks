// header file for semaphMain

#ifndef SEMAPHMAIN_H
#define SEMAPHMAIN_H


#include <vector>

// array manipulation routines
void randFillArray(std::vector<int>& array);
void processArray(std::vector<int>& array);

/**
 * @brief Routine which will be executed by threads
 * 
 * @param arraySize Argument passed to thread
 * @return void* Execution status returned
 */
void* workingRoutine(void* arraySize);

#endif // SEMAPHMAIN_H