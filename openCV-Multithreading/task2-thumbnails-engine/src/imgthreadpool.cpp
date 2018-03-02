/**
 * @brief Thread pool class implementation.
 *
 * Author Mykola Fedorenko, email: stnikolay@ukr.net
 */
#include "imgthreadpool.h"
#include <iostream>

ThreadPool::ThreadPool(int thrNum)
    : mDone(false),
      mThreads(std::vector<std::thread>()),
      mThreadsJoiner(mThreads)
{
    if (thrNum==0 && (thrNum=std::thread::hardware_concurrency())==0){
        thrNum=2; // in case info unavailable
    }
    try {
        for (int i=0; i<thrNum; ++i){
            mThreads.push_back(std::thread(&ThreadPool::threadFunction, this));
        }
    } catch (...) {
        mDone=true;
        std::cout<< "Exception during creation of the threads!"<<std::endl;
        throw;
    }
}

ThreadPool::~ThreadPool()
{
    mDone=true;
    mCondition.notify_all();
}

void ThreadPool::threadFunction()
{
    // threads are executing this function
    FnType curTask;
    while(!mDone){

        std::unique_lock<std::mutex> lck(mMutex);
        mCondition.wait(lck, [=]{ return mDone || !mTasks.empty();});
        if (mDone)
            break;
        curTask=std::move(mTasks.front());
        mTasks.pop_front();
        lck.unlock();

        curTask();
    }
}

void ThreadPool::runPoolTask()
{
    FnType rTask;
    std::unique_lock<std::mutex> lck(mMutex);
    if (mDone){
        return;
    } else if (!mTasks.empty()){
        rTask=std::move(mTasks.front());
        mTasks.pop_front();
        lck.unlock();
        rTask();
    } else {
        lck.unlock();
        std::this_thread::yield();
    }
}
