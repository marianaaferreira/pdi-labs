#include "pdi/operations.hpp"

#include <array>
#include <iostream>
#include <string_view>
#include <opencv2/imgproc.hpp>
#include "pdi/errors.hpp"
#include "pdi/image_io.hpp"
#include "pdi/parameters.hpp"

namespace pdi {
namespace {

// Lista comum às três linguagens. As atividades complementares/opcionais
// não fazem parte do contrato mínimo do template.
constexpr std::array<std::string_view, 19> operations {
    // M1.1 — representação, canais e níveis de cinza
    "inspect",
    "copy",
    "channel_b",
    "channel_g",
    "channel_r",
    "grayscale_average",
    "grayscale_weighted",
    "quantize",

    // M1.2 — transformações de intensidade
    "brightness",
    "contrast",
    "negative",
    "threshold",
    "histogram",

    // M1.3 — convolução e filtragem espacial
    "convolution",
    "mean_filter",
    "weighted_mean",
    "laplacian",
    "sobel",

    // M2
    "segment"
};

} // namespace

bool is_known_operation(std::string_view operation)
{
    for (const auto candidate : operations) {
        if (candidate == operation) {
            return true;
        }
    }
    return false;
}

std::array<std::uint64_t, 256> histogram(const cv::Mat& input_image){
    std::array<std::uint64_t, 256> qt{};

    for (int y = 0; y < input_image.rows; ++y) {
        for (int x = 0; x < input_image.cols; ++x) {
            std::uint64_t intensity;
            if (input_image.channels() == 1) {
                intensity = input_image.at<uchar>(y, x);
            } else {
                cv::Vec3b pixel = input_image.at<cv::Vec3b>(y, x);
                intensity = static_cast<std::uint64_t>((pixel[0] + pixel[1] + pixel[2]) / 3);
            }
            qt[intensity]++;
        }
    }
    return qt;
}

cv::Mat threshold(const cv::Mat& input_image, int threshold_value, bool invert) {
    cv::Mat output_image(input_image.rows, input_image.cols, CV_8UC1);

    for (int y = 0; y < input_image.rows; ++y) {
        for (int x = 0; x < input_image.cols; ++x) {
            const uchar pixel = input_image.at<uchar>(y, x);
            int new_pixel;

            if (invert) {
                if (pixel <= threshold_value) {
                    new_pixel = 255;
                } else {
                    new_pixel = 0;
                }
            } else {
                if (pixel > threshold_value) {
                    new_pixel = 255;
                } else {
                    new_pixel = 0;
                }
            }

            output_image.at<uchar>(y, x) =
                static_cast<uchar>(new_pixel);
        }
    }

    return output_image;
}

cv::Mat to_grayscale(const cv::Mat& input_image)
{
    if (input_image.depth() != CV_8U) {
        throw PdiError(
            ExitCode::invalid_parameter, "A segmentacao exige imagem de 8 bits por canal."
        );
    }

    cv::Mat gray_image;
    if (input_image.channels() == 1) {
        gray_image = input_image;
    } else if (input_image.channels() == 3) {
        cv::cvtColor(input_image, gray_image, cv::COLOR_BGR2GRAY);
    } else if (input_image.channels() == 4) {
        cv::cvtColor(input_image, gray_image, cv::COLOR_BGRA2GRAY);
    } else {
        throw PdiError(
            ExitCode::invalid_parameter, "A segmentacao aceita imagens com 1, 3 ou 4 canais."
        );
    }
    return gray_image;
}

cv::Mat segment_fixed(const cv::Mat& input_image, int threshold_value, bool invert){
    const cv::Mat gray_image = to_grayscale(input_image);
    return threshold(gray_image, threshold_value, invert);
}

cv::Mat segment_otsu(const cv::Mat& input_image, bool invert, double& selected_threshold) {
    const cv::Mat gray_image = to_grayscale(input_image);
    const auto qt = histogram(gray_image);

    std::uint64_t total_pixels = 0;
    double total_sum = 0.0;

    for (int i = 0; i < 256; ++i) {
        total_pixels += qt[i];
        total_sum += i * static_cast<double>(qt[i]);
    }

    std::uint64_t background_pixels = 0;
    double background_sum = 0.0;
    double max_variance = -1.0;
    int best_threshold = 0;

    for (int t = 0; t < 256; ++t) {
        background_pixels += qt[t];
        background_sum += t * static_cast<double>(qt[t]);

        const std::uint64_t foreground_pixels =
            total_pixels - background_pixels;

        if (background_pixels == 0 || foreground_pixels == 0) {
            continue;
        }

        const double background_mean =
            background_sum / background_pixels;

        const double foreground_mean =
            (total_sum - background_sum) / foreground_pixels;

        const double difference =
            background_mean - foreground_mean;

        const double variance =
            static_cast<double>(background_pixels) *
            foreground_pixels * difference * difference;

        if (variance > max_variance) {
            max_variance = variance;
            best_threshold = t;
        }
    }

    selected_threshold = best_threshold;

    return threshold(gray_image, best_threshold, invert);
}

ExitCode run_operation(const CliOptions& options)
{
    // A infraestrutura anterior a este ponto já cuidou da sintaxe da CLI,
    // da presença dos arquivos e da validação dos parâmetros gerais.
    // A partir daqui começa a parte avaliada do laboratório.
    //
    // TODO(aluno) M1.1: implementar inspect, copy, channel_b, channel_g,
    // channel_r, grayscale_average, grayscale_weighted e quantize.
    //
    // TODO(aluno) M1.2: implementar brightness, contrast, negative,
    // threshold e histogram. O template fornece apenas a serialização CSV;
    // a contagem dos 256 níveis continua sendo responsabilidade do aluno.
    //
    // TODO(aluno) M1.3: implementar convolution, mean_filter,
    // weighted_mean, laplacian e sobel. O leitor de kernel fornecido apenas
    // valida e disponibiliza os coeficientes; ele não aplica convolução.
    //
    // Organize as implementações em funções/classes auxiliares e faça aqui
    // somente o despacho por operação. Não coloque os algoritmos em main.cpp
    // e não substitua os percursos manuais pedidos no roteiro por chamadas
    // prontas equivalentes do OpenCV.

    const std::string& input_path = options.input.value();
    const std::string& output_path = options.output.value();

    if (options.operation == "segment") {
        const std::string& method = options.parameters.at("method");
        const bool invert = has_parameter(options, "invert") &&
                            options.parameters.at("invert") == "true";
        const cv::Mat input_image = pdi::read_image(input_path);
        cv::Mat output_image;
        if (method == "fixed") {
            output_image = segment_fixed(
                input_image,
                parameter_as_int(options, "threshold"),
                invert
            );
        } else if (method == "otsu") {
            double selected_threshold = 0.0;
            output_image = segment_otsu(input_image, invert, selected_threshold);
            std::cout << "Limiar Otsu calculado: " << selected_threshold << '\n';
        } else {
            return ExitCode::invalid_parameter;
        }
        pdi::write_image(output_path, output_image);
        return ExitCode::success;
    }

    return ExitCode::general_error;
}

} // namespace pdi
