THREADS = 1 2 5 10 20 28

2d: bellman_2d.c
	gcc -fopenmp bellman_2d.c -o bellman_2d
	./bellman_2d
	./bellman_2d 250 250
	./bellman_2d 250 500
	./bellman_2d 500 250
	./bellman_2d 500 500
	rm bellman_2d

2d_parallel: bellman_2d_parallel.c
	gcc -fopenmp bellman_2d_parallel.c -o bellman_2d_parallel;
	$(foreach THREAD_COUNT, $(THREADS), \
		export OMP_NUM_THREADS=$(THREAD_COUNT); \
		echo "Running 2D parallel with threads: $(THREAD_COUNT)"; \
		./bellman_2d_parallel; \
		./bellman_2d_parallel 250 250 ;\
		./bellman_2d_parallel 250 500; \
		./bellman_2d_parallel 500 250; \
		./bellman_2d_parallel 500 500; \
	)
	rm bellman_2d_parallel

3d: bellman_3d.c
	gcc -fopenmp bellman_3d.c -o bellman_3d
	./bellman_3d
	./bellman_3d 12 12 12
	./bellman_3d 12 12 25
	./bellman_3d 12 25 25
	./bellman_3d 25 25 25
	rm bellman_3d
	
3d_parallel: bellman_3d_parallel.c
	gcc -fopenmp bellman_3d_parallel.c -o bellman_3d_parallel
	$(foreach THREAD_COUNT, $(THREADS), \
		export OMP_NUM_THREADS=$(THREAD_COUNT); \
		echo "Running 2D parallel with threads: $(THREAD_COUNT)"; \
		./bellman_3d_parallel; \
		./bellman_3d_parallel 12 12 12; \
		./bellman_3d_parallel 12 12 25; \
		./bellman_3d_parallel 12 25 25; \
		./bellman_3d_parallel 25 25 25; \
	)
	rm bellman_3d_parallel

generate_2d: generate_2d_graph.c
	gcc -fopenmp generate_2d_graph.c -o generate_2d_graph.o
	./generate_2d_graph.o 1000 1000
	rm generate_2d_graph.o

generate_2d_random: generate_2d_graph.c
	gcc -fopenmp generate_2d_graph.c -o generate_2d_graph.o
	./generate_2d_graph.o 1000 1000 random
	rm generate_2d_graph.o

generate_3d: generate_3d_graph.c
	gcc -fopenmp generate_3d_graph.c -o generate_3d_graph.o
	./generate_3d_graph.o 50 50 50
	rm generate_3d_graph.o

generate_3d_random: generate_3d_graph.c
	gcc -fopenmp generate_3d_graph.c -o generate_3d_graph.o
	./generate_3d_graph.o 50 50 50 random
	rm generate_3d_graph.o

generate: generate_2d generate_3d generate_2d_random generate_3d_random

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.txt