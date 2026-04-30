cd ../build &&
sh build.sh &&
cd ../_EXAMPLE &&
c++ -O2 -march=x86-64-v2 main.cpp ../build/libgaudio.a -o EXAMPLE