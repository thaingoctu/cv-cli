#include "opencv2/opencv.hpp"
#include <iostream>

int main(int argc, char **argv) {
    const cv::String keys{
        "{ h help | | Display the usage }"
    };

    cv::CommandLineParser parser{ argc, argv, keys };
    parser.about("Gets the OpenCV library version string.");

    if (parser.has("help")) {
        parser.printMessage();
        return EXIT_SUCCESS;
    }

    std::cout << cv::getVersionString() << '\n';
    return EXIT_SUCCESS;
}
