cmake -B build
cmake --build build

mkdir -p output
build/COURSEWORK testcases/cornell_box_bunny.txt output/bunny_scene.bmp