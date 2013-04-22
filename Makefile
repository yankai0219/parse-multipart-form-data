main:main.o mutipart.o 
  g++ -o  Multi main.o mutipart.o

main.o: main.cpp
	g++ -c -g main.cpp

mutipart.o:mutipart.cpp mutipart.h
	g++ -c -g mutipart.cpp

clean:
	rm -rf  Multi main.o mutipart.o
test:
	valgrind --leak-check=yes ./Multi
