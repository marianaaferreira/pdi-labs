#include "pdi/exit_code.hpp"
#include "pdi/image_io.hpp"
#include "pdi/operations.hpp"

#include <array>
#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstdint>

namespace {

namespace fs = std::filesystem;

const cv::Mat read_input(const std::string& name){
    const cv::Mat image = pdi::read_image("images/input/" + name);
    assert(!image.empty());
    return image;
}

fs::path output_path(const std::string& name){
    return fs::path("images/output/m1-2_test") / name;
}

void run(const std::string& operation, const std::string& input, const fs::path& output, const std::string& parameter_name = {}, const std::string& parameter_value = {}){
    pdi::CliOptions options;
    options.operation = operation;
    options.input = "images/input/" + input;
    options.output = output.string();

    if (!parameter_name.empty()) {
        options.parameters[parameter_name] = parameter_value;
    }
    assert(pdi::run_operation(options) == pdi::ExitCode::success);
}

int clamp(int value){
    if (value < 0) return 0;
    if (value > 255) return 255;
    return value;
}

void test_brightness(){
    const cv::Mat input = read_input("m1_color.png");
    const fs::path positive_output = output_path("brightness1.png");
    const fs::path negative_output = output_path("brightness2.png");

    run("brightness", "m1_color.png", positive_output, "value", "80");
    run("brightness", "m1_color.png", negative_output, "value", "-50");

    const cv::Mat positive = pdi::read_image(positive_output.string());
    const cv::Mat negative = pdi::read_image(negative_output.string());

    for (int y = 0; y < input.rows; ++y) {
        for (int x = 0; x < input.cols; ++x) {
            const cv::Vec3b source = input.at<cv::Vec3b>(y, x);

            for (int channel = 0; channel < 3; ++channel) {
                assert(
                    positive.at<cv::Vec3b>(y, x)[channel] ==
                    clamp(source[channel] + 80)
                );

                assert(
                    negative.at<cv::Vec3b>(y, x)[channel] ==
                    clamp(source[channel] - 50)
                );
            }
        }
    }
}

void test_contrast(){
    const cv::Mat input = read_input("m1_color.png");

    for (const double alpha : {0.5, 1.0, 1.5}) {
        const fs::path output = output_path("contrast_" + std::to_string(alpha) + ".png");

        run("contrast", "m1_color.png", output, "alpha", std::to_string(alpha));

        const cv::Mat result = pdi::read_image(output.string());

        for (int y = 0; y < input.rows; ++y)
            for (int x = 0; x < input.cols; ++x) {
                const cv::Vec3b source = input.at<cv::Vec3b>(y, x);
                const cv::Vec3b actual = result.at<cv::Vec3b>(y, x);

                for (int channel = 0; channel < 3; ++channel) {
                    double expected = alpha * (source[channel] - 128) + 128;
                    if (expected < 0) expected = 0;
                    if (expected > 255) expected = 255;
                    assert(std::abs(static_cast<double>(actual[channel]) - expected) <= 1.0
                    );
                }
            }
    }
}

void test_negative(){
    const cv::Mat input = read_input("m1_shapes_border_256.png");
    const fs::path output = output_path("negative.png");
    run("negative", "m1_shapes_border_256.png", output);
    const cv::Mat result = pdi::read_image(output.string());
    for (int y = 0; y < input.rows; ++y) {
        for (int x = 0; x < input.cols; ++x) {
            const int source = input.at<uchar>(y, x);
            const int expected = 255 - source;
            assert(result.at<uchar>(y, x) == expected);
        }
    }
}

void test_threshold(){
    const cv::Mat input = read_input("m1_shapes_border_256.png");
    for (const int threshold : {128, 200}) {
        const fs::path output =output_path("threshold_" + std::to_string(threshold) + ".png");
        run("threshold", "m1_shapes_border_256.png", output, "threshold", std::to_string(threshold));

        const cv::Mat result = pdi::read_image(output.string());

        for (int y = 0; y < input.rows; ++y) {
            for (int x = 0; x < input.cols; ++x) {
                const int source = input.at<uchar>(y, x);
                int expected;
                if (source < threshold) expected = 0;
                else expected = 255;
                assert(result.at<uchar>(y, x) == expected);
            }
        }
    }
}

void run_histogram(const fs::path& input, const fs::path& output){
    pdi::CliOptions options;
    options.operation = "histogram";
    options.input = input.string();
    options.output = output.string();

    assert(pdi::run_operation(options) == pdi::ExitCode::success);
}

void test_histogram(){
    const std::array<fs::path, 3> images{
        fs::path("images/input/m1_color.png"),
        output_path("brightness1.png"),
        output_path("brightness2.png")
    };

    for (std::size_t i = 0; i < images.size(); ++i) {
        const fs::path output = output_path("histogram_" + std::to_string(i) + ".csv");

        run_histogram(images[i], output);

        const cv::Mat input = pdi::read_image(images[i].string());
        assert(!input.empty());

        std::ifstream file(output);
        assert(file);

        std::string line;
        std::getline(file, line);
        assert(line == "intensity,count");

        std::uint64_t total = 0;

        for (int intensity = 0; intensity < 256; ++intensity) {
            std::getline(file, line);

            const std::size_t comma = line.find(',');
            assert(comma != std::string::npos);

            const int value = std::stoi(line.substr(0, comma));
            const std::uint64_t count = std::stoull(line.substr(comma + 1));

            assert(value == intensity);
            total += count;
        }

        assert(total == static_cast<std::uint64_t>(input.rows * input.cols));
    }
}

}

int main(){
    fs::remove_all("images/output/m1-2_test");
    fs::create_directories("images/output/m1-2_test");

    test_brightness();
    test_contrast();
    test_negative();
    test_threshold();
    test_histogram();

    fs::remove_all("images/output/m1-2_test");

    return 0;
}