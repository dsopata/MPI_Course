#include<iostream>
#include<iomanip>
#include"Simulation.h"

using namespace std;

void calcCummulative( double *probability ) {
    double sum = 0.0;
    for ( int i = 0; i < 5; i++ )
        sum += probability[ i ];

    for ( int i = 0; i < 5; i++ )
        probability[ i ] /= sum;

    probability[ 1 ] += probability[ 0 ];
    probability[ 2 ] += probability[ 1 ];
    probability[ 3 ] += probability[ 2 ];
    probability[ 4 ] += probability[ 3 ];
}

void copy( double *src, double *dst ) {
    for ( int i = 0; i < 5; i++ ) {
        dst[ i ] = src[ i ];
    }
}

void fill( double *pattern, double **destination, long idxFirst, long idxLast ) {
    for ( long i = idxFirst; i < idxLast; i++ ) {
        copy( pattern, destination[ i ] );
    }
}

int main( int ac, char **av ) {
    long particles = 50000;
    int repetitions = 10;

    MyMPI *mmpi = new MyMPI();
    mmpi->MPI_Init( &ac, &av );

    Simulation *s = new Simulation( particles, mmpi );

    int myRank;
    mmpi->MPI_Comm_rank( MPI_COMM_WORLD, &myRank );

    if ( myRank == 0 ) {
        double pattern1[ 5 ] = { 2, 1.005, 1, 1, 1 };
        double pattern2[ 5 ] = { 2, 0.995, 1, 1, 1 };

        calcCummulative( pattern1 );
        calcCummulative( pattern2 );

// alokacja pamięci
        double ** probability4particles = new double * [ particles ];
        for ( long i = 0; i < particles; i++ ) {
            probability4particles[ i ] = new double [5];
        }

// przygotowanie prawdopodobienstw ruchu cząstek
        fill( pattern1, probability4particles, 0, particles/2 );
        fill( pattern2, probability4particles, particles/2, particles );
/*
    for ( long i = 0; i < particles; i++ ) {
        cout << "P(" << i << ") = [" ;
        for ( int j = 0; j < 5; j++ )
          cout << probability4particles[i][j] << "," ;
        cout << "]" << endl;
    }
*/
        s->setCumulativeProbability( probability4particles );
    }

    double v1, v2;
    double dT_sum, nT_sum, total_sum;
    double dT_start, nT_start;

    dT_sum = nT_sum = total_sum = 0.0;

    s->init();
    for ( int i = 0; i < repetitions; i++ ) {

        if ( myRank == 0 ) {
            dT_start = mmpi->MPI_Wtime();
        }

        s->doIt( 2000 );
        s->doIt( 2000 );    // doIt może być wykonane wielokrotnie

        if ( myRank == 0 ) {
            dT_sum += mmpi->MPI_Wtime() - dT_start;
            nT_start = mmpi->MPI_Wtime();
        }

        s->calcAvgNumberOfPointsBetween(0,500);

        if ( myRank == 0 ) {
            v1 = s->getAvgNumberOfPointsBetween();
        }

        s->calcAvgNumberOfPointsBetween(501,20000);

        if ( myRank == 0 ) {
            nT_sum += mmpi->MPI_Wtime() - nT_start;
            total_sum += mmpi->MPI_Wtime() - dT_start;

            v2 = s->getAvgNumberOfPointsBetween();
            cout << " <x> = " << setiosflags (ios::showbase) << setw(7) << setprecision(3) << s->avgX()
                 << " <y> = " << setiosflags (ios::showbase) << setw(7) << setprecision(3) << s->avgY()
                 << " v1 = "  << setiosflags (ios::showbase | ios::showpoint | ios::fixed) << setw(8) << setprecision(1) << v1
                 << " v2 = "  << setiosflags (ios::showbase | ios::showpoint | ios::fixed) << setw(8) << setprecision(1) << v2 << endl;
        }
    }

    if ( myRank == 0 ) {
        cout << setw(7) << setprecision(3);
        cout << "doIt() avg time .......................  " << dT_sum / repetitions << endl;
        cout << "calcAvgNumberOfPointsBetween() avg time  " << nT_sum / repetitions << endl;
        cout << "Total avg time ........................  " << total_sum / repetitions << endl;
    }

    mmpi->MPI_Finalize();
}
