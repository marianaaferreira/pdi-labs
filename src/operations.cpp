#include "pdi/operations.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "pdi/errors.hpp"
#include "pdi/image_io.hpp"
#include "pdi/parameters.hpp"
#include "pdi/result_io.hpp"

namespace pdi {
namespace {

// Lista comum às três linguagens. As atividades complementares/opcionais
// não fazem parte do contrato mínimo do template.
constexpr std::array<std::string_view, 20> operations {
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
    "segment",
    "analyze"
};

struct ComponentAnalysis {
    cv::Mat labels;
    cv::Mat stats;
    cv::Mat centroids;
    int component_count = 0;
    std::uint64_t foreground_area = 0;
    std::uint64_t kept_area = 0;
    int kept_count = 0;
};

ComponentAnalysis analyze_components(const cv::Mat& mask, int connectivity, int min_area){
    ComponentAnalysis analysis;
    const int label_count = cv::connectedComponentsWithStats(
        mask, analysis.labels, analysis.stats, analysis.centroids,
        connectivity, CV_32S
    );
    analysis.component_count = label_count - 1;

    for (int label = 1; label < label_count; ++label) {
        const int area = analysis.stats.at<int>(label, cv::CC_STAT_AREA);
        analysis.foreground_area += static_cast<std::uint64_t>(area);
        if (area >= min_area) {
            ++analysis.kept_count;
            analysis.kept_area += static_cast<std::uint64_t>(area);
        }
    }
    return analysis;
}

void write_components_csv(const std::filesystem::path& path, const ComponentAnalysis& analysis, int min_area){
    try {
        ensure_parent_directory(path);
    } catch (const std::exception& error) {
        throw PdiError(ExitCode::write_error, "Nao foi possivel preparar o CSV de componentes: " + std::string(error.what()));
    }

    std::ofstream output(path);
    if (!output)
        throw PdiError(ExitCode::write_error, "Nao foi possivel criar o CSV: " + path.string());

    output << "label,area,x,y,width,height,cx,cy,kept\n";
    output << std::setprecision(10);
    for (int label = 1; label <= analysis.component_count; ++label) {
        const int area = analysis.stats.at<int>(label, cv::CC_STAT_AREA);
        output << label << ','
               << area << ','
               << analysis.stats.at<int>(label, cv::CC_STAT_LEFT) << ','
               << analysis.stats.at<int>(label, cv::CC_STAT_TOP) << ','
               << analysis.stats.at<int>(label, cv::CC_STAT_WIDTH) << ','
               << analysis.stats.at<int>(label, cv::CC_STAT_HEIGHT) << ','
               << analysis.centroids.at<double>(label, 0) << ','
               << analysis.centroids.at<double>(label, 1) << ','
               << (area >= min_area ? "true" : "false") << '\n';
    }
    if (!output)
        throw PdiError(ExitCode::write_error, "Falha ao gravar o CSV: " + path.string());
}

void write_analysis_summary(const std::filesystem::path& path, int connectivity, int min_area, const ComponentAnalysis& analysis4, const ComponentAnalysis& analysis8, const ComponentAnalysis& selected){
    try {
        ensure_parent_directory(path);
    } catch (const std::exception& error) {
        throw PdiError(ExitCode::write_error, "Nao foi possivel preparar o resumo: " + std::string(error.what()));
    }

    std::ofstream output(path);
    if (!output) 
        throw PdiError(ExitCode::write_error, "Nao foi possivel criar o resumo: " + path.string());

    output << "{\n"
           << "  \"stage\": \"initial\",\n"
           << "  \"selected_connectivity\": " << connectivity << ",\n"
           << "  \"min_area\": " << min_area << ",\n"
           << "  \"foreground_area_pixels\": " << selected.foreground_area << ",\n"
           << "  \"filtered_area_pixels\": " << selected.kept_area << ",\n"
           << "  \"components_connectivity_4_raw\": " << analysis4.component_count << ",\n"
           << "  \"components_connectivity_4_kept\": " << analysis4.kept_count << ",\n"
           << "  \"components_connectivity_8_raw\": " << analysis8.component_count << ",\n"
           << "  \"components_connectivity_8_kept\": " << analysis8.kept_count << "\n"
           << "}\n";
    if (!output)
        throw PdiError(ExitCode::write_error, "Falha ao gravar o resumo: " + path.string());
}

void analyze_mask(const CliOptions& options, const std::string& input_path, const std::string& output_prefix) {
    const cv::Mat mask = read_image(input_path);

    if (mask.empty() || mask.type() != CV_8UC1) 
        throw PdiError(ExitCode::invalid_parameter, "A operacao analyze exige uma mascara de 8 bits e um canal.");

    for (int y = 0; y < mask.rows; ++y)
        for (int x = 0; x < mask.cols; ++x) {
            const uchar value = mask.at<uchar>(y, x);
            if (value != 0 && value != 255) 
                throw PdiError(ExitCode::invalid_parameter, "A mascara deve conter somente 0 e 255.");
        }

    int connectivity = 8;
    if (has_parameter(options, "connectivity"))
        connectivity = parameter_as_int(options, "connectivity");

    int min_area = 0;
    if (has_parameter(options, "min-area"))
        min_area = parameter_as_int(options, "min-area");

    if (connectivity != 4 && connectivity != 8)
        throw PdiError(ExitCode::invalid_parameter, "A conectividade deve ser 4 ou 8.");

    const ComponentAnalysis analysis4 = analyze_components(mask, 4, min_area);
    const ComponentAnalysis analysis8 = analyze_components(mask, 8, min_area);

    ComponentAnalysis selected;
    if (connectivity == 4)
        selected = analysis4;
    else
        selected = analysis8;

    const std::filesystem::path prefix(output_prefix);
    const std::filesystem::path csv_path(prefix.string() + "_initial.csv");
    const std::filesystem::path summary_path(prefix.string() + "_summary.json");

    write_components_csv(csv_path, selected, min_area);
    write_analysis_summary(summary_path, connectivity, min_area, analysis4, analysis8, selected);

    std::cout << "Componentes (conectividade 4): " << analysis4.component_count << " brutos, " << analysis4.kept_count << " mantidos\n";
    std::cout << "Componentes (conectividade 8): " << analysis8.component_count << " brutos, " << analysis8.kept_count << " mantidos\n";
    std::cout << "CSV selecionado: " << csv_path.string() << '\n';
    std::cout << "Resumo: " << summary_path.string() << '\n';
}

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

        const std::uint64_t foreground_pixels = total_pixels - background_pixels;

        if (background_pixels == 0 || foreground_pixels == 0) continue;

        const double background_mean = background_sum / background_pixels;
        const double foreground_mean = (total_sum - background_sum) / foreground_pixels;
        const double difference = background_mean - foreground_mean;
        const double variance = static_cast<double>(background_pixels) * foreground_pixels * difference * difference;

        if (variance > max_variance) {
            max_variance = variance;
            best_threshold = t;
        }
    }
    selected_threshold = best_threshold;
    return threshold(gray_image, best_threshold, invert);
}

void validate_binary_mask(const cv::Mat& input_image, const cv::Mat& mask){
    if (mask.rows != input_image.rows || mask.cols != input_image.cols || mask.type() != CV_8UC1)
        throw PdiError(ExitCode::general_error, "A mascara de segmentacao deve ter as dimensoes da entrada e ser CV_8UC1.");

    for (int y = 0; y < mask.rows; ++y) {
        for (int x = 0; x < mask.cols; ++x) {
            const uchar value = mask.at<uchar>(y, x);
            if (value != 0 && value != 255)
                throw PdiError( ExitCode::general_error, "A mascara de segmentacao deve conter somente 0 e 255.");
        }
    }
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
        validate_binary_mask(input_image, output_image);
        pdi::write_image(output_path, output_image);
        return ExitCode::success;
    }

    if (options.operation == "analyze") {
        analyze_mask(options, input_path, output_path);
        return ExitCode::success;
    }

    return ExitCode::general_error;
}

} // namespace pdi
