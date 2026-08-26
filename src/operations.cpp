#include "pdi/operations.hpp"

#include <array>
#include <iostream>
#include <string_view>

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

    std::cerr
        << "Operacao '" << options.operation
        << "' reconhecida, mas ainda nao implementada no projeto-base.\n";

    return ExitCode::general_error;
}

} // namespace pdi
