#include "pdi/cli.hpp"

#include <array>
#include <iostream>
#include <stdexcept>
#include <string_view>

namespace pdi {
namespace {

constexpr std::array<std::string_view, 7> parameter_names {
    "value", "levels", "threshold", "alpha", "kernel", "border", "size"
};

bool is_known_parameter(std::string_view name)
{
    for (const auto candidate : parameter_names) {
        if (candidate == name) {
            return true;
        }
    }
    return false;
}

} // namespace

CliOptions parse_cli(int argc, char** argv)
{
    CliOptions options;

    for (int i = 1; i < argc; ++i) {
        const std::string argument = argv[i];

        if (argument == "--help" || argument == "-h") {
            options.help = true;
            continue;
        }
        if (argument == "--version") {
            options.version = true;
            continue;
        }
        if (!argument.starts_with("--")) {
            throw std::invalid_argument("Argumento inesperado: " + argument);
        }
        if (i + 1 >= argc) {
            throw std::invalid_argument("Falta valor para: " + argument);
        }

        const std::string value = argv[++i];
        const std::string name = argument.substr(2);

        if (name == "operation") {
            options.operation = value;
        } else if (name == "input") {
            options.input = value;
        } else if (name == "output") {
            options.output = value;
        } else if (is_known_parameter(name)) {
            // Os parâmetros conhecidos são convertidos e validados pela camada
            // de contrato antes de chegarem ao algoritmo do estudante.
            options.parameters[name] = value;
        } else {
            throw std::invalid_argument("Opcao desconhecida: --" + name);
        }
    }

    return options;
}

void print_help()
{
    std::cout
        << "pdi_lab - projeto-base dos laboratorios M1\n\n"
        << "Uso geral:\n"
        << "  pdi_lab --operation <operacao> --input <arquivo> --output <arquivo> [parametros]\n"
        << "  pdi_lab --help\n"
        << "  pdi_lab --version\n\n"
        << "Parametros comuns:\n"
        << "  --value <inteiro>          brilho\n"
        << "  --levels <2|4|8|16>        quantizacao\n"
        << "  --threshold <0..255>       limiarizacao\n"
        << "  --alpha <real>             contraste\n"
        << "  --kernel <arquivo>         convolucao generica\n"
        << "  --border <copy|replicate>  estrategia de borda\n"
        << "  --size <3|5>               tamanho do filtro de media\n\n"
        << "Operacoes M1.1:\n"
        << "  inspect, copy, channel_b, channel_g, channel_r,\n"
        << "  grayscale_average, grayscale_weighted, quantize\n\n"
        << "Operacoes M1.2:\n"
        << "  brightness, contrast, negative, threshold, histogram\n\n"
        << "Operacoes M1.3:\n"
        << "  convolution, mean_filter, weighted_mean, laplacian, sobel\n\n"
        << "Exemplos:\n"
        << "  pdi_lab --operation quantize --input in.png --output out.png --levels 8\n"
        << "  pdi_lab --operation threshold --input in.png --output out.png --threshold 128\n"
        << "  pdi_lab --operation mean_filter --input in.png --output out.png --size 3 --border replicate\n";
}

void print_version()
{
    std::cout << "pdi_lab 0.2.0 contract=2\n";
}

} // namespace pdi
