#include "opencv2/opencv.hpp"
#include "helpers.hpp"
#include <iostream>

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

    auto filename{ parser.get<cv::String>("@filename") };

    if (!parser.check()) {
        parser.printErrors();
        return EXIT_FAILURE;
    }

    cv::FileStorage fs(filename, cv::FileStorage::READ);

    if (!fs.isOpened()) {
        std::cerr << "[ERROR] Failed to open the file.\n";
        return EXIT_FAILURE;
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
