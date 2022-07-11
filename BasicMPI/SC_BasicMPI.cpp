#include <mpi.h>
#include "windows.h"
#include <iostream>

using namespace std;

void PointToPoint() {
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int number;
    if (world_rank == 0) {
        number = 20;
        MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }
    else if(world_rank == 1) {
        int y;
        MPI_Recv(&y, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
        MPI_STATUS_IGNORE);
        printf("Process 1 received number %d from process 0\n", y);
    }
}

void NonBlocking() {
    int rank, size;
    int tag, destination, count;
    int buffer; //value to send
    MPI_Status status;
    MPI_Request request = MPI_REQUEST_NULL;
    MPI_Comm_size(MPI_COMM_WORLD, &size); //number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //rank of current process
    tag = 1234;
    destination = 2; //destination process
    count = 1; //number of elements in buffer
    if (rank == 0) {
        cout << "Enter a value to send to procces " << destination << ": ";
        cin >> buffer;
        MPI_Isend(&buffer, count, MPI_INT, destination, tag, MPI_COMM_WORLD, &request); //non blocking send to destination process
    }
    if (rank == destination)
        MPI_Irecv(&buffer, count, MPI_INT, 0, tag, MPI_COMM_WORLD, &request);
    //destination process receives
    MPI_Wait(&request, &status); //bloks and waits for destination process to receive data
    if (rank == 0)
        cout << "Process " << rank << " sent " << buffer << endl;
    if (rank == destination)
        cout << "Process " << rank << " got " << buffer << endl;
}

void Barrier() {
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    cout << "(Process " << my_rank << "): " << "waiting on the barrier\n";
    MPI_Barrier(MPI_COMM_WORLD);
    cout << "(Process " << my_rank << "): " << "All proccesses waited on the barrier\n";
}

bool IsPrime(std::size_t nr) {
    if (nr < 2)
    return 0;
    for (int i = 2; i <= sqrt(int(nr)); i++)
    if (nr % i == 0)
    return 0;
    return 1;
}

void CountPrimes(int maxNumber) {
    int noPrimes = 0, sum;
    int my_rank;
    int size;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    //cout <<"My rank: " << my_rank << endl;
    for (int i = my_rank; i <= maxNumber; i += size) {
        noPrimes += IsPrime(i);
    }
    MPI_Reduce(&noPrimes, &sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    if (!my_rank) cout << sum << endl;
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    CountPrimes(1000000);
    MPI_Finalize();
    return 0;
}
