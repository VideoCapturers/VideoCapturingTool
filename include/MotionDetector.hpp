#include <opencv2/core/core.hpp>
#include <opencv2/videoio.hpp>

#include <string>

class MotionDetector
{
public:
    explicit MotionDetector(std::string inputFile, int threshold = 8, bool showAll = false);
    void run(int prerecord, std::string outputDir="../output/"); // NOTE: outputDir with '/' at the end
    ~MotionDetector();

private:
    void processImage(const cv::Mat& frame);
    bool somethingHasMoved() const;

    int threshold_;
    bool showAll_; // Either or not show 2 windows and logs in the console
    cv::VideoCapture capture_;
    cv::Size frameSize_;
    int pixels_; // Number of pixels in a frame
    cv::Mat grayFrame1_;
    cv::Mat grayFrame2_;
    cv::Mat result_;
};