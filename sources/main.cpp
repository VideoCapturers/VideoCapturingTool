#include <MotionDetector.hpp>

int main()
{
    MotionDetector detector("/dev/video0", 8, true);
    detector.run(5); // Prerecord is 5 sec
}
