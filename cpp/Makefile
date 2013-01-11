all: a.out

go: a.out
	./a.out
	cp scene.png images/`date +%Y%m%d-%H%M%S`.png

a.out: lodepng.o ray.cpp
	g++ --std=c++0x -g ray.cpp lodepng.o
opt: lodepng.o ray.cpp
	g++ --std=c++0x  -O3 ray.cpp lodepng.o
lodepng.o: lodepng.cpp
	g++ -c lodepng.cpp
