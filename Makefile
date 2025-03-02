
2d: bellman_2d.c
	gcc -fopenmp bellman_2d.c -o bellman_2d
	./bellman_2d
	./bellman_2d 999 999
	./bellman_2d 0 999
	./bellman_2d 999 0
	./bellman_2d 500 500
	rm bellman_2d

2d_parallel: bellman_2d_parallel.c
	gcc -fopenmp bellman_2d_parallel.c -o bellman_2d_parallel
	./bellman_2d_parallel
	./bellman_2d_parallel 999 999
	./bellman_2d_parallel 0 999
	./bellman_2d_parallel 999 0
	./bellman_2d_parallel 500 500
	rm bellman_2d_parallel

3d: bellman_3d.c
	gcc -fopenmp bellman_3d.c -o bellman_3d
	./bellman_3d
	./bellman_3d 49 49 49
	./bellman_3d 0 49 25
	./bellman_3d 25 0 49
	./bellman_3d 25 25 25
	rm bellman_3d
	
3d_parallel: bellman_3d_parallel.c
	gcc -fopenmp bellman_3d_parallel.c -o bellman_3d_parallel
	./bellman_3d_parallel
	./bellman_3d_parallel 49 49 49
	./bellman_3d_parallel 0 49 25
	./bellman_3d_parallel 25 0 49
	./bellman_3d_parallel 25 25 25
	rm bellman_3d_parallel

generate_2d: generate_2d_graph.c
	gcc -fopenmp generate_2d_graph.c -o generate_2d_graph.o
	./generate_2d_graph.o 1000 1000

generate_2d_random: generate_2d_graph.c
	gcc -fopenmp generate_2d_graph.c -o generate_2d_graph.o
	./generate_2d_graph.o 1000 1000 random

generate_3d: generate_3d_graph.c
	gcc -fopenmp generate_3d_graph.c -o generate_3d_graph.o
	./generate_3d_graph.o 50 50 50

generate_3d_random: generate_3d_graph.c
	gcc -fopenmp generate_3d_graph.c -o generate_3d_graph.o
	./generate_3d_graph.o 50 50 50 random

generate: generate_2d generate_3d generate_2d_random generate_3d_random

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.txt