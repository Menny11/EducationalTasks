/**
 * @brief This main file provides testing of the image resize
 * functions defined in the imgResizeAsync and imgResizeThreads
 * 
 * Author Mykola Fedorenko, email: stnikolay@ukr.net
 * 
 * Implementation using std::thread  (2 threads) 1.5-1.7 faster
 * than single thread on my core2duo e8500(3.1GHz)(~6ms=1 thread, ~4ms=2).
 * For 12 threads results almost the same. Tested on 8k picture.
 *
 * Async implementation is sligtly faster than std::thread (5-10%)
 */
#include <iostream>
#include <thread>

#include "imgResizeAsync.h"
#include "imgResizeThreads.h"
#include "exectimerclass.h"

int main(int argc, char* argv[])
{
    int nThreads=0;
    if (argc == 4){
        nThreads=std::thread::hardware_concurrency();
       if (nThreads==0)
           nThreads=2; //in case info unavailable?!
    } else if(argc == 5){
        nThreads=std::stoi(argv[4]);
    }
    else {
        std::cout << "Input should be:\n"
                  << "<Program> <imgfile> <width> <height> [threads number]\n"
                  << "Output image will be placed in the same dir of original."
                  << std::endl;
        return 1;
    }
    std::string imgFileName=argv[1];
    int imWidth=std::stoi(argv[2]);
    int imHeight=std::stoi(argv[3]);

//    int imWidth=640, imHeight=360;
//    int nThreads=2;
//    std::string imgFileName="../the_image2.jpg";

    std::string outImgName=imgFileName.substr(0, imgFileName.rfind('.'));
    cv::Mat inImage=cv::imread(imgFileName, cv::IMREAD_UNCHANGED);
    cv::Mat outImage;

    // single thread resizing image
    {
        ExTimer<> sinTime("Single thread resizing time: ");
        cv::resize(inImage, outImage, cv::Size(imWidth, imHeight));
    }
    cv::imwrite(outImgName+"_small.jpg", outImage);

    // multithreaded resizing using std::threads
    {
        ExTimer<> stdTrTimer("std::thread resizing time: ");
        imageResizeThreads(inImage,
                           outImage,
                           cv::Size(imWidth, imHeight),
                           nThreads);
    }
    cv::imwrite(outImgName+"_small_threads.jpg", outImage);

    // multithreaded resizing using std::async and std::future
    {
        ExTimer<> asyncTimer("std::async resizing time: ");
        imageResizeAsync(inImage,
                         outImage,
                         cv::Size(imWidth, imHeight),
                         nThreads);
    }
    cv::imwrite(outImgName+"_small_async.jpg", outImage);
    return 0;
}
