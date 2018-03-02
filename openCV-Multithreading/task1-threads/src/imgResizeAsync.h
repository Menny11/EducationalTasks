/**
 * @brief Header file for a function which implements 
 * resizing of the image using opencv, std::async and std::future
 * 
 * Author Mykola Fedorenko, email: stnikolay@ukr.net
 * 
 */

#ifndef IMG_RESIZE_ASYNC_H
#define IMG_RESIZE_ASYNC_H

#include <opencv2/opencv.hpp>

/**
 * @brief imageResizeAsync Resizes images asynchronously using specified
 * number of async tasks/treads (using std::async and std::future)
 * @param inImg Initial image to resize.
 * @param outImg Resized image to output.
 * @param outSize Size to which initial image should be resized.
 * @param taskNum Number of async tasks/threads to use.
 */
void imageResizeAsync(const cv::Mat& inImg,
                     cv::Mat& outImg,
                     const cv::Size& outSize,
                     int taskNum);

# endif // IMG_RESIZE_ASYNC_H
