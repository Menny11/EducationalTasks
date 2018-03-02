#ifndef EXECTIMERCLASS_H
#define EXECTIMERCLASS_H

#include <chrono>
#include <string>

template<typename CLKResolution=std::chrono::microseconds>
class ExTimer
{
public:
    ExTimer(std::string mes="Total execution time: "):mMes(mes){
        sTime=std::chrono::high_resolution_clock::now();
    }
    ~ExTimer(){
        auto eTime=std::chrono::high_resolution_clock::now();
        auto durTime=
            std::chrono::duration_cast<CLKResolution>(eTime-sTime);
        std::string resMes;
        if (typeid(CLKResolution)==typeid(std::chrono::microseconds)){
            resMes="mks.";
        } else if (typeid(CLKResolution)==typeid(std::chrono::milliseconds)){
            resMes="ms.";
        } else if(typeid(CLKResolution)==typeid(std::chrono::nanoseconds)){
            resMes="ns.";
        } else if(typeid(CLKResolution)==typeid(std::chrono::seconds)){
            resMes="s.";
        } else if(typeid(CLKResolution)==typeid(std::chrono::minutes)){
            resMes="m.";
        } else {
            resMes="h.";
        }
        std::cout<< mMes<< durTime.count()<<resMes<< std::endl;
    }
private:
    std::chrono::high_resolution_clock::time_point sTime;
    std::string mMes;
};

#endif // EXECTIMERCLASS_H
