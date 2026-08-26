# Versao minima do CMake usada pelo projeto-base.
cmake_minimum_required(VERSION 3.24)

# Declara o projeto C++ e sua versao.
# A versao identifica o template fornecido pela disciplina.
project(pdi_lab VERSION 0.2.0 LANGUAGES CXX)

# O projeto utiliza C++20 de forma portavel.
# REQUIRED impede que o CMake escolha silenciosamente um padrao mais antigo.
# EXTENSIONS OFF evita extensoes especificas de um compilador.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Gera compile_commands.json, util para editores e ferramentas de analise.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Habilita a infraestrutura de testes do CMake por meio do CTest.
include(CTest)

# Centraliza a descoberta do OpenCV, inclusive para instalacoes portateis.
include(cmake/PdiOpenCV.cmake)

# A biblioteca pdi_core contem a infraestrutura comum da aplicacao:
# - leitura da linha de comando;
# - validacao do contrato;
# - leitura e escrita de imagens;
# - conversao de parametros;
# - leitura de kernels;
# - escrita de CSV e JSON;
# - despacho das operacoes.
#
# Os algoritmos avaliados permanecem sem implementacao no template.
add_library(pdi_core
    src/cli.cpp
    src/contract.cpp
    src/image_io.cpp
    src/kernel.cpp
    src/operations.cpp
    src/parameters.cpp
    src/result_io.cpp
)

# Alias para tornar a dependencia mais explicita nos demais alvos.
add_library(pdi::core ALIAS pdi_core)

# Disponibiliza os headers de include/ para quem utilizar pdi_core.
target_include_directories(
    pdi_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Liga a infraestrutura do projeto aos modulos do OpenCV encontrados.
target_link_libraries(pdi_core PUBLIC ${OpenCV_LIBS})

# Warnings ajudam a detectar construcoes suspeitas durante a compilacao.
# Eles nao sao convertidos em erro para nao bloquear o desenvolvimento.
if(MSVC)
    target_compile_options(pdi_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(
        pdi_core
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
            -Wsign-conversion
    )
endif()

# Executavel principal usado pela interface de linha de comando.
add_executable(pdi_lab src/main.cpp)
target_link_libraries(pdi_lab PRIVATE pdi::core)

# Os testes abaixo verificam apenas a infraestrutura fornecida.
# Eles nao implementam nem validam a corretude dos algoritmos avaliados.
if(BUILD_TESTING)
    add_executable(pdi_smoke_test tests/smoke_test.cpp)
    target_link_libraries(pdi_smoke_test PRIVATE pdi::core)
    add_test(NAME pdi_smoke_test COMMAND pdi_smoke_test)

    add_executable(pdi_infrastructure_test tests/infrastructure_test.cpp)
    target_link_libraries(pdi_infrastructure_test PRIVATE pdi::core)
    add_test(NAME pdi_infrastructure_test COMMAND pdi_infrastructure_test)

    # Os testes usam caminhos relativos para as imagens-base. Definir a pasta
    # do projeto como working directory torna o comportamento igual entre os
    # diferentes diretórios de build gerados pelos presets.
    set_tests_properties(
        pdi_smoke_test
        pdi_infrastructure_test
        PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()
