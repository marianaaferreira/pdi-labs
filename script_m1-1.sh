#!/bin/bash

cmake --preset unix-debug
cmake --build --preset unix-debug
ctest --preset unix-debug
./build/unix-debug/pdi_lab --operation inspect --input images/input/m1_checkerboard_256.png --output images/output/inspect.txt
./build/unix-debug/pdi_lab --operation copy --input images/input/m1_color_2x2.png --output images/output/copy.png
./build/unix-debug/pdi_lab --operation channel_b --input images/input/m1_color.png --output images/output/channel_b.png
./build/unix-debug/pdi_lab --operation channel_g --input images/input/m1_color.png --output images/output/channel_g.png
./build/unix-debug/pdi_lab --operation channel_r --input images/input/m1_color.png --output images/output/channel_r.png
./build/unix-debug/pdi_lab --operation grayscale_average --input images/input/m1_color.png --output images/output/grayscale_average.png
./build/unix-debug/pdi_lab --operation grayscale_weighted --input images/input/m1_color.png --output images/output/grayscale_weighted.png
./build/unix-debug/pdi_lab --operation quantize --levels 16 --input images/input/m1_shapes_border_256.png --output images/output/quant_16.png
./build/unix-debug/pdi_lab --operation quantize --levels 8 --input images/input/m1_shapes_border_256.png --output images/output/quant_8.png
./build/unix-debug/pdi_lab --operation quantize --levels 4 --input images/input/m1_shapes_border_256.png --output images/output/quant_4.png
./build/unix-debug/pdi_lab --operation quantize --levels 2 --input images/input/m1_shapes_border_256.png --output images/output/quant_2.png