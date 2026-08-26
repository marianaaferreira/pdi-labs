#include "pdi/contract.hpp"

#include "pdi/operations.hpp"
#include "pdi/parameters.hpp"

#include <array>
#include <exception>
#include <filesystem>
#include <string>

namespace pdi {
namespace {

ValidationResult require_parameter(const CliOptions& options, const std::string& name)
{
    if (!has_parameter(options, name)) {
        return {
            ExitCode::invalid_arguments,
            "A operacao '" + options.operation + "' exige --" + name + "."
        };
    }
    return {};
}

ValidationResult validate_typed_parameters(const CliOptions& options)
{
    try {
        if (options.operation == "quantize") {
            const auto required = require_parameter(options, "levels");
            if (!required) return required;
            const int levels = parameter_as_int(options, "levels");
            if (levels != 2 && levels != 4 && levels != 8 && levels != 16) {
                return {ExitCode::invalid_parameter, "--levels deve ser 2, 4, 8 ou 16."};
            }
        }

        if (options.operation == "brightness") {
            const auto required = require_parameter(options, "value");
            if (!required) return required;
            (void)parameter_as_int(options, "value");
        }

        if (options.operation == "contrast") {
            const auto required = require_parameter(options, "alpha");
            if (!required) return required;
            if (parameter_as_double(options, "alpha") <= 0.0) {
                return {ExitCode::invalid_parameter, "--alpha deve ser maior que zero."};
            }
        }

        if (options.operation == "threshold") {
            const auto required = require_parameter(options, "threshold");
            if (!required) return required;
            const int threshold = parameter_as_int(options, "threshold");
            if (threshold < 0 || threshold > 255) {
                return {ExitCode::invalid_parameter, "--threshold deve estar entre 0 e 255."};
            }
        }

        if (options.operation == "convolution") {
            auto required = require_parameter(options, "kernel");
            if (!required) return required;
            required = require_parameter(options, "border");
            if (!required) return required;

            const auto kernel_path = parameter_as_path(options, "kernel");
            if (!std::filesystem::is_regular_file(kernel_path)) {
                return {
                    ExitCode::read_error,
                    "Nao foi possivel abrir o kernel: " + kernel_path.string()
                };
            }
            (void)parameter_as_border(options);
        }

        if (options.operation == "mean_filter") {
            auto required = require_parameter(options, "size");
            if (!required) return required;
            required = require_parameter(options, "border");
            if (!required) return required;
            const int size = parameter_as_int(options, "size");
            if (size != 3 && size != 5) {
                return {ExitCode::invalid_parameter, "--size deve ser 3 ou 5 para mean_filter."};
            }
            (void)parameter_as_border(options);
        }

        constexpr std::array spatial_with_border {
            "weighted_mean", "laplacian", "sobel"
        };
        for (const auto* operation : spatial_with_border) {
            if (options.operation == operation) {
                const auto required = require_parameter(options, "border");
                if (!required) return required;
                (void)parameter_as_border(options);
            }
        }
    } catch (const std::exception& error) {
        return {ExitCode::invalid_parameter, error.what()};
    }

    return {};
}

} // namespace

ValidationResult validate_contract(const CliOptions& options)
{
    if (options.operation.empty()) {
        return {ExitCode::invalid_arguments, "Informe --operation."};
    }
    if (!is_known_operation(options.operation)) {
        return {
            ExitCode::unknown_operation,
            "Operacao desconhecida: " + options.operation
        };
    }
    if (!options.input.has_value() || options.input->empty()) {
        return {ExitCode::invalid_arguments, "Informe --input."};
    }
    if (!std::filesystem::is_regular_file(*options.input)) {
        return {
            ExitCode::read_error,
            "Nao foi possivel abrir a imagem: " + *options.input
        };
    }
    if (options.operation != "inspect" &&
        (!options.output.has_value() || options.output->empty())) {
        return {ExitCode::invalid_arguments, "Informe --output."};
    }

    return validate_typed_parameters(options);
}

} // namespace pdi
