#include "opencv2/opencv.hpp"
#include <iostream>

static cv::String type2Str(int type) {
    cv::String r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch (depth) {
        case CV_8U:  r = "CV_8U"; break;
        case CV_8S:  r = "CV_8S"; break;
        case CV_16U: r = "CV_16U"; break;
        case CV_16S: r = "CV_16S"; break;
        case CV_32S: r = "CV_32S"; break;
        case CV_32F: r = "CV_32F"; break;
        case CV_64F: r = "CV_64F"; break;
        default:     r = "User"; break;
    }

    r += "C";
    r += (chans+'0');

    return r;
}

int main(int argc, char **argv) {
    const cv::String keys =
        "{ h help    |        | Display the usage                      }"
        "{ v verbose |        | Verbose mode                           }"
        "{ imshow    |        | Display an image in a window           }"
        "{ @filename | <none> | Name of the file to read the data from }";

    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("Loads data from a file storage.");

    if (parser.has("help")) {
        parser.printMessage();
        return EXIT_SUCCESS;
    }

    cv::String filename{ parser.get<cv::String>("@filename")};

    if (!parser.check()) {
        parser.printErrors();
        return EXIT_FAILURE;
    }

    cv::FileStorage fs(filename, cv::FileStorage::READ);
    
    if (!fs.isOpened()) {
        std::cerr << "[ERROR] Failed to open the file.\n";
    }

    cv::Mat mat;
    fs["mat"] >> mat;

    if (mat.empty()) {
        std::cout << "[WARN] fsread returns an empty matrix.\n";
    }

    if (parser.has("verbose")) {
        std::cout << "[INFO] Loaded the data:\n"
            << "rows = " << mat.rows << '\n'
            << "cols = " << mat.cols << '\n'
            << "channels = " << mat.channels() << '\n'
            << "type = " << type2Str(mat.type()) << '\n'
            << "isContinous = " << (mat.isContinuous() ? "true" : "false") << '\n';
    }

    if (!mat.empty() && parser.has("imshow")) {
        cv::imshow(filename.c_str(), mat);
        cv::waitKey();
    }

    return EXIT_SUCCESS;
}
