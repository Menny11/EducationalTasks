#include <future>
#include <vector>
#include <iostream>

#include "imgResizeAsync.h"

void asyncResizeTask(const cv::Mat inImg, cv::Mat outImg){
    cv::resize(inImg, outImg, cv::Size(outImg.cols, outImg.rows));
}

void imageResizeAsync(const cv::Mat& inImg,
                     cv::Mat& outImg,
                     const cv::Size& outSize,
                     int taskNum)
{
    if (taskNum==1){
        cv::resize(inImg, outImg, outSize);
        return;
    }
    cv::Mat resImg(outSize.height, outSize.width, inImg.type());
    std::vector<std::future<void>> resizeResults(taskNum-1); //-1=main thread!

    int inStep=inImg.rows/taskNum;
    int resStep=outSize.height/taskNum;
    int inRowBeg=0, inRowEnd=inStep;
    int resRowBeg=0, resRowEnd=resStep;
    for (int i=0; i<taskNum-1; ++i){
        resizeResults[i]=std::async(asyncResizeTask,
                                    inImg.rowRange(inRowBeg, inRowEnd),
                                    resImg.rowRange(resRowBeg, resRowEnd));
        inRowBeg=inRowEnd;
        inRowEnd+=inStep;
        resRowBeg=resRowEnd;
        resRowEnd+=resStep;
    }
    // also do part of the work in the main thread
    asyncResizeTask(inImg.rowRange(inRowBeg, inImg.rows),
                    resImg.rowRange(resRowBeg, outSize.height));
    for (auto& resResult: resizeResults)
        resResult.get();
    outImg=std::move(resImg);
}
