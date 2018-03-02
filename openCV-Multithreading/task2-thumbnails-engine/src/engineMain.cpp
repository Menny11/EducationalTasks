/**
 * @brief This program creates thumbnails/resize images using multithreading
 *  and puts them in thumbnail folder in the pictures folder.
 *  Size can be specified. For resizing of the images
 * threadpool was implemented.
 *
 * Author Mykola Fedorenko, email: stnikolay@ukr.net
 *
 */

#include <iostream>
#include <experimental/filesystem>
#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>

#include <imgthreadpool.h>
#include <exectimerclass.h>

void imgPartResize(const cv::Mat inImg, cv::Mat outImg)
{
    cv::resize(inImg, outImg, cv::Size(outImg.cols, outImg.rows));
}

void imageResize(const cv::Mat& inImg,
                 cv::Mat& outImg,
                 const cv::Size& outSize,
                 ThreadPool* poolPtr)
{
    int taskNum = poolPtr->getThreadsNum()*2;
    if (taskNum==1){
        cv::resize(inImg, outImg, outSize);
        return;
    }
    cv::Mat resImg(outSize.height, outSize.width, inImg.type());
    std::vector<std::future<void>> resResults(taskNum-1); //-1 for this thread

    int inStep=inImg.rows/taskNum;
    int resStep=outSize.height/taskNum;
    int inRowBeg=0, inRowEnd=inStep;
    int resRowBeg=0, resRowEnd=resStep;
    for (int i=0; i<taskNum-1; ++i){
        resResults[i] = poolPtr->submit(imgPartResize,
                                        inImg.rowRange(inRowBeg, inRowEnd),
                                        resImg.rowRange(resRowBeg, resRowEnd));
        inRowBeg=inRowEnd;
        inRowEnd+=inStep;
        resRowBeg=resRowEnd;
        resRowEnd+=resStep;
    }
    // also do part of the work in this thread
    imgPartResize(inImg.rowRange(inRowBeg, inImg.rows),
                  resImg.rowRange(resRowBeg, outSize.height));

    // wait for future and do some tasks until it will be ready
    for (int i = 0; i < resResults.size(); ++i){
        while (resResults[i].wait_for(std::chrono::seconds(0))==
                                      std::future_status::timeout){
            poolPtr->runPoolTask();
        }
        resResults[i].get();
    }
    outImg=std::move(resImg);
}

int main(int argc, char* argv[])
{
    if (argc!=4){
        std::cout << "Input should be: \n"
                  << "<Program> <Folder> <ImgWidth> <ImgHeight>"<<std::endl;
        return 1;
    }
    std::string prgFolder=argv[0];
    prgFolder=prgFolder.substr(0, prgFolder.rfind('/'));
    std::string imgFolder=argv[1];
    int imWidth=std::stoi(argv[2]);
    int imHeigt=std::stoi(argv[3]);
    if (imWidth<1 || imWidth>10000000 || imHeigt<1 || imHeigt>10000000){
        std::cout << "Improper size values" << std::endl;
        return 1;
    }

    ExTimer<std::chrono::milliseconds> aTimer("Program execution time: ");

//    int imWidth=640, imHeigt=360;
//    std::string imgFolder="../img";

    std::cout<< "Loading images please wait.."<<std::endl;
    std::vector<cv::String> initNames;
    std::vector<std::string> imgNames;
    cv::glob(imgFolder, initNames, false);

    ThreadPool imPool;
    std::vector<cv::Mat> imgsToResize;
    std::vector<cv::Mat> imgsResized;
    std::vector<std::future<void>> imgsFuture;
    for (auto& iFile: initNames){
        cv::Mat curImg=imread(iFile, cv::IMREAD_UNCHANGED);
        if (curImg.empty())
            continue;
        std::cout << "Loaded " << iFile << std::endl;
        imgNames.push_back(iFile);

        cv::Mat outImg(imHeigt, imWidth, curImg.type());
        imgsToResize.push_back(std::move(curImg));
        imgsResized.push_back(std::move(outImg));
        auto curFut=imPool.submit(imageResize,
                                  std::cref(imgsToResize.back()),
                                  std::ref(imgsResized.back()),
                                  cv::Size(imWidth, imHeigt),
                                  &imPool);
        imgsFuture.push_back(std::move(curFut));
    }

    if (imgNames.empty()){
        std::cout<<"There is no images!.."<<std::endl;
        return 1;
    }
    // create directory for resized
    std::string outPath=imgFolder+"/thumbnails";
    if(boost::filesystem::create_directory(outPath))
        std::cout << "Directory" << outPath << " created!"<<std::endl;
    else {
        if (!boost::filesystem::exists(outPath))
            std::cout << "Unable to create direcrory!\n" << std::endl;
    }
    // get all of the resized pictures
    for (auto& fut: imgsFuture)
        fut.get();

    for (int i=0; i<imgsResized.size(); ++i){
        cv::imwrite(outPath + imgNames[i].substr(imgNames[i].rfind('/')),
                    imgsResized[i]);
    }
    return 0;
}
