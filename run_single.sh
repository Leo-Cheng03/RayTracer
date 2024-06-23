cmake -B build
cmake --build build

mkdir -p output
build/COURSEWORK testcases/cornell_box_bunny.txt output/cornell_bunny_scene.bmp