#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "common.h"

//
//  benchmarking program
//
int main( int argc, char **argv )
{    
    int navg,nabsavg=0, block_num=4;
    double davg,dmin, absmin=1.0, absavg=0.0, ghostbnd = .04;

    if( find_option( argc, argv, "-h" ) >= 0 )
    {
        printf( "Options:\n" );
        printf( "-h to see this help\n" );
        printf( "-n <int> to set the number of particles\n" );
        printf( "-o <filename> to specify the output file name\n" );
        printf( "-s <filename> to specify a summary file name\n" );
        printf( "-no turns off all correctness checks and particle output\n");
        return 0;
    }
    
    int n = read_int( argc, argv, "-n", 1000 );

    char *savename = read_string( argc, argv, "-o", NULL );
    char *sumname = read_string( argc, argv, "-s", NULL );
    
    FILE *fsave = savename ? fopen( savename, "w" ) : NULL;
    FILE *fsum = sumname ? fopen ( sumname, "a" ) : NULL;

    particle_t *particles = (particle_t*) malloc( n * sizeof(particle_t) );
    set_size( n );
    init_particles( n, particles );
    
    //
    //  simulate a number of time steps
    //
    double simulation_time = read_timer( );
	
    for( int step = 0; step < NSTEPS; step++ )
    {
	navg = 0;
        davg = 0.0;
	dmin = 1.0;

        //init blocks array
        int blocks[n];
        for(int i = 0; i < n; i++) {
            blocks[i] = -1;
        }

        // //init adjacency matrix
        // int adj[n][n];
        // for(int i = 0; i < n; i++) {
        //     for(int j = 0; j < n; j++) {
        //         if(i != j) {
        //             //check for adjacencies
        //             bool hadj = (j == i-1) && (j % n != 0);
        //             bool vadj = 
        //             bool 
        //             if(adjacent) {
        //                 adj[i][j] = 1;
        //                 adj[j][i] = 1;
        //             } else {
        //                 adj[i][j] = 0;
        //                 adj[j][i] = 0;
        //             }
        //         } else {
        //             adj[i][j] = 0;
        //             adj[j][i] = 0;
        //         }
        //     }
        // }

        // //print
        // std::cout << std::endl;
        // for(int i = 0; i < n; i++) {
        //     for(int j = 0; j < n; j++) {
        //         std::cout << adj[i][j];

        //         if(j == n-1) {
        //             std::cout << " ";
        //         }
        //     }

        //     std::cout << std::endl;
        // }

        //
        //  compute forces
        //
        for( int i = 0; i < n; i++ ) {
            particles[i].ax = particles[i].ay = 0;
            get_block(blocks, particles[i], i);

            for (int j = 0; j < n; j++ ) {
                get_block(block_num, blocks, particles[i], j);

                if(blocks[i] == blocks[j]) {
				    apply_force( particles[i], particles[j],&dmin,&davg,&navg);
                } 
                // else {
                //     if(adjacent_ghosts(block_num, &adj, blocks, ghostbnd, particles, i, j)) {
                //         apply_force( particles[i], particles[j],&dmin,&davg,&navg);
                //     }
                // }
            }
        }
 
        //
        //  move particles
        //
        for( int i = 0; i < n; i++ ) 
            move( particles[i] );		

        if( find_option( argc, argv, "-no" ) == -1 )
        {
          //
          // Computing statistical data
          //
          if (navg) {
            absavg +=  davg/navg;
            nabsavg++;
          }
          if (dmin < absmin) absmin = dmin;
		
          //
          //  save if necessary
          //
          if( fsave && (step%SAVEFREQ) == 0 )
              save( fsave, n, particles );
        }
    }
    simulation_time = read_timer( ) - simulation_time;
    
    printf( "n = %d, simulation time = %g seconds", n, simulation_time);

    if( find_option( argc, argv, "-no" ) == -1 )
    {
      if (nabsavg) absavg /= nabsavg;
    // 
    //  -the minimum distance absmin between 2 particles during the run of the simulation
    //  -A Correct simulation will have particles stay at greater than 0.4 (of cutoff) with typical values between .7-.8
    //  -A simulation were particles don't interact correctly will be less than 0.4 (of cutoff) with typical values between .01-.05
    //
    //  -The average distance absavg is ~.95 when most particles are interacting correctly and ~.66 when no particles are interacting
    //
    printf( ", absmin = %lf, absavg = %lf", absmin, absavg);
    if (absmin < 0.4) printf ("\nThe minimum distance is below 0.4 meaning that some particle is not interacting");
    if (absavg < 0.8) printf ("\nThe average distance is below 0.8 meaning that most particles are not interacting");
    }
    printf("\n");     

    //
    // Printing summary data
    //
    if( fsum) 
        fprintf(fsum,"%d %g\n",n,simulation_time);
 
    //
    // Clearing space
    //
    if( fsum )
        fclose( fsum );    
    free( particles );
    if( fsave )
        fclose( fsave );
    
    return 0;
}
