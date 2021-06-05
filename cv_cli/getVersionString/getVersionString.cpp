#include "opencv2/opencv.hpp"
#include <iostream>

int main(int argc, char **argv) {
    const cv::String keys = 
        "{ h help | | Print the usage }";
    
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("Get the OpenCV library version string");

    if (!parser.check()) {
        parser.printErrors();
        return EXIT_FAILURE;
    }

    if (parser.has("help")) {
        parser.printMessage();
        return EXIT_SUCCESS;
    }

    std::cout << cv::getVersionString();
    return EXIT_SUCCESS;
}
