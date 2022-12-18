# On Euler, we will benchmark your DGEMM's performance against the performance
# of the default vendor-tuned DGEMM. This is done in benchmark-blas.
#


CC = gcc
OPT = -Ofast -m3dnow -funroll-all-loops
CFLAGS = -Wall -std=gnu99 $(OPT)
LDFLAGS = -Wall
# librt is needed for clock_gettime
LDLIBS = -lrt -Wl,--no-as-needed -L${MKLROOT}/lib/intel64 -lmkl_intel_lp64 -lmkl_core -lmkl_sequential -lpthread -lm -ldl -m64 -I${MKLROOT}/include

targets = benchmark-naive benchmark-blocked benchmark-blas benchmark-sizet benchmark-tp benchmark-l2 benchmark-l3 #benchmark-strassen
objects = benchmark.o dgemm-naive.o dgemm-blocked.o dgemm-blas.o dgemm-sizet dgemm-transpose.o dgemm-l2.o dgemm-l3.o #dgemm-strassen.o

.PHONY : default
default : all

.PHONY : all
all : clean $(targets)

benchmark-naive : benchmark.o dgemm-naive.o
	$(CC) -o $@ $^ $(LDLIBS)
benchmark-blocked : benchmark.o dgemm-blocked.o
	$(CC) -o $@ $^ $(LDLIBS)
benchmark-blas : benchmark.o dgemm-blas.o
	$(CC) -o $@ $^ $(LDLIBS)
benchmark-sizet : benchmark.o dgemm-sizet.o
	$(CC) -o $@ $^ $(LDLIBS)
benchmark-tp : benchmark.o dgemm-transpose.o
	$(CC) -o $@ $^ $(LDLIBS)
benchmark-l2 : benchmark.o dgemm-l2.o
	$(CC) -o $@ $^ $(LDLIBS)
benchmark-l3 : benchmark.o dgemm-l3.o
	$(CC) -o $@ $^ $(LDLIBS)
#benchmark-strassen : benchmark.o dgemm-strassen.o
#	$(CC) -o $@ $^ $(LDLIBS)

%.o : %.c
	$(CC) -c $(CFLAGS) $<

.PHONY : clean
clean:
	rm -f $(targets) $(objects)