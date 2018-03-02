#include <thread>
#include <vector>

#include "imgResizeThreads.h"

class ThreadsGuard
{
public:
    explicit
    ThreadsGuard(std::vector<std::thread>& vec):mThrVec(vec){}
    ThreadsGuard(const ThreadsGuard& tg)=delete;
    ThreadsGuard& operator=(const ThreadsGuard& tg)=delete;
    ~ThreadsGuard(){
        for (auto& thr: mThrVec){
            if (thr.joinable()){
                thr.join();
            }
        }
    }
private:
    std::vector<std::thread>& mThrVec;
};

void threadResizeTask(cv::Mat inImg, cv::Mat outImg)
{
    cv::resize(inImg, outImg, cv::Size(outImg.cols, outImg.rows));
}

void imageResizeThreads(const cv::Mat& inImg,
                        cv::Mat& outImg,
                        const cv::Size& outSize,
                        int thrNum)
{
    if (thrNum==1){
        cv::resize(inImg, outImg, outSize);
        return;
    }
    cv::Mat resImg(outSize.height, outSize.width, inImg.type());
    std::vector<std::thread> threads(thrNum-1); //minus 1 for main thread!
    {
        ThreadsGuard threadsJoiner(threads);
        int inStep=inImg.rows/thrNum;
        int resStep=outSize.height/thrNum;
        int inRowBeg=0, inRowEnd=inStep;
        int resRowBeg=0, resRowEnd=resStep;
        for (auto& thr: threads){
            thr=std::thread(threadResizeTask,
                            inImg.rowRange(inRowBeg, inRowEnd),
                            resImg.rowRange(resRowBeg, resRowEnd));
            inRowBeg=inRowEnd;
            inRowEnd+=inStep;
            resRowBeg=resRowEnd;
            resRowEnd+=resStep;
        }
        // also do part of the work in the main thread
        threadResizeTask(inImg.rowRange(inRowBeg, inImg.rows),
                         resImg.rowRange(resRowBeg, outSize.height));
    }
    outImg=std::move(resImg);
}
