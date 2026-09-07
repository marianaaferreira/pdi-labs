#!/bin/bash

cmake --preset unix-debug
cmake --build --preset unix-debug
ctest --preset unix-debug
./build/unix-debug/pdi_lab --operation brightness --input images/input/m1_color.png --output images/output/brightness1.png --value 80
./build/unix-debug/pdi_lab --operation brightness --input images/input/m1_color.png --output images/output/brightness2.png --value -50
./build/unix-debug/pdi_lab --operation contrast --input images/input/m1_color.png --output images/output/contrast1.png --alpha 0.5
./build/unix-debug/pdi_lab --operation contrast --input images/input/m1_color.png --output images/output/contrast2.png --alpha 1.0
./build/unix-debug/pdi_lab --operation contrast --input images/input/m1_color.png --output images/output/contrast3.png --alpha 1.5
./build/unix-debug/pdi_lab --operation negative --input images/input/m1_shapes_border_256.png --output images/output/negative.png
./build/unix-debug/pdi_lab --operation threshold --input images/input/m1_shapes_border_256.png --output images/output/threshold1.png --threshold 128
./build/unix-debug/pdi_lab --operation threshold --input images/input/m1_shapes_border_256.png --output images/output/threshold2.png --threshold 200
./build/unix-debug/pdi_lab --operation histogram --input images/input/m1_color.png --output images/output/histogram_original.csv
./build/unix-debug/pdi_lab --operation histogram --input images/output/brightness1.png --output images/output/histogram_processed.csv
./build/unix-debug/pdi_lab --operation histogram --input images/output/brightness2.png --output images/output/histogram_processed2.csv
./build/unix-debug/pdi_lab --operation histogram --input images/output/contrast1.png --output images/output/histogram_processed2-1.csv
./build/unix-debug/pdi_lab --operation histogram --input images/output/contrast2.png --output images/output/histogram_processed2-2.csv
./build/unix-debug/pdi_lab --operation histogram --input images/output/contrast3.png --output images/output/histogram_processed2-3.csv