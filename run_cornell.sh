cmake -B build
cmake --build build

mkdir -p output
build/COURSEWORK testcases/cornell_box.txt output/cornell_scene.bmp