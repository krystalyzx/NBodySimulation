#include "mpi.h"
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <math.h>

#define DELTA 1
int rank, size; 
double** nvalues;
int mod(int a, int b){
    int r = a % b;
    return r < 0 ? r + b : r;
}
int next(int cur){
    return mod((cur + 1),size);
}
int prev(int cur){
    return mod((cur - 1), size);
}

void print_body(int index){
      printf("%d x: %f y: %f, mass: %f\n", rank, nvalues[index][0],nvalues[index][1] ,nvalues[index][2]);
}

int main( int argc, char *argv[] ){
    if (argc <3) {
        printf("Incorrect number of arguments\n");
        return  0;
    }
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    int round = atoi(argv[1]);
    char* fname = argv[2];
    int mynext = next(rank);
    int myprev = prev(rank);
    char* buf = NULL;
    int bsize = 0;
    nvalues = malloc(size * sizeof(double*));
    int i,j;
    for(i = 0; i < size; i++){
        nvalues[i] = malloc(3 * sizeof(double));
    }
    FILE * outfp;
    //INITIALIZATION
    if (rank == 0){
        FILE *fp = fopen(fname, "r");
        if (fp == NULL){
            perror("Unable to open file\n");
        }

        fseek(fp, 0, SEEK_END); // seek to end of file
        bsize = ftell(fp) + 1; // get current file pointer
        fseek(fp, 0, SEEK_SET); 
        buf = calloc(1,bsize);
        
        if(fgets(buf,bsize,fp)!= NULL){
            //printf("%d %s\n",rank,buf);
            char* ptr = buf;
            nvalues[rank][0] = atof(strsep(&ptr, " "));
            nvalues[rank][1] = atof(strsep(&ptr, " "));
            nvalues[rank][2] = atof(strsep(&ptr, " "));
            print_body(rank);
            memset(buf, 0,bsize);
        }

        fclose(fp);
        outfp = fopen("out_data.txt", "w+");
        if(outfp == NULL){
            printf("unable to open output file\n");
        }
        if(outfp!= NULL){
             fprintf(outfp, "%d\n", size);
        }
       


    }
    else{
        MPI_Recv(&bsize, 1, MPI_INT, myprev, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        //printf("%d receive: %d\n",rank, bsize);
        buf = calloc(bsize, 1);
        MPI_Recv(buf, bsize, MPI_CHAR, myprev, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        //printf("%d receive: %s\n",rank, buf);
        char* ptr = buf;
        nvalues[rank][0] = atof(strsep(&ptr, " "));
        nvalues[rank][1] = atof(strsep(&ptr, " "));
        nvalues[rank][2] = atof(strsep(&ptr, "\n"));
        printf("%d initial value\n", rank);
        print_body(rank);
        //printf("%d %s\n", rank,ptr);
        if(ptr != NULL){
            int cpysize = buf + bsize - ptr;
            memmove(buf, ptr,cpysize);
            memset(ptr + cpysize, 0, bsize - cpysize);
            
        }else{
            memset(buf, 0, bsize);
        }
        //printf("%d rest: %s\n", rank,buf);
    }

    if (rank != size - 1){
        MPI_Send(&bsize, 1, MPI_INT, mynext,0, MPI_COMM_WORLD);
        MPI_Send(buf, strlen(buf), MPI_CHAR, mynext,0, MPI_COMM_WORLD);
    }
    free(buf);
    double vx = 0;
    double vy = 0;
    double G = 6.6743e-11;
    //double t_delta = 1.0;
    
    for (i = 0; i <= round; i++){
        //SIMULATION
        int last_value_from = rank;
        int value_from = prev(rank);
        double fx = 0;
        double fy = 0;
        double x = nvalues[rank][0];
        double y = nvalues[rank][1];
        double mass = nvalues[rank][2];
        while (value_from != rank){
            //printf("%d send body value %d\n", rank, last_value_from);
            MPI_Send(nvalues[last_value_from], 3, MPI_DOUBLE, mynext,0, MPI_COMM_WORLD);
            MPI_Recv(nvalues[value_from], 3, MPI_DOUBLE, myprev, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            //printf("%d body value from %d\n", rank, value_from);
            //print_body(value_from);
            last_value_from = value_from;
            value_from = prev(value_from);
        }
        if(rank == 0){
            if(outfp!=NULL){
                for(j = 0; j < size; j++){
                fprintf(outfp, "%f %f %f\n",nvalues[j][0],nvalues[j][1], nvalues[j][2]);
            }
            }
            
            if (i == round){
                //TERMINATION
                for(j = 0; j < size; j++){
                    printf("%f %f %f\n",nvalues[j][0],nvalues[j][1], nvalues[j][2]);
                }
                fclose(outfp);  
                break;
            }
        }

        for( j = 0; j < size; j++){
            if(j != rank){
                double dx = nvalues[j][0] - x;
                double dy = nvalues[j][1] - y;
                double r_square = pow(dx,2) + pow(dy, 2);
                double r = sqrt(r_square);
                double F = (G * mass * nvalues[j][2])/ r_square;
                fx = fx + F * dx / r;
                fy = fy + F * dy / r;
            }
        }
        vx = vx + (fx * DELTA)/ mass;
        vy = vy + (fy * DELTA) / mass;
        nvalues[rank][0] = nvalues[rank][0] + vx * DELTA;
        nvalues[rank][1] = nvalues[rank][1] + vy * DELTA;
    }

    for(i = 0; i < size; i++){
        free(nvalues[i]);
    }
    free(nvalues);
    MPI_Finalize();
    return 0;
}
