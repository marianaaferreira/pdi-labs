#include "pdi/parameters.hpp"

#include <stdexcept>

namespace pdi {
namespace {

const std::string& require_parameter(const CliOptions& options, const std::string& name)
{
    const auto it = options.parameters.find(name);
    if (it == options.parameters.end() || it->second.empty()) {
        throw std::invalid_argument("Parametro ausente: --" + name);
    }
    return it->second;
}

} // namespace

bool has_parameter(const CliOptions& options, const std::string& name)
{
    const auto it = options.parameters.find(name);
    return it != options.parameters.end() && !it->second.empty();
}

int parameter_as_int(const CliOptions& options, const std::string& name)
{
    const auto& text = require_parameter(options, name);
    std::size_t consumed = 0;
    int value = 0;
    try {
        value = std::stoi(text, &consumed);
    } catch (const std::exception&) {
        throw std::invalid_argument("Valor invalido para --" + name + ": " + text);
    }
    if (consumed != text.size()) {
        throw std::invalid_argument("Valor invalido para --" + name + ": " + text);
    }
    return value;
}

double parameter_as_double(const CliOptions& options, const std::string& name)
{
    const auto& text = require_parameter(options, name);
    std::size_t consumed = 0;
    double value = 0.0;
    try {
        value = std::stod(text, &consumed);
    } catch (const std::exception&) {
        throw std::invalid_argument("Valor invalido para --" + name + ": " + text);
    }
    if (consumed != text.size()) {
        throw std::invalid_argument("Valor invalido para --" + name + ": " + text);
    }
    return value;
}

std::filesystem::path parameter_as_path(const CliOptions& options, const std::string& name)
{
    return std::filesystem::path(require_parameter(options, name));
}

BorderStrategy parameter_as_border(const CliOptions& options, const std::string& name)
{
    const auto& text = require_parameter(options, name);
    if (text == "copy") {
        return BorderStrategy::copy;
    }
    if (text == "replicate") {
        return BorderStrategy::replicate;
    }
    throw std::invalid_argument(
        "Valor invalido para --" + name + ": " + text +
        ". Use copy ou replicate."
    );
}

} // namespace pdi
