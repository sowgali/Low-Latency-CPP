CMAKE=$(which cmake)
NINJA=$(which ninja)

mkdir -p cmake_build_release
$CMAKE -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=$NINJA -G Ninja -S . -B cmake_build_release

$CMAKE --build cmake_build_release --target clean -j 4
$CMAKE --build cmake_build_release --target all -j 4