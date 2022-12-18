all: mandel_omp

mandel_omp: mandel_omp.c pngwriter.c
	gcc -fopenmp -o $@ -I. -O3 $^ -lpng

clean:
	rm -rf mandel_omp