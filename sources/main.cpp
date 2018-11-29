#include <MotionDetector.hpp>

int main()
{
    MotionDetector detector("../resources/1.MP4");
    detector.run(5); // Prerecord is 5 sec
}
