#include "opencv2/opencv.hpp"
#include "helpers.hpp"
#include <iostream>

constexpr auto INVALID_FLAG = (-1);

static void printFlags() {
    std::cout << "Possible flags:\n"
        << "\tunchanged\n"
        << "\tgrayscale\n"
        << "\tcolor\n"
        << "\tanydepth\n"
        << "\tanycolor\n"
        << "\tload_gdal\n"
        << "\treduced_grayscale_2\n"
        << "\treduced_grayscale_4\n"
        << "\treduced_grayscale_8\n"
        << "\treduced_color_2\n"
        << "\treduced_color_4\n"
        << "\treduced_color_8\n"
        << "\tignore_orientation\n";
}

static int getImreadMode(const cv::String &flags) {
    if ("unchanged" == flags) {
        return cv::ImreadModes::IMREAD_UNCHANGED;
    } else if ("grayscale" == flags) {
        return cv::ImreadModes::IMREAD_GRAYSCALE;
    } else if ("color" == flags) {
        return cv::ImreadModes::IMREAD_COLOR;
    } else if ("anydepth" == flags) {
        return cv::ImreadModes::IMREAD_ANYDEPTH;
    } else if ("anycolor" == flags) {
        return cv::ImreadModes::IMREAD_ANYCOLOR;
    } else if ("load_gdal" == flags) {
        return cv::ImreadModes::IMREAD_LOAD_GDAL;
    } else if ("reduced_grayscale_2" == flags) {
        return cv::ImreadModes::IMREAD_REDUCED_GRAYSCALE_2;
    } else if ("reduced_grayscale_4" == flags) {
        return cv::ImreadModes::IMREAD_REDUCED_GRAYSCALE_4;
    } else if ("reduced_grayscale_8" == flags) {
        return cv::ImreadModes::IMREAD_REDUCED_GRAYSCALE_8;
    } else if ("reduced_color_2" == flags) {
        return cv::ImreadModes::IMREAD_REDUCED_COLOR_2;
    } else if ("reduced_color_4" == flags) {
        return cv::ImreadModes::IMREAD_REDUCED_COLOR_4;
    } else if ("reduced_color_8" == flags) {
        return cv::ImreadModes::IMREAD_REDUCED_COLOR_8;
    } else if ("ignore_orientation" == flags) {
        return cv::ImreadModes::IMREAD_IGNORE_ORIENTATION;
    } else {
        return INVALID_FLAG;
    }
}

int main(int argc, char **argv) {
    const cv::String keys = 
        "{ h help      |        | Display the usage                              }"
        "{ v verbose   |        | Verbose mode                                   }"
        "{ imshow      |        | Display an image in a window                   }"
        "{ filestorage |        | Write data to the specified XML/YAML/JSON file }"
        "{ @filename   | <none> | Name of file to be loaded                      }"
        "{ flags       | color  | Mode of imread                                 }";

    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("Loads an image from a file.");

    if (parser.has("help")) {
        parser.printMessage();
        printFlags();
        return EXIT_SUCCESS;
    }

    cv::String filename{ parser.get<cv::String>("@filename") };
    cv::String flags{ parser.get<cv::String>("flags") };
    cv::String filestorage{ parser.get<cv::String>("filestorage") };

    if (!parser.check()) {
        parser.printErrors();
        return EXIT_FAILURE;
    }

    int imread_mode = getImreadMode(flags);

    if (INVALID_FLAG == imread_mode) {
        std::cerr << "[ERROR] Invalid flags.\n";
        return EXIT_FAILURE;
    }

    cv::Mat image = cv::imread(filename, imread_mode);

    if (image.empty()) {
        std::cout << "[WARN] imread returns an empty matrix.\n";
    }

    if (parser.has("filestorage")) {
        cv::FileStorage fs(filestorage, cv::FileStorage::WRITE);
        fs << "mat" << image;
    }

    if (parser.has("verbose")) {
        std::cout << "[INFO] Loaded the image:\n"
            << "rows = " << image.rows << '\n'
            << "cols = " << image.cols << '\n'
            << "channels = " << image.channels() << '\n'
            << "type = " << type2Str(image.type()) << '\n'
            << "isContinous = " << (image.isContinuous() ? "true" : "false") << '\n';
    }

    if (!image.empty() && parser.has("imshow")) {
        cv::imshow(filename.c_str(), image);
        cv::waitKey();
    }

    return EXIT_SUCCESS;
}
