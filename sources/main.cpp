#include <MotionDetector.hpp>

int main()
{
    MotionDetector detector(8, true);
    detector.run(5); // Prerecord is 5 sec
}