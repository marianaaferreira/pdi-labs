#include "pdi/operations.hpp"

#include <array>
#include <iostream>
#include <string_view>
#include <opencv2/opencv.hpp>
#include "pdi/image_io.hpp"
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

void inspect(const cv::Mat input_image) {
    std::ofstream out("images/output/inspect.txt");
    if (!out) {
        std::cerr << "Não foi possível abrir output/inspect.txt\n";
        return;
    }
    int largura = input_image.cols;
    int altura = input_image.rows;
    int canais = input_image.channels();
    int tipo = input_image.type();
    int qtd_pixels = altura * largura;

    out << "Largura: " << largura << "\n";
    out << "Altura: " << altura << "\n";
    out << "Canais: " << canais << "\n";
    out << "Tipo: " << tipo << "\n";
    out << "Quantidade de pixels: " << qtd_pixels << "\n";

    if (canais == 1) {
        int minv = 255;
        int maxv = 0;
        int soma = 0;

        for (int y = 0; y < altura; ++y) {
            for (int x = 0; x < largura; ++x) {
                const auto value = input_image.at<uchar>(y, x);
                if (value < minv) minv = value;
                if (value > maxv) maxv = value;
                soma += value;
            }
        }

        float media = (float)(soma) / (largura * altura);

        out << "Minimo: " << minv << "\n";
        out << "Maximo: " << maxv << "\n";
        out << "Media: " << media << "\n";
    }
    else if (canais == 3){
        int minB = 255, minG = 255, minR = 255;
        int maxB = 0,   maxG = 0,   maxR = 0;

        int somaB = 0;
        int somaG = 0;
        int somaR = 0;

        for (int y = 0; y < input_image.rows; y++){
            for (int x = 0; x < input_image.cols; x++){

                cv::Vec3b pixel = input_image.at<cv::Vec3b>(y, x);

                int B = pixel[0];
                int G = pixel[1];
                int R = pixel[2];

                // Azul
                if (B < minB)
                    minB = B;

                if (B > maxB)
                    maxB = B;

                somaB += B;

                // Verde
                if (G < minG)
                    minG = G;

                if (G > maxG)
                    maxG = G;

                somaG += G;

                // Vermelho
                if (R < minR)
                    minR = R;

                if (R > maxR)
                    maxR = R;

                somaR += R;
            }
        }

        float mediaB = (float)somaB / qtd_pixels;
        float mediaG = (float)somaG / qtd_pixels;
        float mediaR = (float)somaR / qtd_pixels;

        out << "\nCanal Azul (B):" << "\n";
        out << "Minimo: " << minB << "\n";
        out << "Maximo: " << maxB << "\n";
        out << "Media: " << mediaB << "\n";

        out << "\nCanal Verde (G):" << "\n";
        out << "Minimo: " << minG << "\n";
        out << "Maximo: " << maxG << "\n";
        out << "Media: " << mediaG << "\n";

        out << "\nCanal Vermelho (R):" << "\n";
        out << "Minimo: " << minR << "\n";
        out << "Maximo: " << maxR << "\n";
        out << "Media: " << mediaR << "\n";

    }
}

cv::Mat copy(const cv::Mat input_image) {
    cv::Mat copia(input_image.rows, input_image.cols, input_image.type());

    if (input_image.channels() == 1) {
        for (int y = 0; y < input_image.rows; ++y) {
            for (int x = 0; x < input_image.cols; ++x) {
                copia.at<uchar>(y, x) = input_image.at<uchar>(y, x);
            }
        }
        return copia;
    }

    if (input_image.channels() == 3) {
        for (int y = 0; y < input_image.rows; ++y) {
            for (int x = 0; x < input_image.cols; ++x) {
                copia.at<cv::Vec3b>(y, x) = input_image.at<cv::Vec3b>(y, x);
            }
        }
    }

    return copia;
}

cv::Mat channel_b(const cv::Mat input_image) {
    cv::Mat output_image = cv::Mat::zeros(input_image.rows, input_image.cols, CV_8UC3);

    for (int y = 0; y < input_image.rows; ++y) {
        for (int x = 0; x < input_image.cols; ++x) {
            output_image.at<cv::Vec3b>(y, x)[0] = input_image.at<cv::Vec3b>(y, x)[0];
        }
    }

    return output_image;
}

cv::Mat channel_g(const cv::Mat input_image) {
    cv::Mat output_image = cv::Mat::zeros(input_image.rows, input_image.cols, CV_8UC3);

    for (int y = 0; y < input_image.rows; ++y) {
        for (int x = 0; x < input_image.cols; ++x) {
            output_image.at<cv::Vec3b>(y, x)[1] = input_image.at<cv::Vec3b>(y, x)[1];
        }
    }

    return output_image;
}

cv::Mat channel_r(const cv::Mat input_image) {
    cv::Mat output_image = cv::Mat::zeros(input_image.rows, input_image.cols, CV_8UC3);

    for (int y = 0; y < input_image.rows; ++y) {
        for (int x = 0; x < input_image.cols; ++x) {
            output_image.at<cv::Vec3b>(y, x)[2] = input_image.at<cv::Vec3b>(y, x)[2];
        }
    }

    return output_image;
}

cv::Mat grayscale_average(const cv::Mat input_image) {
    cv::Mat output_image(input_image.rows, input_image.cols, CV_8UC1);

    for (int y = 0; y < input_image.rows; ++y) {
        for (int x = 0; x < input_image.cols; ++x) {
            const auto pixel = input_image.at<cv::Vec3b>(y, x);
            const int average = (pixel[0] + pixel[1] + pixel[2]) / 3;
            output_image.at<uchar>(y, x) = static_cast<uchar>(average);
        }
    }

    return output_image;
}

cv::Mat grayscale_weighted(const cv::Mat input_image) {
    cv::Mat output_image(input_image.rows, input_image.cols, CV_8UC1);

    for (int y = 0; y < input_image.rows; ++y) {
        for (int x = 0; x < input_image.cols; ++x) {
            const auto pixel = input_image.at<cv::Vec3b>(y, x);
            const int weighted_average = static_cast<int>(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
            output_image.at<uchar>(y, x) = static_cast<uchar>(weighted_average);
        }
    }

    return output_image;
}

cv::Mat quantize(const cv::Mat& input_image, int levels)
{
    const int step = 256 / levels;
    cv::Mat output_image(input_image.rows, input_image.cols, CV_8UC1);

    for (int y = 0; y < input_image.rows; ++y) {
        for (int x = 0; x < input_image.cols; ++x) {
            uchar pixel = input_image.at<uchar>(y, x);
            int level = pixel / step;
            int new_value = level * step;
            if (new_value > 255) new_value = 255;
            output_image.at<uchar>(y, x) = static_cast<uchar>(new_value);
        }
    }

    return output_image;
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

    if (options.operation == "inspect") {
        inspect(pdi::read_image(input_path));
        return ExitCode::success;
    }

    if (options.operation == "copy") {
        cv::Mat output_image = copy(pdi::read_image(input_path));
        pdi::write_image(output_path, output_image);
        return ExitCode::success;
    }

    if (options.operation == "channel_b") {
        cv::Mat output_image = channel_b(pdi::read_image(input_path));
        pdi::write_image(output_path, output_image);
        return ExitCode::success;
    }

    if (options.operation == "channel_g") {
        cv::Mat output_image = channel_g(pdi::read_image(input_path));
        pdi::write_image(output_path, output_image);
        return ExitCode::success;
    }

    if (options.operation == "channel_r") {
        cv::Mat output_image = channel_r(pdi::read_image(input_path));
        pdi::write_image(output_path, output_image);
        return ExitCode::success;
    }

    if (options.operation == "grayscale_average") {
        cv::Mat output_image = grayscale_average(pdi::read_image(input_path));
        pdi::write_image(output_path, output_image);
        return ExitCode::success;
    }

    if (options.operation == "grayscale_weighted") {
        cv::Mat output_image = grayscale_weighted(pdi::read_image(input_path));
        pdi::write_image(output_path, output_image);
        return ExitCode::success;
    }

    if (options.operation == "quantize") {
        const int levels = parameter_as_int(options, "levels");
        cv::Mat output_image = quantize(pdi::read_image(input_path), levels);
        pdi::write_image(output_path, output_image);
        return ExitCode::success;
    }

    return ExitCode::general_error;
}

} // namespace pdi
