cmake --preset unix-debug
cmake --build --preset unix-debug
ctest --preset unix-debug

#fixed
./build/unix-debug/pdi_lab --operation segment --input images/input/moeda-facil.png --output images/output/fixed-facil.png --method fixed --threshold 240 --invert true
./build/unix-debug/pdi_lab --operation segment --input images/input/moeda-dificil.png --output images/output/fixed-dificil.png --method fixed --threshold 128 --invert true

#otsu
./build/unix-debug/pdi_lab --operation segment --input images/input/moeda-facil.png --output images/output/otsu-facil.png --method otsu --invert true
./build/unix-debug/pdi_lab --operation segment --input images/input/moeda-dificil.png --output images/output/otsu-dificil.png --method otsu --invert true

#m2.2
./build/unix-debug/pdi_lab --operation segment --input images/input/moeda-facil.png --output images/output/fixed-facil-m22.png --method fixed --threshold 240 --invert true
./build/unix-debug/pdi_lab --operation analyze --input images/output/fixed-facil-m22.png --output results/m22/moeda-facil --connectivity 8 --min-area 0
./build/unix-debug/pdi_lab --operation segment --input images/input/moeda-dificil.png --output images/output/fixed-dificil-m22.png --method fixed --threshold 128 --invert true
./build/unix-debug/pdi_lab --operation analyze --input images/output/fixed-dificil-m22.png --output results/m22/moeda-dificil --connectivity 8 --min-area 0