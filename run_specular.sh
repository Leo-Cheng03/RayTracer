cmake -B build
cmake --build build

mkdir -p output
build/COURSEWORK testcases/cornell_box_specular.txt output/specular_scene.bmp