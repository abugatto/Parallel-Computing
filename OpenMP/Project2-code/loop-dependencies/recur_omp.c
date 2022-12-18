#include "walltime.h"
#include <math.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[]) {
  int N = 2000000000;
  double up = 1.00000001;
  double Sn = 1.00000001;

  omp_set_dynamic(0);
  int N_init = 100000;
  int N_max = 1000000000;
  int N_step = 10;
  for(int N = N_init; N <= N_max; N *= N_step) {
    /* allocate memory for the recursion */
    double *opt = (double *)malloc((N + 1) * sizeof(double));

    if (opt == NULL)
      die("failed to allocate problem size");

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////

    double time_start = wall_time();

    for (int n = 0; n <= N; ++n) {
        opt[n] = Sn;
        Sn *= up;
    }

    double sequential = wall_time() - time_start;

    printf("Sequential RunTime   :  %f seconds\n", sequential);
    printf("Final Result Sn    :  %.17g \n", Sn);

    double temp = 0.0;
    for (int n = 0; n <= N; ++n) {
      temp += opt[n] * opt[n];
    }
    printf("Result ||opt||^2_2 :  %f\n", temp / (double)N);
    printf("\n");

    double time_serial = wall_time() - time_start;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////

    int p_init = 1;
    int p_max = 24;
    int p_step = 1;
    for(int p = p_init; p <= p_max; p += p_step) {
      omp_set_num_threads(p);
      
      double base = up;
      for (int n = 0; n <= N; ++n) {
        opt[n] = 0;
      }

      time_start = wall_time();

      #pragma omp parallel for firstprivate(base,up) lastprivate(Sn)
      for (int n = 0; n <= N; ++n) {
        Sn = base * pow(up, n);
        opt[n] = Sn;
      }

      double runtime = wall_time() - time_start;
      printf("Parallel RunTime   :  %f seconds\n", runtime);
      printf("Final Result Sn    :  %.17g \n", Sn);

      temp = 0.0;
      for (int n = 0; n <= N; ++n) {
        temp += opt[n] * opt[n];
      }
      printf("Result ||opt||^2_2 :  %f\n", temp / (double)N);
      printf("\n");

      ////////////////////////////////////////////////////
      ////////////////////////////////////////////////////

      FILE* file;
      if(N == N_init && p == p_init) {
        file = fopen("data.csv","w");
      } else {
        file = fopen("data.csv","a");
      }
      fprintf(file, "%d, %d, %f, %f,\n", N, p, sequential, runtime);
      fclose(file); 
    }
  }

  return 0;
}
