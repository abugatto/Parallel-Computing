#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "common.h"

double size;

//
//  tuned constants
//
#define density 0.0005
#define mass    0.01
#define cutoff  0.01
#define min_r   (cutoff/100)
#define dt      0.0005

//
//  timer
//
double read_timer( )
{
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

//
//  keep density constant
//
void set_size( int n )
{
    size = sqrt( density * n );
}

//
//  Initialize the particle positions and velocities
//
void init_particles( int n, particle_t *p )
{
    srand48( time( NULL ) );
        
    int sx = (int) ceil(sqrt((double) n));
    int sy = (n+sx-1)/sx;
    
    int *shuffle = (int*) malloc( n * sizeof(int) );
    for( int i = 0; i < n; i++ )
        shuffle[i] = i;
    
    for( int i = 0; i < n; i++ ) 
    {
        //
        //  make sure particles are not spatially sorted
        //
        int j = lrand48()%(n-i);
        int k = shuffle[j];
        shuffle[j] = shuffle[n-i-1];
        
        //
        //  distribute particles evenly to ensure proper spacing
        //

        p[i].x = size*(1.+(k%sx))/(1+sx);
        p[i].y = size*(1.+(k/sx))/(1+sy);

        //
        //  assign random velocities within a bound
        //
=
        p[i].vx = drand48()*2-1;
        p[i].vy = drand48()*2-1;
    }
    free( shuffle );
}

//
//  get block that particle is in
//
void get_block(int& block_num, int* blocks, particle_t& particle, int& idx) {
    //check if computed
    if(block[idx] != -1) {
        return;
    }

    //get block
    int bx = floor(particle.x/block_num);
    int by = floor(particle.y/block_num);
    blocks[idx] = (bx-1)*block_num - by;
}

// //
// //  Check if particles are in adjecent ghost regions
// //
// bool adjacent_ghosts(int& block_num, bool* adj[n], int* blocks, double& ghostbnd, particle_t* particles, int& i, int& j) {
//     //check for adjacency
//     bool vertical_adj = false, horizontal_adj = false;
//     if(adj[blocks[i]][blocks[j]] == true) {
//         //only check right half of matrix
//         int bi = i;
//         int bj = j;
//         if(bi < bj) {
//             int t = bj;
//             bj = bi;
//             bi = t;
//         }

//         //vertical else horizontal
//         if(bj == bi || bj == bi+1 || bj == bi+2) {
//             vertical_adj = true;

//             //if both
//             if(bj == bi || bj == bi+2) { 
//                 horizontal_adj = true;
//             }
//         } else {
//             horizontal_adj = true;
//         }
//     } else {
//         return false;
//     }

//     //get block interval
//     double interval = size / block_num;

//     //Check that particle is in ghost region
//     bool is_ghost = false;
//     for(int p = 0; p < 2; p++) {
//         int idx = 0; 
//         if(p == 1) {
//             idx = 1;
//         }

//         //get points
//         double x = particles[idx].x;
//         double y = particles[idx].y;

//         //check bottom-top bounds
//         if(vertical_adj) {
//             double xbl = blocks[i]*interval;
//             double xbu = xbl + ghostbnd;
//             double xtu = blocks[i-1]*interval;
//             double xtl = xtu - ghostbnd; 
//             if((x <= xbu && x >= xbl) || (x <= xtu && x >= xtl)) {
//                 is_ghost == true;
//             }
//         }

//         //check left-right bounds
//         if(horizontal_adj) {
//             double ybl = blocks[j]*interval;
//             double ybu = ybl + ghostbnd;
//             double ytu = blocks[j+1]*interval;
//             double ytl = ytu - ghostbnd;
//             if((y <= ybu && y >= ybl) || (y <= ytu && y >= ytl)) {
//                 is_ghost = true;
//             }
//         }
//     }

//     return is_ghost;
// }

//
//  interact two particles
//
void apply_force( particle_t &particle, particle_t &neighbor , double *dmin, double *davg, int *navg)
{

    double dx = neighbor.x - particle.x;
    double dy = neighbor.y - particle.y;
    double r2 = dx * dx + dy * dy;
    if( r2 > cutoff*cutoff )
        return;
	if (r2 != 0)
        {
	   if (r2/(cutoff*cutoff) < *dmin * (*dmin))
	      *dmin = sqrt(r2)/cutoff;
           (*davg) += sqrt(r2)/cutoff;
           (*navg) ++;
        }
		
    r2 = fmax( r2, min_r*min_r );
    double r = sqrt( r2 );
 
    
	
    //
    //  very simple short-range repulsive force
    //
    double coef = ( 1 - cutoff / r ) / r2 / mass;
    particle.ax += coef * dx;
    particle.ay += coef * dy;
}

//
//  integrate the ODE
//
void move( particle_t &p )
{
    //
    //  slightly simplified Velocity Verlet integration
    //  conserves energy better than explicit Euler method
    //
    p.vx += p.ax * dt;
    p.vy += p.ay * dt;
    p.x  += p.vx * dt;
    p.y  += p.vy * dt;

    //
    //  bounce from walls
    //
    while( p.x < 0 || p.x > size )
    {
        p.x  = p.x < 0 ? -p.x : 2*size-p.x;
        p.vx = -p.vx;
    }
    while( p.y < 0 || p.y > size )
    {
        p.y  = p.y < 0 ? -p.y : 2*size-p.y;
        p.vy = -p.vy;
    }
}

//
//  I/O routines
//
void save( FILE *f, int n, particle_t *p )
{
    static bool first = true;
    if( first )
    {
        fprintf( f, "%d %g\n", n, size );
        first = false;
    }
    for( int i = 0; i < n; i++ )
        fprintf( f, "%g %g\n", p[i].x, p[i].y );
}

//
//  command line option processing
//
int find_option( int argc, char **argv, const char *option )
{
    for( int i = 1; i < argc; i++ )
        if( strcmp( argv[i], option ) == 0 )
            return i;
    return -1;
}

int read_int( int argc, char **argv, const char *option, int default_value )
{
    int iplace = find_option( argc, argv, option );
    if( iplace >= 0 && iplace < argc-1 )
        return atoi( argv[iplace+1] );
    return default_value;
}

char *read_string( int argc, char **argv, const char *option, char *default_value )
{
    int iplace = find_option( argc, argv, option );
    if( iplace >= 0 && iplace < argc-1 )
        return argv[iplace+1];
    return default_value;
}
