#include <omp.h>
#include "walltime.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>

#define NUM_ITERATIONS 100

// Example benchmarks
// 0.008s ~0.8MB
//#define N 100000
// 0.1s ~8MB
// #define N 1000000
// 1.1s ~80MB
// #define N 10000000
// 13s ~800MB
// #define N 100000000
// 127s 16GB
//#define N 1000000000
#define EPSILON 0.1

using namespace std;

int main() {
  //set time
  double time_serial, time_start = 0.0;
  double *a, *b;

  ////////////////////////////////////////////////////////
  //Loop through array sizes
  ////////////////////////////////////////////////////////
  unsigned int N_init = 100000;
  unsigned int N_max = 1000000000;
  unsigned int N_step = 10;

  //Set dynamic adjustment of threads at 0 default
  omp_set_dynamic(0);
  for(int N = N_init; N <= N_max; N *= N_step) {
    // Allocate memory for the vectors as 1-D arrays
    a = new double[N];
    b = new double[N];
    
    // Initialize the vectors with some values
    for (int i = 0; i < N; i++) {
      a[i] = i;
      b[i] = i / 10.0;
    }

    ////////////////////////////////////////////////////////
    // SERIAL REGION
    ////////////////////////////////////////////////////////

    long double alpha = 0;
    // serial execution
    // Note that we do extra iterations to reduce relative timing overhead
    time_start = wall_time();
    for (int iterations = 0; iterations < NUM_ITERATIONS; iterations++) {
      alpha = 0.0;
      for (int i = 0; i < N; i++) {
        alpha += a[i] * b[i];
      }
    }
    time_serial = wall_time() - time_start;

    ////////////////////////////////////////////////////////
    //PARALLEL REGION: with threads p=1:4
    ////////////////////////////////////////////////////////
    int p_init = 1;
    int p_max = 24;
    int p_step = 1;
    for(int p = p_init; p <= p_max; p += p_step) {
      omp_set_num_threads(p);

      //Declare  alpha and time values for parallel region
      long double alpha_parallel_reduction = 0;
      long double alpha_parallel_critical = 0;
      double time_red = 0;
      double time_critical = 0;

      ////////////////////////////////////////////////////////
      // Using reduction pragma
      ////////////////////////////////////////////////////////

      time_start = wall_time();
      for (int iterations = 0; iterations < NUM_ITERATIONS; iterations++) {
        alpha_parallel_reduction = 0.0;

        #pragma omp parallel for reduction(+ : alpha_parallel_reduction)
        for (int i = 0; i < N; i++) {
          alpha_parallel_reduction += a[i] * b[i];
        }
      }
      time_red = wall_time() - time_start;
      
      ////////////////////////////////////////////////////////
      // Using  critical pragma
      ////////////////////////////////////////////////////////

      time_start = wall_time();
      long double partial;
      for (int iterations = 0; iterations < NUM_ITERATIONS; iterations++) {
        alpha_parallel_critical = 0.0;

        #pragma omp parallel private(partial)
        {
          partial = 0.0;
          
          #pragma omp for
          for (int i = 0; i < N; i++) {
            partial += a[i] * b[i];
          }

          #pragma omp critical 
          alpha_parallel_critical += partial;
        }
      }
      time_critical = wall_time() - time_start;

      ////////////////////////////////////////////////////////
      //Print DATA
      ////////////////////////////////////////////////////////

      cout << "Alpha using serial/reduction/critical method = [" 
          << alpha << ", " << alpha_parallel_reduction << ", " << alpha_parallel_critical << "]" << endl;
      
      cout << "Correct serial/reduction/critical methods = [" 
          << (alpha==alpha) << ", "
          << ((fabs(alpha_parallel_reduction - alpha) / fabs(alpha_parallel_reduction)) < EPSILON) << ", " 
          << ((fabs(alpha_parallel_critical - alpha) / fabs(alpha_parallel_critical)) < EPSILON) << "]" << endl;

      cout << "[" << N << ", " << p << "]: " << "Time using serial/reduction/critical method = [" 
          << time_serial << ", " << time_red << ", " << time_critical << "]" << endl;

      ////////////////////////////////////////////////////////
      //Output Data to CSV [arrSize, nThreads, time_serial, time_red, time_critical]
      ////////////////////////////////////////////////////////
      std::ofstream file;
      if(N == N_init && p == p_init) {
        file.open("data.csv", std::ios::trunc);
      } else {
        file.open("data.csv", std::ios::app);
      }
      file << N << ", ";
      file << p << ", ";
      file << time_serial << ", ";
      file << time_red << ", ";
      file << time_critical << ", " << endl;
      file.close();
    }

    // De-allocate memory
    delete[] a;
    delete[] b;
  }

  return 0;
}
