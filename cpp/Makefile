all: a.out

go: a.out
	./a.out

a.out: lodepng.o
	g++ -g ray.cpp lodepng.o
opt: lodepng.o
	g++ -O3 ray.cpp lodepng.o
lodepng.o: lodepng.cpp
	g++ -c lodepng.cpp
