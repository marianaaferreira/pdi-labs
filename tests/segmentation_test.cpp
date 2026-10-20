#include "pdi/cli.hpp"
#include "pdi/contract.hpp"
#include "pdi/errors.hpp"
#include "pdi/exit_code.hpp"
#include "pdi/image_io.hpp"
#include "pdi/operations.hpp"

#include <opencv2/core.hpp>

#include <chrono>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>

namespace {

class TestResultsDirectory {
public:
    TestResultsDirectory(){
        path_ = std::filesystem::current_path() / "tests/test-results";
        std::filesystem::create_directories(path_);
    }

    const std::filesystem::path& path() const {
        return path_;
    }

private:
    std::filesystem::path path_;
};

void require(bool condition, const std::string& message){
    if (!condition) throw std::runtime_error(message);
}

pdi::CliOptions make_options(const std::filesystem::path& input_path, const std::filesystem::path& output_path, const std::string& method, std::optional<std::string> threshold = std::nullopt){
    pdi::CliOptions options;
    options.operation = "segment";
    options.input = input_path.string();
    options.output = output_path.string();
    options.parameters["method"] = method;
    if (threshold.has_value())
        options.parameters["threshold"] = *threshold;
    return options;
}

pdi::ExitCode execute_operation(const pdi::CliOptions& options){
    const auto validation = pdi::validate_contract(options);
    if (!validation)
        return validation.code;
    return pdi::run_operation(options);
}

cv::Mat segment_and_read(const std::filesystem::path& input_path, const std::filesystem::path& output_path, const std::string& method, int threshold = 0){
    std::optional<std::string> threshold_option;
    if (method == "fixed")
        threshold_option = std::to_string(threshold);

    const auto options = make_options(input_path, output_path, method, threshold_option);
    require(static_cast<int>(execute_operation(options)) == 0, "A operacao de segmentacao falhou para " + input_path.string());
    return pdi::read_image(output_path.string());
}

void verify_mask(const cv::Mat& input, const cv::Mat& mask){
    require(!mask.empty(), "A mascara de segmentacao esta vazia.");
    require(mask.rows == input.rows, "A mascara tem altura diferente da entrada.");
    require(mask.cols == input.cols, "A mascara tem largura diferente da entrada.");
    require(mask.type() == CV_8UC1, "A mascara nao possui exatamente um canal de 8 bits.");

    for (int y = 0; y < mask.rows; ++y) {
        for (int x = 0; x < mask.cols; ++x) {
            const auto value = mask.at<uchar>(y, x);
            require(value == 0 || value == 255, "A mascara contem um valor diferente de 0 e 255.");
        }
    }
}

void test_synthetic_image(const std::filesystem::path& directory){
    cv::Mat input(3, 4, CV_8UC3);
    for (int y = 0; y < input.rows; ++y) {
        for (int x = 0; x < input.cols; ++x) {
            const uchar intensity = (x + y) % 2 == 0 ? 0 : 255;
            input.at<cv::Vec3b>(y, x) = cv::Vec3b(intensity, intensity, intensity);
        }
    }

    const auto input_path = directory / "synthetic.png";
    const auto output_path = directory / "synthetic-mask.png";
    pdi::write_image(input_path.string(), input);

    const auto loaded_input = pdi::read_image(input_path.string());
    const auto mask = segment_and_read(input_path, output_path, "fixed", 127);
    verify_mask(loaded_input, mask);

    for (int y = 0; y < mask.rows; ++y) {
        for (int x = 0; x < mask.cols; ++x) {
            const uchar expected = (x + y) % 2 == 0 ? 0 : 255;
            require(mask.at<uchar>(y, x) == expected, "A segmentacao sintetica produziu um pixel inesperado.");
        }
    }
}

void test_threshold_boundary(const std::filesystem::path& directory){
    cv::Mat input(1, 3, CV_8UC1);
    input.at<uchar>(0, 0) = 126;
    input.at<uchar>(0, 1) = 127;
    input.at<uchar>(0, 2) = 128;

    const auto input_path = directory / "threshold-boundary.png";
    const auto output_path = directory / "threshold-boundary-mask.png";
    pdi::write_image(input_path.string(), input);

    const auto loaded_input = pdi::read_image(input_path.string());
    const auto mask = segment_and_read(input_path, output_path, "fixed", 127);
    verify_mask(loaded_input, mask);
    require(mask.at<uchar>(0, 0) == 0 &&
            mask.at<uchar>(0, 1) == 0 &&
            mask.at<uchar>(0, 2) == 255,
        "O limiar deve classificar 126 e 127 como 0 e 128 como 255."
    );
}

void test_invalid_inputs(const std::filesystem::path& directory){
    const auto input_path = directory / "synthetic.png";
    const auto output_path = directory / "invalid-mask.png";

    const auto missing_options = make_options(directory / "missing-image.png", output_path, "fixed", "127");
    const auto missing_validation = pdi::validate_contract(missing_options);
    require(missing_validation.code == pdi::ExitCode::read_error, "Uma imagem inexistente deve retornar read_error.");
    require(static_cast<int>(execute_operation(missing_options)) != 0, "Uma imagem inexistente deve produzir exit code diferente de zero.");

    bool read_failed = false;
    try {
        (void)pdi::read_image((directory / "missing-image.png").string());
    } catch (const pdi::PdiError& error) {
        read_failed = error.code() == pdi::ExitCode::read_error;
    }
    require(read_failed, "A leitura direta de imagem inexistente deve falhar.");

    const auto unknown_method = pdi::validate_contract(make_options(input_path, output_path, "unknown", "127"));
    require(unknown_method.code == pdi::ExitCode::invalid_parameter, "Um metodo de segmentacao desconhecido deve ser rejeitado.");
    require(static_cast<int>(unknown_method.code) != 0, "Um metodo desconhecido deve produzir exit code diferente de zero.");

    const auto invalid_threshold = pdi::validate_contract(make_options(input_path, output_path, "fixed", "256"));
    require(invalid_threshold.code == pdi::ExitCode::invalid_parameter, "Um limiar fora do intervalo permitido deve ser rejeitado.");
    require(static_cast<int>(invalid_threshold.code) != 0, "Um parametro invalido deve produzir exit code diferente de zero.");
}

void test_real_image(const std::filesystem::path& directory, const std::filesystem::path& input_path, const std::string& output_name){
    require(std::filesystem::is_regular_file(input_path), "Imagem real de teste nao encontrada: " + input_path.string());
    const auto input = pdi::read_image(input_path.string());
    const auto mask = segment_and_read(input_path, directory / output_name, "otsu");
    verify_mask(input, mask);
}

} // namespace

int main(){
    TestResultsDirectory results_directory;

    test_synthetic_image(results_directory.path());
    test_threshold_boundary(results_directory.path());
    test_invalid_inputs(results_directory.path());
    test_real_image(results_directory.path(), "images/input/moeda-facil.png", "moeda-facil-mask.png");
    test_real_image(results_directory.path(), "images/input/moeda-dificil.png", "moeda-dificil-mask.png");

    return 0;
}