#include <opencv2/core/core.hpp>
#include <opencv2/videoio.hpp>

#include <string>

class MotionDetector
{
public:
    explicit MotionDetector(std::string fileName, int threshold = 8, bool showWindows = false);
    void run(int prerecord);
    ~MotionDetector();

private:
    void initRecorder(cv::VideoWriter& videoWriter, std::string fileName) const;
    void processImage(const cv::Mat& frame);
    bool somethingHasMoved() const;

    int threshold_;
    bool show_; // Either or not show 2 windows
    cv::VideoCapture capture_;
    cv::Size frameSize_;
    int pixels_; // Number of pixels in a frame
    cv::Mat grayFrame1_;
    cv::Mat grayFrame2_;
    cv::Mat result_;
};
