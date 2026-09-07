# Mini relatorio — Laboratorio M1

## Identificacao

- Estudante: Mariana Ferreira
- Matricula: 7930666
- Laboratorio: M1.2
- Linguagem: C++

## 1. Objetivo

Implementar manualmente transformacoes de intensidade em imagens, percorrendo os pixels e entender como brilho, contraste, negativo e limiarizacao alteram os valores. Também foi implementado o cálculo de histograma e exportação para CSV.

## 2. Operacoes implementadas

- brightness: soma um valor inteiro a cada canal e limita o resultado ao intervalo [0, 255].
- contrast: aplica alpha * (pixel - 128) + 128 por canal e limita o resultado a [0, 255].
- negative: calcula 255 - pixel para cada pixel de uma imagem em tons de cinza.
- threshold: compara cada pixel com T, valores menores que T recebem 0 e valores maiores ou iguais recebem 255.
- histogram: cria um vetor com 256 posições. Para imagens em tons de cinza usa o valor do pixel e para imagens coloridas usa a média dos três canais.

## 3. Testes realizados

| Teste | Entrada | Operacao | Parametros | Resultado esperado ou criterio de verificacao |
|---|---|---|---|---|
| Brilho positivo | `images/input/m1_color.png` | `brightness` | `value=80` | Valores acima de 255 devem receber 255. |
| Brilho negativo | `images/input/m1_color.png` | `brightness` | `value=-50` | Valores abaixo de 0 devem receber 0. |
| Contraste 1 | `images/input/m1_color.png` | `contrast` | `alpha=0.5` | Aproximar os valores de 128. |
| Contraste 2 | `images/input/m1_color.png` | `contrast` | `alpha=1.0` | Reproduzir os valores originais|
| Contraste 3 | `images/input/m1_color.png` | `contrast` | `alpha=1.5` | Afastar os valores de 128 |
| Negativo | `images/input/m1_shapes_border_256.png` | `negative` | - | Cada pixel da imagem de saída deve ser igual a 255 menos o valor do pixel correspondente da imagem de entrada. |
| Limiar 1 | `images/input/m1_shapes_border_256.png` | `threshold` | `threshold=128` | A saída deve conter somente 0 e 255. Pixels com intensidade menor que 128 devem resultar em 0 e pixels com intensidade maior ou igual a 128 devem resultar em 255. |
| Limiar 2 | `images/input/m1_shapes_border_256.png` | `threshold` | `threshold=200` | A saída deve conter somente 0 e 255. Pixels com intensidade menor que 200 devem resultar em 0 e pixels com intensidade maior ou igual a 200 devem resultar em 255. |
| Histograma 1 | `images/input/m1_color.png` | `histogram` | - | O CSV deve conter 256 intensidades e a soma das contagens deve ser igual ao numero de pixels. |
| Histograma 2 | `images/output/brightness1.png` | `histogram` | - | O CSV deve conter 256 intensidades e a soma das contagens deve ser igual ao numero de pixels. |
| Histograma 3 | `images/output/brightness2.png` | `histogram` | - | O CSV deve conter 256 intensidades e a soma das contagens deve ser igual ao numero de pixels. |

## 4. Resultados

| Operacao | Entrada | Parametros | Arquivo de saida | Observacao |
|---|---|---|---|---|
| `brightness` | `m1_color.png` | `value=80` | `images/output/brightness1.png` | Aumento de brilho |
| `brightness` | `m1_color.png` | `value=-50` | `images/output/brightness2.png` | Redução de brilho |
| `contrast` | `m1_color.png` | `alpha=0.5`, `1.0`, `1.5` | `images/output/contrast1.png`, `contrast2.png`, `contrast3.png` | Comparaçã entre contrastes. |
| `negative` | `m1_shapes_border_256.png` | - | `images/output/negative.png` | Inversao das intensidades em tons de cinza. |
| `threshold` | `m1_shapes_border_256.png` | `T=128` e `T=200` | `images/output/threshold1.png`, `threshold2.png` | Resultados binarios com dois limiares. |
| `histogram` | `images/input/m1_color.png` | - | `images/output/histogram_original.csv` | Histograma da imagem original. |
| `histogram` | `brightness1.png` e `brightness2.png` | - | `images/output/histogram_processed.csv`, `histogram_processed2.csv` | Comparação dos valores causados pelo aumento e redução de brilho. |


## 5. Analise tecnica

1. Qual é a diferença observada entre alteração de brilho e alteração de contraste?
A alteração de brilho soma o mesmo valor a todos os pixels, deslocando a imagem para intensidades maiores ou menores. A alteração de contraste pode aproximar as intensidades de 128 e reduzir a diferença entre claro e escuro ou afastar de 128 e aumentar a diferença.

2. Em quais testes ocorreu saturação e qual foi seu efeito?
Ocorreu saturação no brilho positivo, quando valores acima de 255 foram fixados em 255, e no brilho negativo quando valores abaixo de 0 foram fixados em 0. A saturação elimina diferenças entre valores que ultrapassariam o limite e todos passam a ter a mesma intensidade.

3. Como o histograma se deslocou após alterar o brilho?
Com value=80, o histograma foi deslocado para a direita: a faixa ocupada passou a ser de 80 a 255. Com value=-50, foi deslocado para a esquerda: a faixa passou a ser de 0 a 181. Assim, entradas positivas clareiam a imagem e a negativa escurece.

4. Como a distribuição das intensidades mudou ao alterar o contraste?
Para alpha=0,5, a distribuicao ficou em torno de 128, ocupando aproximadamente de 64 a 179, sem saturação nos extremos. Para alpha=1,0, os permaneceram os mesmos da imagem original. Para alpha=1,5, a distribuição passou a ocupar de 0 a 255.

5. Que informação é perdida após a limiarização?
Se perde a variação de tons, detalhes de textura e diferenças entre pixels que pertenciam ao mesmo lado. A imagem passa a representar apenas valores binários.

## 6. Limitacoes

- Negative e threshold acessam os pixels como uchar, portanto foram projetadas para imagens de um canal em tons de cinza. Não há verificação local para canais antes dessas operações.
- O histograma colorido usa a média dos canais, perdendo informação de cor.

## 7. Referencias

- Material didático disponibilizado em aula.
- OPENCV. OpenCV documentation. [S. l.]: OpenCV, 2026. Disponível em: https://docs.opencv.org/.
- GONZALEZ, Rafael C.; WOODS, Richard E. Processamento digital de imagens. 3. ed. São Paulo: Pearson Prentice Hall, 2010.