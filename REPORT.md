# Mini relatorio — Laboratorio M1

## Identificacao

- Estudante: Mariana Ferreira
- Matricula: 7930666
- Laboratorio: M2-1
- Linguagem: C++

## 1. Objetivo

O objetivo deste laboratório é avaliar e comparar métodos de limiarização para a geração de máscaras binárias. Foi implementado o limiar fixo e o método de Otsu. O alvo da segmentação escolhido foram moedas.
Foram utilizadas duas imagens com níveis de dificuldade diferentes. A imagem fácil apresenta moedas sobre um fundo branco e uniforme, já a imagem difícil apresenta moedas sobre a mão de uma pessoa, com variação de iluminação.
A comparação dos dois métodos busca a identificação da melhor escolha para a continuidade do projeto.

## 2. Operacoes implementadas

- Conversão para escala de cinza: a função to_grayscale converte imagens coloridas para escala de cinza utilizando cv::cvtColor, com as conversões COLOR_BGR2GRAY e COLOR_BGRA2GRAY. Imagens que já possuem um canal são utilizadas diretamente. A função aceita imagens de 8 bits por canal com 1, 3 ou 4 canais.
- Histograma: a função histogram calcula a frequência das intensidades dos pixels em 256 níveis, de 0 a 255.
- Limiarização por valor fixo: a função converte a entrada para escala de cinza e aplica um limiar definido pelo usuário.
- Limiarização de Otsu: a função calcula o histograma da imagem em escala de cinza e utiliza as frequências para determinar automaticamente um limiar. O algoritmo avalia os possíveis limiares e seleciona aquele que maximiza a variância entre as classes de fundo e primeiro plano.

## 3. Testes realizados

Foram consideradas duas imagens para avaliar o comportamento dos métodos em condições diferentes. A imagem fácil apresenta moedas sobre um fundo claro, enquanto a imagem difícil apresenta moedas sobre a mão de uma pessoa

| Teste | Entrada | Operacao | Parametros | Resultado esperado ou criterio de verificacao |
|---|---|---|---|---|
| 1|moeda-facil.png |segment com método fixed |limiar: 240 | Gerar uma máscara binária e avaliar a separação entre as moedas e o fundo.|
| 2 | moeda-dificil.png | segment com método fixed | limiar: 128 |Gerar uma máscara binária e avaliar os efeitos das variações de iluminação e textura na segmentação.|
| 3 | moeda-facil.png | segment com método otsu | limiar calculado automaticamente |  Comparar a máscara obtida com a produzida pelo limiar fixo.| 
| 4 | moeda-dificil.png | segment com método otsu | limiar calculado automaticamente |  Comparar a máscara obtida com a produzida pelo limiar fixo.| 

## 4. Resultados

Os arquivos de saída foram organizados em images/output/.

| Operacao | Entrada | Parametros | Arquivo de saida | Observacao |
|---|---|---|---|---|
|fixed |moeda-facil.png |--threshold 240 |images/output/fixed-facil.png | A máscara apresentou as regiões das moedas preenchidas de maneira relativamente uniforme|
|fixed |moeda-dificil.png |--threshold 128 |images/output/fixed-dificil.png |A máscara apresenta regiões fragmentadas e partes que não foram classificadas como primeiro plano. |
| otsu | moeda-facil.png | limiar automatico | images/output/otsu-facil.png |Foram observadas partes dos objetos classificadas como fundo. |
| otsu | moeda-dificil.png | limiar automatico | images/output/otsu-dificil.png | A máscara apresenta regiões da mão que são classificadas como objeto que não pertencem ao alvo. |

## 5. Analise tecnica

A. Em que casos o limiar fixo falhou?
Na imagem fácil, o limiar fixo apresentou uma separação visual relativamente uniforme entre as regiões das moedas e o fundo.
Na imagem difícil, o método apresentou regiões fragmentadas e partes da cena não classificadas como primeiro plano. As variações de iluminação, as sombras e as diferenças de intensidade dificultam a escolha de um único limiar capaz de separar corretamente as moedas do restante da imagem.

B. Qual parâmetro foi mais sensível?
O valor do limiar fixo é um parâmetro importante, pois determina quais pixels serão classificados como primeiro plano. A sensibilidade foi avaliada executando o método com diferentes limiares para a mesma imagem e comparando as máscaras obtidas.

C. Quais erros podem atrapalhar a contagem de objetos no M2.2?
Os principais erros são a fragmentação de uma moeda em várias regiões, a inclusão de regiões da mão ou do fundo e a perda de partes dos objetos.

Escolha da configuração para M2-2:
A configuração selecionada para o M2.2 será o fixed, pois apresentou resultados visualmente melhores na imagem fácil, com regiões correspondentes às moedas preenchidas de maneira relativamente uniforme. Essa característica favorece a identificação dos objetos e a contagem de moedas na próxima etapa do projeto. Apesar das limitações observadas na imagem difícil, o método foi escolhido por produzir resultados adequados no cenário em que os objetos estão mais bem separados do fundo.

## 6. Limitacoes

Na imagem difícil, as variações de iluminação, as sombras e as texturas podem fazer com que partes das moedas sejam classificadas como fundo ou que regiões da mão sejam incluídas na máscara.
O método de Otsu calcula o limiar automaticamente com base no histograma, mas isso não garante uma segmentação correta em imagens nas quais os objetos e o fundo apresentam intensidades semelhantes.

## 7. Referencias

Material didático disponibilizado em aula.
OPENCV. OpenCV documentation. [S. l.]: OpenCV, 2026. Disponível em: https://docs.opencv.org/.
