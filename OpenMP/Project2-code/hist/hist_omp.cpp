#include "walltime.h"
#include <iostream>
#include <random>
#include <omp.h>
#include <fstream>

#define BINS 16

using namespace std;

int main() {
  long N_init = 100000;
  long N_max = 1000000000;
  int N_step = 10;
  for(int N = N_init; N <= N_max; N*=N_step) {
    double time_start, time_end;

    // Initialize random number generator
    unsigned int seed = 123;
    float mean = BINS / 2.0;
    float sigma = BINS / 12.0;
    std::default_random_engine generator(seed);
    std::normal_distribution<float> distribution(mean, sigma);

    // Generate random sequence
    // Note: normal distribution is on interval [-inf; inf]
    //       we want [0; BINS-1]
    int *vec = new int[N];
    for (long i = 0; i < N; ++i) {
      vec[i] = int(distribution(generator));
      if (vec[i] < 0)
        vec[i] = 0;
      if (vec[i] > BINS - 1)
        vec[i] = BINS - 1;
    }

    // Initialize histogram
    // Set all bins to zero
    long dist[BINS];
    for (int i = 0; i < BINS; ++i) {
      dist[i] = 0;
    }

    int p_init = 1;
    int p_max = 24;
    int p_step = 1;

    omp_set_dynamic(0);
    for(int p = p_init; p < p_max; p += p_step) {
      omp_set_num_threads(p);
      time_start = wall_time();

      #pragma omp parallel for reduction(+ : dist)
      for (long i = 0; i < N; ++i) {
        dist[vec[i]]++;
      }
      time_end = wall_time();

      /* // Write results
      for (int i = 0; i < BINS; ++i) {
        cout << "dist[" << i << "]=" << dist[i] << endl;
      } */
      cout << "Time: " << time_end - time_start << " sec" << endl;

      //add results to the csv
      std::ofstream file;
      if(N == N_init && p == p_init) {
        file.open("data.csv", std::ios::trunc);
      } else {
        file.open("data.csv", std::ios::app);
      }
      file << N << ", ";
      file << p << ", ";
      file << time_end - time_start << ", " << endl;
      file.close();
    }

    delete[] vec;
  }
  return 0;
}