#include "opencv2/opencv.hpp"
#include "helpers.hpp"
#include <iostream>

const cv::String COMMON_KEYS{
    "{ h help      |        | Display the usage                              }"
    "{ v verbose   |        | Verbose mode                                   }"
    "{ imshow      |        | Display an image in a window                   }"
    "{ filestorage |        | Write data to the specified XML/YAML/JSON file }"
    "{ @filename   | <none> | Name of file to be loaded                      }"
};

constexpr auto INVALID_FLAG{ -1 };

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

class ParamParser {
public:
    ParamParser(
        int argc,
        const char *const argv[],
        const cv::String &keys = "",
        const cv::String &about_message = "")
        : parser_{ argc, argv, params_.append(COMMON_KEYS).append(keys) } {

        parser_.about(about_message);
        filename_.append(parser_.get<cv::String>("@filename"));
        filestorage_.append(parser_.get<cv::String>("filestorage"));
    }

    ~ParamParser() {}

    bool isParamsOK(bool print_error = true) const {
        bool paramsOK{ parser_.check() };
        if (!paramsOK && print_error) {
            parser_.printErrors();
        }
        return paramsOK;
    }

    bool hasFileStorage() const {
        return parser_.has("filestorage");
    }

    bool hasHelp(bool print_help = true) const {
        bool needed_help{ parser_.has("help") };
        if (needed_help && print_help) {
            parser_.printMessage();
        }
        return needed_help;
    }

    bool hasImShow() const {
        return parser_.has("imshow");
    }

    bool hasVerbose() const {
        return parser_.has("verbose");
    }

    cv::String getFileName() const {
        return filename_;
    }

    cv::String getFileStorage() const {
        return filestorage_;
    }

protected:
    template<typename T>
    T getArgument(const cv::String &name) const {
        return parser_.get<T>(name);
    }

private:
    cv::String params_;
    cv::String filename_;
    cv::String filestorage_;
    cv::CommandLineParser parser_;
};

class ImReadParamParser : public ParamParser {
public:
    ImReadParamParser(
        int argc,
        const char *const argv[],
        const cv::String &keys = "",
        const cv::String &about_message = "")
        : ParamParser{ argc, argv, keys, about_message } {
        flags_.append(getArgument<cv::String>("flags"));
    }

    ~ImReadParamParser() {}

    bool hasHelp(bool print_help = true) const {
        bool needed_help = ParamParser::hasHelp();
        if (needed_help) {
            printFlags();
        }
        return needed_help;
    }

    cv::String getFlag() const {
        return flags_;
    }

private:
    cv::String flags_;

    void printFlags() const {
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
};

int main(int argc, char **argv) {
    const cv::String keys{
        "{ flags | color  | Mode of imread }"
    };
    const cv::String about_message{ "Loads an image from a file." };

    ImReadParamParser parser(argc, argv, keys, about_message);

    if (parser.hasHelp()) {
        return EXIT_SUCCESS;
    }

    if (!parser.isParamsOK()) {
        return EXIT_FAILURE;
    }

    auto imread_mode{ getImreadMode(parser.getFlag()) };

    if (INVALID_FLAG == imread_mode) {
        std::cerr << "[ERROR] Invalid flags.\n";
        return EXIT_FAILURE;
    }

    auto image{ cv::imread(parser.getFileName(), imread_mode) };

    if (image.empty()) {
        std::cout << "[WARN] imread returns an empty matrix.\n";
    }

    if (parser.hasFileStorage()) {
        cv::FileStorage fs{ parser.getFileStorage(), cv::FileStorage::WRITE };
        fs << "mat" << image;
    }

    if (parser.hasVerbose()) {
        std::cout << "[INFO] Loaded the image:\n"
            << "rows = " << image.rows << '\n'
            << "cols = " << image.cols << '\n'
            << "channels = " << image.channels() << '\n'
            << "type = " << type2Str(image.type()) << '\n'
            << "isContinous = " << (image.isContinuous() ? "true" : "false") << '\n';
    }

    if (parser.hasImShow() && !image.empty()) {
        cv::imshow(parser.getFileName(), image);
        cv::waitKey();
    }

    return EXIT_SUCCESS;
}
