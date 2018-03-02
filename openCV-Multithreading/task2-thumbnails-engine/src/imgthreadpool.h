/**
 * @brief Thread pool class header. Also wraper and RAII classes
 * are declared/defined here.
 *
 * Author Mykola Fedorenko, email: stnikolay@ukr.net
 */
#ifndef IMGTHREADPOOL_H
#define IMGTHREADPOOL_H


#include <thread>
#include <vector>
#include <future>
#include <atomic>
#include <queue>
#include <condition_variable>
#include <functional>
#include <type_traits>

/**
 * @brief The ThreadsGuard class Join threads during destruction
 * must be put in the rigth place in the pool members declarations
 * (during destruction threads must be destroyed after it)
 */
class ThreadsGuard
{
public:
    explicit
    ThreadsGuard(std::vector<std::thread>& vec):mThrVec(vec){}
    ThreadsGuard(const ThreadsGuard& tg)=delete;
    ThreadsGuard& operator=(const ThreadsGuard& tg)=delete;
    ~ThreadsGuard(){
        for (auto& thr: mThrVec)
            if (thr.joinable())
                thr.join();
    }
private:
    std::vector<std::thread>& mThrVec;
};

class FunctionWrapper;

class ThreadPool
{
public:
    using FnType=FunctionWrapper;
    explicit
    ThreadPool(int trNum=0);
    ~ThreadPool();
    ThreadPool(const ThreadPool& trp)=delete;
    ThreadPool& operator=(ThreadPool trp)=delete;

    template <typename Fn, typename ...Args>
    auto submit(Fn&& fn, Args&&... args)->
                std::future<typename std::result_of<Fn(Args...)>::type>
    {
        using ResType= typename std::result_of<Fn(Args...)>::type;

        auto taskPtr = std::make_shared<std::packaged_task<ResType()>>
                       (std::bind(std::forward<Fn>(fn),
                                  std::forward<Args>(args)...));
        std::future<ResType> res = taskPtr->get_future();

        if (mDone)
            throw std::logic_error("Submitting in a destructing pool!");
        {
            std::lock_guard<std::mutex> curLk(mMutex);
            mTasks.push_back([taskPtr]{ (*taskPtr)(); });
        }
        mCondition.notify_one();
        return res;
    }
    void runPoolTask();
    int getThreadsNum(){ return mThreads.size();}
private:
    std::atomic_bool mDone;
    std::vector<std::thread> mThreads;
    std::deque<FnType> mTasks;
//    std::vector<std::queue<FnType>> mTaskQueues;
    std::mutex mMutex;
    std::condition_variable mCondition;
    ThreadsGuard mThreadsJoiner;

    void threadFunction();

};


/**
 * @brief The FuncType class - type erasure class for tasks in the pool
 *  will contain std::packaged_task (because it movable only)
 */
class FunctionWrapper
{
    struct FuncWrapperAbs{
        virtual void callStored()=0;
        virtual ~FuncWrapperAbs(){}
    };
    template <typename TFunc>
    struct FuncWrapperImpl: FuncWrapperAbs{
        FuncWrapperImpl(TFunc&& func): mFunc(std::move(func)){}
        void callStored(){ mFunc();}
        TFunc mFunc;
    };
    std::unique_ptr<FuncWrapperAbs> implPtr=nullptr;
public:
    FunctionWrapper()=default;
    template<typename TFunc>
    FunctionWrapper(TFunc&& func)
        : implPtr(new FuncWrapperImpl<TFunc>(std::move(func))){}

    FunctionWrapper(const FunctionWrapper& fu)=delete;
    FunctionWrapper(FunctionWrapper& fu)=delete;
    FunctionWrapper& operator=(const FunctionWrapper& fu)=delete;
    FunctionWrapper(FunctionWrapper&& func): implPtr(std::move(func.implPtr)){}
    FunctionWrapper& operator=(FunctionWrapper&& func){
        implPtr=std::move(func.implPtr);
        return *this;
    }
    void operator()(){implPtr->callStored();}
};


#endif // IMGTHREADPOOL_H
