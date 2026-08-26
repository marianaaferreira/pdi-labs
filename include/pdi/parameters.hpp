#pragma once

#include "pdi/cli.hpp"

#include <filesystem>
#include <string>

namespace pdi {

// Estratégias de borda previstas no Laboratório M1.3.
// A infraestrutura converte a string da CLI para este enum para que o
// algoritmo do estudante não precise fazer parsing de texto.
enum class BorderStrategy {
    copy,
    replicate
};

// Funções de infraestrutura para obter parâmetros já convertidos.
// Elas lançam std::invalid_argument quando o parâmetro está ausente ou
// possui sintaxe inválida. As regras de obrigatoriedade são verificadas
// previamente pelo contrato da CLI.
int parameter_as_int(const CliOptions& options, const std::string& name);
double parameter_as_double(const CliOptions& options, const std::string& name);
std::filesystem::path parameter_as_path(const CliOptions& options, const std::string& name);
BorderStrategy parameter_as_border(const CliOptions& options, const std::string& name = "border");

bool has_parameter(const CliOptions& options, const std::string& name);

} // namespace pdi
