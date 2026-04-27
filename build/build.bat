c++ -O2 -march=x86-64-v2 -c ../GAudio.cpp ../libs/miniaudio/miniaudio.c -o GAudio.o
ar rcs libgaudio.a GAudio.o
del GAudio.o