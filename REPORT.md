# Mini relatorio — Laboratorio M1

## Identificacao

- Estudante: Mariana Ferreira
- Matricula: 7930666
- Laboratorio: M1.1
- Linguagem: C++

## 1. Objetivo

O objetivo desse laboratório foi a construção do conhecimento de como uma imagem digital é manipulada por meio dos seus pixels, por meio de implementações de operações relacionadas a inspeção da imagem, cópia, separação de canais, conversão para níveis de cinza e redução da quantidade de níveis de intensidade.
As operações foram realizadas manualmente, sem o uso de funções prontas.

## 2. Operacoes implementadas

Foram implementadas as operações: inspeção da imagem, cópia, separação dos canais, conversão para níveis de cinza e quantização.

- Inspeção da imagem: foi obtido a largura, altura, quantidade de canais, tipo da imagem, e quantidade de pixels. Para imagens coloridas foi calculado os valores de forma separada por canal.
- Cópia: foi criada uma matriz de mesma dimensão e tipo, percorrido os pixels da imagem de entrada e copiado o valor para a imagem de saída, tratando imagens de um canal e três canais de forma separada.
- Separação dos canais: foi implementado três operações para gerar os canais azul, verde e vermelho. As imagens de saída são inicializadas com zeros e somente o componente correspondente ao canal solicitado é copiado da imagem de entrada.
- Conversão para níveis de cinza por média simples: para cada pixel colorido, os três componentes são somados e divididos por três. O resultado é armazenado em uma imagem de um canal.
- Conversão para níveis de cinza por média ponderada: foi utilizada a expressão (0,299R + 0,587G + 0,114B), considerando os canais na ordem BGR. O resultado é convertido para inteiro e armazenado em uma imagem de um canal.
- Quantização: a partir de uma imagem em tons de cinza foi calculado o intervalo de cada nível com base na quantidade de níveis informada. Para cada pixel, o nível correspondente é calculado por divisão e o valor de saída é obtido multiplicando o nível pelo tamanho do intervalo. Também foi incluída uma verificação para impedir que o resultado ultrapasse o valor de 255.

## 3. Testes realizados

Foram utilizados os arquivos de imagem fornecidos para o laboratório para permitir a conferência manual dos resultados.

| Teste | Entrada | Operacao | Parametros | Resultado esperado ou criterio de verificacao |
|---|---|---|---|---|
|1 |m1_checkerboard_256.png | Inspeção| - | Verificar largura, altura, canais, tipo, quantidade de pixels e estatísticas de intensidade da imagem.|
|2|m1_color_2x2.png|Cópia| - | A imagem copy.png deve possuir os mesmos valores de pixels da imagem de entrada|
|3|m1_color.png|Separação do canal azul| - |channel_b.png deve preservar somente os valores do canal azul, mantendo os canais verde e vermelho zerados.|
|4|m1_color.png|Separação do canal verde| - |channel_g.png deve preservar somente os valores do canal verde, mantendo os canais azul e vermelho zerados.|
|5|m1_color.png|Separação do canal vermelho| - |channel_r.png deve preservar somente os valores do canal vermelho, mantendo os canais azul e verde zerados.|
|6|m1_color.png|Conversão para níveis de cinza em média simples| - |Verificar a geração de grayscale_average.png|
|7|m1_color.png|Conversão para níveis de cinza em média ponderada| - |Verificar a geração de grayscale_weighted.png|
|8|m1_shapes_border_256.png|Quantização|16 níveis|A saída deve apresentar no máximo 16 valores de intensidade|
|9|m1_shapes_border_256.png|Quantização|8 níveis|A saída deve apresentar no máximo 8 valores de intensidade|
|10|m1_shapes_border_256.png|Quantização|4 níveis|A saída deve apresentar no máximo 4 valores de intensidade|
|11|m1_shapes_border_256.png|Quantização|2 níveis|A saída deve apresentar no máximo 2 valores de intensidade|

## 4. Resultados

As imagens geradas pela implementação foram:

| Operacao | Entrada | Parametros | Arquivo de saida | Observacao |
|---|---|---|---|---|
|Inspeção |m1_checkerboard_256.png | - | inspect.txt ||
|Cópia|m1_color_2x2.png| - |copy.png||
|Separação do canal azul|m1_color.png| - |channel_b.png|Somente o canal azul é preservado|
|Separação do canal verde|m1_color.png| - |channel_g.png|Somente o canal verde é preservado|
|Separação do canal vermelho|m1_color.png| - |channel_r.png|Somente o canal vermelho é preservado|
|Conversão para níveis de cinza em média simples|m1_color.png| - |grayscale_average.png|Os três canais contribuem igualmente|
|Conversão para níveis de cinza em média ponderada|m1_color.png| - |grayscale_weighted.png|O canal verde possui maior contribuição|
|Quantização|m1_shapes_border_256.png|16 níveis|quant_16.png|Redução moderada da quantidade de intensidades|
|Quantização|m1_shapes_border_256.png|8 níveis|quant_8.png|Maior perda de variações de tons|
|Quantização|m1_shapes_border_256.png|4 níveis|quant_4.png|Redução mais evidente|
|Quantização|m1_shapes_border_256.png|2 níveis|quant_2.png|Imagem apresenta apenas duas faixas de intensidade|


## 5. Analise tecnica

1. Qual é a diferença entre resolução espacial e resolução radiométrica?
A resolução espacial representa a quantidade de pixels, o que influencia na nitidez da imagem, enquanto a resolução radiométrica representa a quantidade de tons que cada pixel pode ter, influenciando na cor e brilho.

2. Por que a média ponderada produz resultado diferente da média simples?
Porque nossos olhos não enxergam todas as cores com a mesma intensidade, somos mais sensíveis ao verde e menos ao azul. A média ponderada dá pesos diferentes para R, G e B por causa disso, enquanto a média simples trata todas as cores igual.

3. O que ocorre visualmente quando a quantidade de níveis é reduzida?
A imagem perde suavidade nos tons. Quanto menos níveis, mais a imagem parece quadriculada.

4. Em quais regiões da imagem a perda de informação fica mais evidente?
Nas áreas com transições suaves de cor. Áreas com bordas bem definidas essa perda é pouco notada.

5. Como o tipo e o número de canais da imagem interferem na forma de acessar um pixel?
1 valor por pixel: acessa o valor do pixel diretamente.
3 valores por pixel: precisa indicar também qual canal (R, G ou B).

## 6. Limitacoes

A implementação foi desenvolvida considerando principalmente imagens compatíveis com as operações solicitadas no laboratório, imagens coloridas com três canais e imagens em tons de cinza. Imagens com número de canais diferente precisariam ser tratadas separadamente ou rejeitadas pelo programa.

## 7. Referencias

- Material didático disponibilizado para o Laboratório M1.1.
- OPENCV. OpenCV documentation. [S. l.]: OpenCV, 2026. Disponível em: https://docs.opencv.org/.
- GONZALEZ, Rafael C.; WOODS, Richard E. Processamento digital de imagens. 3. ed. São Paulo: Pearson Prentice Hall, 2010.
