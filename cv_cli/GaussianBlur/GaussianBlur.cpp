#include "opencv2/opencv.hpp"
#include "helpers.hpp"
#include <iostream>

constexpr auto INVALID_BORDER_TYPE{ -1 };

static void printBorderTypes() {
    std::cout << "Supported border types:\n"
        << "\tdefault\t\treflect101\n"
        << "\tconstant\tiiiiii|abcdefgh|iiiiiii with some specified i\n"
        << "\treplicate\taaaaaa|abcdefgh|hhhhhhh\n"
        << "\treflect\t\tfedcba|abcdefgh|hgfedcb\n"
        << "\treflect101\tgfedcb|abcdefgh|gfedcba\n"
        << "\ttransparent\tuvwxyz|abcdefgh|ijklmno\n"
        << "\tisolated\tdo not look outside of ROI\n";
}

static int getBorderType(const cv::String &border_type) {
    if ("default" == border_type) {
        return cv::BorderTypes::BORDER_DEFAULT;
    } else if ("constant" == border_type) {
        return cv::BorderTypes::BORDER_CONSTANT;
    } else if ("replicate" == border_type) {
        return cv::BorderTypes::BORDER_REPLICATE;
    } else if ("reflect" == border_type) {
        return cv::BorderTypes::BORDER_REFLECT;
    } else if ("reflect101" == border_type) {
        return cv::BorderTypes::BORDER_REFLECT_101;
    } else if ("transparent" == border_type) {
        return cv::BorderTypes::BORDER_TRANSPARENT;
    } else if ("isolated" == border_type) {
        return cv::BorderTypes::BORDER_ISOLATED;
    } else {
        return INVALID_BORDER_TYPE;
    }
}

int main(int argc, char **argv) {
    const cv::String keys{
        "{ h help      |         | Display the usage                                 }"
        "{ v verbose   |         | Verbose mode                                      }"
        "{ imshow      |         | Display an image in a window                      }"
        "{ @filename   | <none>  | Name of the file to read the data from            }"
        "{ filestorage |         | Write data to the specified XML/YAML/JSON file    }"
        "{ ksizeW      |         | Width of Gaussian kernel size                     }"
        "{ ksizeH      |         | Height of Gaussian kernel size                    }"
        "{ sigmaX      |         | Gaussian kernel standard deviation in X direction }"
        "{ sigmaY      | 0       | Gaussian kernel standard deviation in Y direction }"
        "{ borderType  | default | Pixel extrapolation method                        }"
    };

    cv::CommandLineParser parser{ argc, argv, keys };
    parser.about("Blurs an image using a Gaussian filter.");

    if (parser.has("help")) {
        parser.printMessage();
        printBorderTypes();
        return EXIT_SUCCESS;
    }

    auto filename{ parser.get<cv::String>("@filename") };
    auto filestorage{ parser.get<cv::String>("filestorage") };
    auto ksizeW{ parser.get<int>("ksizeW") };
    auto ksizeH{ parser.get<int>("ksizeH") };
    auto sigmaX{ parser.get<double>("sigmaX") };
    auto sigmaY{ parser.get<double>("sigmaY") };
    auto border_type{ parser.get<cv::String>("borderType") };

    if (!parser.check()) {
        parser.printErrors();
        return EXIT_FAILURE;
    }

    if (0 == ksizeW && 0 == ksizeH) {
        std::cerr << "[ERR] Both ksizeW and ksizeH are zeros.\n";
        return EXIT_FAILURE;
    }
    if ((0 == ksizeW && 0 != ksizeH) || (0 != ksizeW && 0 == ksizeH)) {
        std::cerr << "[ERR] Either ksizeW or ksizeH is zero.\n";
        return EXIT_FAILURE;
    }
    if (ksizeW < 0 || ksizeH < 0) {
        std::cerr << "[ERR] Either ksizeW or ksizeH is negative.\n";
        return EXIT_FAILURE;
    }
    if (0 == (ksizeW % 2) || 0 == (ksizeH % 2)) {
        std::cerr << "[ERR] Either ksizeW or ksizeH is not odd.\n";
        return EXIT_FAILURE;
    }

    cv::FileStorage fs{ filename, cv::FileStorage::READ };

    if (!fs.isOpened()) {
        std::cerr << "[ERROR] Failed to open the file.\n";
        return EXIT_FAILURE;
    }

    cv::Mat src{};
    fs["mat"] >> src;

    if (src.empty()) {
        std::cout << "[ERROR] Source data is empty.\n";
        return EXIT_FAILURE;
    }
    if (CV_8U != src.depth() &&
        CV_16U != src.depth() &&
        CV_16S != src.depth() &&
        CV_32F != src.depth() &&
        CV_64F != src.depth()) {
        std::cout << "[ERROR] Source depth is not supported.\n";
        return EXIT_FAILURE;
    }

    auto border_type_mode{ getBorderType(border_type) };

    if (INVALID_BORDER_TYPE == border_type_mode) {
        std::cerr << "[ERROR] Invalid border type.\n";
        return EXIT_FAILURE;
    }

    cv::Mat dst{};
    cv::GaussianBlur(
        src,
        dst,
        cv::Size(ksizeW, ksizeH),
        sigmaX,
        sigmaY,
        border_type_mode
    );

    if (parser.has("filestorage")) {
        cv::FileStorage fsw{ filestorage, cv::FileStorage::WRITE };
        fsw << "mat" << dst;
    }

    if (parser.has("verbose")) {
        std::cout << "[INFO] Blurred image:\n"
            << "rows = " << dst.rows << '\n'
            << "cols = " << dst.cols << '\n'
            << "channels = " << dst.channels() << '\n'
            << "type = " << type2Str(dst.type()) << '\n'
            << "isContinous = " << (dst.isContinuous() ? "true" : "false") << '\n';
    }

    if (parser.has("imshow") && !dst.empty()) {
        cv::String win_name{ "gaussian_blur_" };
        auto pos{ filename.find_last_of('\\') };
        win_name += filename.substr(pos + 1);

        cv::imshow(win_name, dst);
        cv::waitKey();
    }

    return EXIT_SUCCESS;
}
