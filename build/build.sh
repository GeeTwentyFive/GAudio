c++ -O2 -march=x86-64-v2 -c ../GAudio.cpp -o GAudio.o &&
ar rcs libgaudio.a GAudio.o
rm GAudio.o