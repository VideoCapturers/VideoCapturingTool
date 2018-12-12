#include <MotionDetector.hpp>

#include <opencv2/highgui.hpp>
#include <opencv2/video/tracking.hpp>

#include <list>
#include <algorithm>
#include <iostream>
#include <ctime>

MotionDetector::MotionDetector(int threshold, bool showWindows)
    : threshold_(threshold), showWindows_(showWindows)
{
    capture_ = cv::VideoCapture(0);

    if (!capture_.isOpened())
    {
        std::cerr << "Can't open the video.\n";
        return;
    }

    cv::Mat frame;

    if (!capture_.read(frame))
    {
        std::cout << "Can't read the video.\n";
        return;
    }

    frameSize_ = frame.size();
    pixels_ = frameSize_.width * frameSize_.height;
    grayFrame1_ = cv::Mat(frameSize_, CV_8U); // Gray frame at t - 1
    cv::cvtColor(frame, grayFrame1_, cv::COLOR_RGB2GRAY); // Convert grayFrame1_ from one color space to another
    grayFrame2_ = cv::Mat(frameSize_, CV_8U); // Gray frame at t
    result_ = cv::Mat(frameSize_, CV_8U); // Will hold the thresholded result

    if (showWindows_)
    {
        cv::namedWindow("Video");
        cv::namedWindow("Result");
        cv::createTrackbar("Detection threshold: ", "Video", &threshold_, 100);
    }
}

void MotionDetector::run(int prerecord, std::string outputDir)
{
    bool isRecording = false;
    std::list<cv::Mat> cachedFrames; // Frames of prerecord / postrecord
    time_t currentTime = 0;
    time_t afterMoving = 0; // Time elapsed after last moving
    time_t startedTime = time(nullptr);
    time_t timeWithPrerecord = startedTime + prerecord;
    cv::VideoWriter videoWriter;

    while (true)
    {
        cv::Mat currentFrame;
        int64 start = cv::getTickCount();

        if (!capture_.read(currentFrame))
            return;

        processImage(currentFrame);
        //currentTime = startedTime + static_cast<int>(capture_.get(cv::CAP_PROP_POS_MSEC) / 1000);
        currentTime = time(nullptr);
        std::string text(ctime(&currentTime));
        text.pop_back(); // Remove '\n' for correct information display
        cv::putText(currentFrame, text, cv::Point(25, 30), cv::FONT_HERSHEY_SIMPLEX, 1, 
                    cv::Scalar(0, 255, 0)); // Green text (colors are BGR)
        cachedFrames.push_back(currentFrame);

        if (currentTime > timeWithPrerecord)
            cachedFrames.pop_front();

        if (!isRecording)
        {
            if (somethingHasMoved())
            {
                isRecording = true;
                std::cout << text << " : moving detected" << std::endl;
                std::replace(text.begin(), text.end(), ':', '-'); // File name correction
                videoWriter = cv::VideoWriter(outputDir + text + ".avi",
                                              static_cast<int>(capture_.get(cv::CAP_PROP_FOURCC)),
                                              static_cast<int>(capture_.get(cv::CAP_PROP_FPS) / 2),
                                              frameSize_);

                for (const auto& frame : cachedFrames)
                    videoWriter.write(frame);
            }
        }
        else
        {
            videoWriter.write(currentFrame);

            if (somethingHasMoved())
                afterMoving = 0;
            else if (!afterMoving)
                afterMoving = currentTime + prerecord;
            else if (currentTime > afterMoving)
            {
                isRecording = false;
                videoWriter.release();
                afterMoving = 0;
                std::cout << text << ": stop recording" << std::endl;
            }
        }

        if (showWindows_)
        {
            cv::imshow("Video", currentFrame);
            cv::imshow("Result", result_);
        }

        if (cv::waitKey(1) == 27) // Break if user enters 'Esc'
        {
            if (videoWriter.isOpened())
                videoWriter.release();

            break;
        }
        
        std::cout << "FPS : " << cv::getTickFrequency() / (cv::getTickCount() - start) << std::endl;
    }
}

MotionDetector::~MotionDetector()
{
    capture_.release();
}

void MotionDetector::processImage(const cv::Mat& frame)
{
    cv::cvtColor(frame, grayFrame2_, cv::COLOR_RGB2GRAY);
    cv::absdiff(grayFrame1_, grayFrame2_, result_);

    // Remove the noise and do the threshold
    cv::blur(result_, result_, cv::Size(5, 5));
    cv::morphologyEx(result_, result_, cv::MORPH_OPEN,
                     cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * 5 + 1, 2 * 5 + 1), // 2 * morphSize + 1
                     cv::Point(5, 5))); // morphSize = 5
    cv::morphologyEx(result_, result_, cv::MORPH_CLOSE,
                     cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2 * 0 + 1, 2 * 0 + 1), // 2 * morphSize + 1
                     cv::Point(0, 0))); // morphSize = 0
    cv::threshold(result_, result_, 10, 255, cv::THRESH_BINARY_INV);

    grayFrame2_.copyTo(grayFrame1_);
}

bool MotionDetector::somethingHasMoved() const
{
    int blackPixels = 0; // The number of black pixels

    for (int x = 0; x < frameSize_.height; ++x)
        for (int y = 0; y < frameSize_.width; ++y)
            if (result_.at<uchar>(x, y) == 0)
                blackPixels++;

    double avg = (blackPixels * 100.0) / pixels_; // Calculate the average of black pixels in the image
    return avg > threshold_;
}