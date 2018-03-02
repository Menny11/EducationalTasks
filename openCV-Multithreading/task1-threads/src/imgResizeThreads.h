/**
 * @brief Program imlement resize of the image using std::thread
 * 
 * Author Mykola Fedorenko, email: stnikolay@ukr.net
 * 
 */

#ifndef IMG_RESIZE_THREADS_H
#define IMG_RESIZE_THREADS_H

#include <opencv2/opencv.hpp>
/**
 * @brief imageResizeThreads Resizes images using specified number of threads
 * @param inImg Initial image to resize.
 * @param outImg Resized image for output.
 * @param outSize Size to which initial image should be resized.
 * @param threadsNumber Number of threads to use.
 */
void imageResizeThreads(const cv::Mat &inImg,
                       cv::Mat &outImg,
                       const cv::Size& outSize,
                       int threadsNumber);


# endif // IMG_RESIZE_THREADS_H
