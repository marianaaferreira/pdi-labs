#include "pdi/exit_code.hpp"
#include "pdi/image_io.hpp"
#include "pdi/operations.hpp"

#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace {

namespace fs = std::filesystem;

const cv::Mat read_input(const std::string& name){
	const cv::Mat image = pdi::read_image("images/input/" + name);
	assert(!image.empty());
	return image;
}

fs::path output_path(const std::string& name){
	return fs::path("images/output/m1-1_test") / name;
}

void run(const std::string& operation, const std::string& input, const fs::path& output, const std::string& levels = {}){
	pdi::CliOptions options;
	options.operation = operation;
	options.input = "images/input/" + input;
	options.output = output.string();

	if (!levels.empty()) {
		options.parameters["levels"] = levels;
	}

	assert(pdi::run_operation(options) == pdi::ExitCode::success);
}

void test_inspection(){
	const cv::Mat input = read_input("m1_checkerboard_256.png");
	run("inspect","m1_checkerboard_256.png",output_path("unused.png"));

	std::ifstream report("images/output/inspect.txt");
	assert(report);

	std::stringstream contents;
	contents << report.rdbuf();

	const std::string text = contents.str();
	assert(text.find("Largura: " + std::to_string(input.cols)) != std::string::npos);
	assert(text.find("Altura: " + std::to_string(input.rows)) != std::string::npos);
	assert(text.find("Canais: " + std::to_string(input.channels())) != std::string::npos);
	assert(text.find("Tipo: " + std::to_string(input.type())) != std::string::npos);
	assert(text.find("Quantidade de pixels: " + std::to_string(input.total())) != std::string::npos);

	int minimum = 255;
	int maximum = 0;
	int sum = 0;

	for (int y = 0; y < input.rows; ++y) {
		for (int x = 0; x < input.cols; ++x) {
			const int value = input.at<uchar>(y, x);
			if (value < minimum)
				minimum = value;
			if (value > maximum) 
				maximum = value;
			sum += value;
		}
	}

	assert(text.find("Minimo: " + std::to_string(minimum)) != std::string::npos);
	assert(text.find("Maximo: " + std::to_string(maximum)) != std::string::npos);

	std::ostringstream average;
	average << static_cast<float>(sum) / input.total();

	assert(text.find("Media: " + average.str()) != std::string::npos);
}

void test_copy(){
	const cv::Mat input = read_input("m1_color_2x2.png");
	const fs::path output = output_path("copy.png");

	run("copy","m1_color_2x2.png",output);

	const cv::Mat result = pdi::read_image(output.string());

	assert(result.size() == input.size());
	assert(result.type() == input.type());

	for (int y = 0; y < input.rows; ++y)
		for (int x = 0; x < input.cols; ++x)
			assert(result.at<cv::Vec3b>(y, x) == input.at<cv::Vec3b>(y, x));
}

void test_channel(const std::string& operation, int channel, const std::string& name){
	const cv::Mat input = read_input("m1_color.png");
	const fs::path output = output_path(name + ".png");

	run(operation, "m1_color.png", output);

	const cv::Mat result = pdi::read_image(output.string());

	assert(result.type() == CV_8UC3);

	for (int y = 0; y < input.rows; ++y) {
		for (int x = 0; x < input.cols; ++x) {
			const cv::Vec3b source = input.at<cv::Vec3b>(y, x);
			const cv::Vec3b actual = result.at<cv::Vec3b>(y, x);
			if (channel == 0) {
				assert(actual[0] == source[0]);
				assert(actual[1] == 0);
				assert(actual[2] == 0);
			} else if (channel == 1) {
				assert(actual[0] == 0);
				assert(actual[1] == source[1]);
				assert(actual[2] == 0);
			} else {
				assert(actual[0] == 0);
				assert(actual[1] == 0);
				assert(actual[2] == source[2]);
			}
		}
	}
}

void test_grayscale(const std::string& operation, bool weighted, const std::string& name){
	const cv::Mat input = read_input("m1_color.png");
	const fs::path output = output_path(name + ".png");
	run(operation, "m1_color.png", output);
	const cv::Mat result = pdi::read_image(output.string());
	assert(result.type() == CV_8UC1);

	for (int y = 0; y < input.rows; ++y) {
		for (int x = 0; x < input.cols; ++x) {
			const cv::Vec3b pixel = input.at<cv::Vec3b>(y, x);
			int expected = 0;
			if (weighted)
				expected = static_cast<int>(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
			else
				expected = (pixel[0] + pixel[1] + pixel[2]) / 3;
			assert(result.at<uchar>(y, x) == expected);
		}
	}
}

void test_quantization(int levels){
	const cv::Mat input = read_input("m1_shapes_border_256.png");
	const fs::path output = output_path("quant_" + std::to_string(levels) + ".png");
	run("quantize", "m1_shapes_border_256.png", output,std::to_string(levels));
	const cv::Mat result = pdi::read_image(output.string());
	assert(result.type() == CV_8UC1);
	std::array<bool, 256> values{};

	for (int y = 0; y < input.rows; ++y) {
		for (int x = 0; x < input.cols; ++x) {
			const int value = result.at<uchar>(y, x);
			values[value] = true;
		}
	}

	int distinct_values = 0;
	for (bool present : values)
		if (present)
			distinct_values++;
	assert(distinct_values <= levels);
}

}

int main()
{
	fs::remove_all("images/output/m1-1_test");
	fs::create_directories("images/output/m1-1_test");

	test_inspection();
	test_copy();
	test_channel("channel_b", 0, "channel_b");
	test_channel("channel_g", 1, "channel_g");
    test_channel("channel_r", 2, "channel_r");
	test_grayscale("grayscale_average", false,"grayscale_average");
	test_grayscale("grayscale_weighted",true, "grayscale_weighted");
	test_quantization(16);
	test_quantization(8);
	test_quantization(4);
	test_quantization(2);

	fs::remove_all("images/output/m1-1_test");

	return 0;
}