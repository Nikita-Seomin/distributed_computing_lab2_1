#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>

#define FIRST_THREAD 0

double *resVector, *matrixAndVector;

int readDataFromCons(){
    //read size of matrix
    int height = 0;
    scanf( "%d", &height);
    resVector = (double *) malloc((height + 1) * sizeof(double));
    matrixAndVector = (double *) malloc((height * height + height) * sizeof(double));

    //read matrix and vector
    for (int i = 0; i < height * height + height; i++)
        scanf( "%lf", &matrixAndVector[i]);
    return height;
}

double multiplicationRowAndVector(int rowNum,int height, double* arr){

    int startEl = (rowNum - 1) * height;
    int endEl = (rowNum - 1) * height + height;
    double res = 0;
    for (int i = startEl, j = height * height; i < endEl; ++i, ++j){
        res += arr[i] * arr[j];
    }
    return res;
}

int main(int argc, char **argv)
{
    clock_t before;
    int rank, size;
    MPI_Init(&argc, &argv);
    int height;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == FIRST_THREAD) {
        height = readDataFromCons();
        before = clock();
        for (int i = 1; i < height + 1; ++i) {
            MPI_Status status;
            MPI_Send(matrixAndVector, height * height + height, MPI_DOUBLE, i, 1, MPI_COMM_WORLD);
        }
        for (int i = 1; i < height + 1; ++i){
            MPI_Status status;
            double buf[1];
            int num;
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &num);
            MPI_Recv(buf, num, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &status);
            printf("%d %d\n", num, i);
            resVector[i] = buf[0];
        }
        for (int i = 1; i < height + 1; ++i)
            printf("%lf\n", resVector[i]);

        clock_t difference = clock() - before;
        printf("Processor time taken in pow function: %f "
               "seconds\n",
               (float)difference / CLOCKS_PER_SEC);
    }
    else
    {
        int count;
        MPI_Status status;
        double out[1];
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);
        count /= 2;
        count = (int) (-1 + sqrt(1 + 4 * count)) / 2;
        double buf[count * count + count];
        MPI_Recv(buf, count * count + count, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        out[0] = multiplicationRowAndVector(rank, count, buf);
        MPI_Send(out, rank , MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();


    return 0;
}
