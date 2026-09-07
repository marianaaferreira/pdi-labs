# Declaração de uso de IA generativa

- Ferramenta: ChatGPT, Github Copilot
- Finalidade: Utilização da ferramenta para auxiliar no entendimento de conceitos e elaboração e revisão dos testes, identificação e correção de erros de código.
- Partes afetadas: Desenvolvimento das operações, dos testes e revisão da documentação e dos relatórios.
- Forma de validação: As sugestões dadas pelas ferramentas foram analisadas e testadas, verificando seu funcionamento no ambiente de desenvolvimento.
- Modificações realizadas: Os conteúdos e códigos sugeridos pelas ferramentas foram revisados e adaptados.
- Link da conversa ou registro equivalente: Registro dos prompts utilizados:
    - Analise o seguinte warning de compilação: "warning: conversion to ‘std::array<long unsigned int, 256>::size_type’ {aka ‘long unsigned int’} from ‘int’ may change the sign of the result [-Wsign-conversion] qt[intensity]++;", ocorrido na linha qt[intensity]++. Explique por que esse warning pode estar acontecendo.
    - Como posso verificar em um teste se um arquivo CSV de histograma possui as 256 intensidades e se a soma das contagens corresponde ao número de pixels da imagem?
    - Verifique se os testes implementados para as operações implementadas em operations.cpp estão de acordo com os critérios definidos em laboratorio_M1_2.md. Identifique possíveis erros nos testes e sugira as correções necessárias.