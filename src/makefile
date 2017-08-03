all : cube

clean :
	rm -rvf *.o cube *~

Cube.o: Cube.cpp Cube.h
	g++ -ggdb3 -std=c++11 -c Cube.cpp -I . -o Cube.o

glad.o: glad.c ../include/glad/glad.h
	g++ -ggdb3 -c glad.c -I ../include -o glad.o

rubutils.o : rubutils.cpp rubutils.h
	g++ -ggdb3 -std=c++11 -c rubutils.cpp -I ../include -o rubutils.o

main.o : main.cpp 
	g++ -ggdb3 -std=c++11 -c main.cpp -o main.o -I ../include

cube : main.o rubutils.o glad.o Cube.o ncube_renderer.o
	g++ -ggdb3 -std=c++11 glad.o Cube.o rubutils.o main.o ncube_renderer.o -o cube -lglfw -ldl -lpthread

ncube_renderer.o: ncube_renderer.cc ncube_renderer.h
	g++ -ggdb3 -std=c++11 -c ncube_renderer.cc -o ncube_renderer.o -I ../include
