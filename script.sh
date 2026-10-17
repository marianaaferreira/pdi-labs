cmake --preset unix-debug
cmake --build --preset unix-debug
ctest --preset unix-debug

#fixed
./build/unix-debug/pdi_lab --operation segment --input images/input/moeda-facil.png --output images/output/fixed-facil.png --method fixed --threshold 240
./build/unix-debug/pdi_lab --operation segment --input images/input/moeda-dificil.png --output images/output/fixed-dificil.png --method fixed --threshold 128

#otsu
./build/unix-debug/pdi_lab --operation segment --input images/input/moeda-facil.png --output images/output/otsu-facil.png --method otsu 
./build/unix-debug/pdi_lab --operation segment --input images/input/moeda-dificil.png --output images/output/otsu-dificil.png --method otsu 