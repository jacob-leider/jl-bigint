main: main.o cases.o testutils.o add_sub_mul.o
	g++ -std=c++11 main.o cases.o testutils.o add_sub_mul.o -o main

cases.o: cases.cpp
	g++ -c -std=c++11 cases.cpp -o cases.o

main.o: main.cpp cases.cpp
	g++ -c -std=c++11 main.cpp -o main.o

testutils.o: ../testutils.c
	gcc -c ../testutils.c -o testutils.o

add_sub_mul.o: ../../src/add_sub_mul.c
	gcc -c ../../src/add_sub_mul.c -o add_sub_mul.o

cases.cpp: gen_tests.py
	python3 gen_tests.py

clean:
	rm cases.*
	rm *.o
	rm main
