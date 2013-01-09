all: a.out

go: a.out
	./a.out

a.out: lodepng.o ray.cpp
	g++ -g ray.cpp lodepng.o
opt: lodepng.o ray.cpp
	g++ -O3 ray.cpp lodepng.o
lodepng.o: lodepng.cpp
	g++ -c lodepng.cpp
