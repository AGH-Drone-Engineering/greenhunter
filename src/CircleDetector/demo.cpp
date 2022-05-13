#include <iostream>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " FILENAME" << endl;
        return -1;
    }

    Mat img_raw = imread(argv[1], IMREAD_COLOR);

    imshow("Detector", img_raw);

    waitKey();

    return 0;
}
