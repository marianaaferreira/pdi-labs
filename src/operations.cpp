#include "pdi/operations.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <opencv2/opencv.hpp>
#include "pdi/image_io.hpp"
#include "pdi/result_io.hpp"
#include <fstream>
#include "pdi/parameters.hpp"

namespace pdi {
namespace {

// Lista comum às três linguagens. As atividades complementares/opcionais
// não fazem parte do contrato mínimo do template.
constexpr std::array<std::string_view, 18> operations {
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
    "sobel"
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

cv::Mat brightness(const cv::Mat& input_image, int value){
    cv::Mat output_image(input_image.rows, input_image.cols, input_image.type());
    if (input_image.channels() == 1) {
        for (int y = 0; y < input_image.rows; ++y)
            for (int x = 0; x < input_image.cols; ++x) {
                int pixel_value = input_image.at<uchar>(y, x) + value;
                if (pixel_value < 0) pixel_value = 0;
                if (pixel_value > 255) pixel_value = 255;
                output_image.at<uchar>(y, x) = static_cast<uchar>(pixel_value);
            }
        return output_image;
    }

    if (input_image.channels() == 3)
        for (int y = 0; y < input_image.rows; ++y)
            for (int x = 0; x < input_image.cols; ++x) {
                cv::Vec3b pixel = input_image.at<cv::Vec3b>(y, x);
                int blue = pixel[0] + value;
                int green = pixel[1] + value;
                int red = pixel[2] + value;

                if (blue < 0) blue = 0;
                if (blue > 255) blue = 255;
                if (green < 0) green = 0;
                if (green > 255) green = 255;
                if (red < 0) red = 0;
                if (red > 255) red = 255;

                pixel[0] = static_cast<uchar>(blue);
                pixel[1] = static_cast<uchar>(green);
                pixel[2] = static_cast<uchar>(red);

                output_image.at<cv::Vec3b>(y, x) = pixel;
            }
    return output_image;
}

cv::Mat contrast(const cv::Mat& input_image, double alpha) {
    cv::Mat output_image(input_image.rows, input_image.cols, input_image.type());

    for (int y = 0; y < input_image.rows; ++y) {
        for (int x = 0; x < input_image.cols; ++x) {
            if (input_image.channels() == 1) {
                int pixel = input_image.at<uchar>(y, x);
                double new_pixel = alpha * (pixel - 128) + 128;
                if (new_pixel < 0) new_pixel = 0;
                else if (new_pixel > 255) new_pixel = 255;
                output_image.at<uchar>(y, x) = static_cast<uchar>(new_pixel);

            } else if (input_image.channels() == 3) {
                cv::Vec3b pixel = input_image.at<cv::Vec3b>(y, x);
                double  blue = alpha * (pixel[0] - 128) + 128;
                double  green = alpha * (pixel[1] - 128) + 128;
                double  red = alpha * (pixel[2] - 128) + 128;

                if (blue < 0) blue = 0;
                if (blue > 255) blue = 255;

                if (green < 0) green = 0;
                if (green > 255) green = 255;

                if (red < 0) red = 0;
                if (red > 255) red = 255;

                pixel[0] = static_cast<uchar>(blue);
                pixel[1] = static_cast<uchar>(green);
                pixel[2] = static_cast<uchar>(red);

                output_image.at<cv::Vec3b>(y, x) = pixel;
            }
        }
    }
    return output_image;
}

cv::Mat negative(const cv::Mat& input_image) {
    cv::Mat output_image(input_image.rows, input_image.cols, input_image.type());

    for (int y = 0; y < input_image.rows; ++y)
        for (int x = 0; x < input_image.cols; ++x) {
            int pixel = input_image.at<uchar>(y, x);
            int new_pixel = 255 - pixel;
            output_image.at<uchar>(y, x) =static_cast<uchar>(new_pixel);
        }
    return output_image;
}

cv::Mat threshold(const cv::Mat& input_image, int threshold) {
    cv::Mat output_image(input_image.rows, input_image.cols, input_image.type());

    for (int y = 0; y < input_image.rows; ++y) {
        for (int x = 0; x < input_image.cols; ++x) {
            int pixel = input_image.at<uchar>(y, x);
            int new_pixel;
            if (pixel < threshold)
                new_pixel = 0;
            else
                new_pixel = 255;

            output_image.at<uchar>(y, x) = static_cast<uchar>(new_pixel);
        }
    }
    return output_image;
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

    if (options.operation == "brightness") {
        const int value = parameter_as_int(options, "value");
        cv::Mat output_image = brightness(pdi::read_image(input_path), value);
        pdi::write_image(output_path, output_image);
        return ExitCode::success;
    }

    if (options.operation == "contrast") {
        const double alpha = parameter_as_double(options, "alpha");
        cv::Mat output_image = contrast(pdi::read_image(input_path), alpha);
        pdi::write_image(output_path, output_image);
        return ExitCode::success;
    }

    if (options.operation == "negative") {
        cv::Mat output_image = negative(pdi::read_image(input_path));
        pdi::write_image(output_path, output_image);
        return ExitCode::success;
    }

    if (options.operation == "threshold") {
        const int threshold_value = parameter_as_int(options, "threshold");
        cv::Mat output_image = threshold(pdi::read_image(input_path), threshold_value);
        pdi::write_image(output_path, output_image);
        return ExitCode::success;
    }

    if (options.operation == "histogram") {
        const auto qt = histogram(pdi::read_image(input_path));
        pdi::write_histogram_csv(output_path, qt);
        return ExitCode::success;
    }

    return ExitCode::general_error;
}

} // namespace pdi
