from mpi4py import MPI
import numpy as np
import queue
import time

comm = MPI.COMM_WORLD
rank_queue = 0
ARRSIZE = 1000000

class Queue:
    producing = True
    state = MPI.Status()
    def __init__(self, size = 10, no_consumers = 2):
        self.no_consumers = no_consumers
        self.queue = queue.Queue(maxsize = size)
        print("Queue created")
    def __del__(self):
        print("Queue destructed")

    def fakeProd(self, no_arrays):
        for i in range(no_arrays):
            array = np.random.randint(1000, size=ARRSIZE)
            #print("Prod checksum: ", np.sum(array))
            self.queue.put(array)
    
    def Queue(self, no_arrays):
        state = MPI.Status()
        self.fakeProd(no_arrays)
        print("Q size: ", self.queue.qsize())
        while not self.queue.empty():
            Consumer_ready = comm.recv(source = MPI.ANY_SOURCE, tag=2, status = state)
            #print(state.source)
            array = self.queue.get()
            #print("Queue checksum: ",np.sum(array))
            comm.send(array, state.source, 0)
        done = True
        for i in range(self.no_consumers):
            comm.send(done, dest=i+1, tag=1)

        

class Consumer:
    counter = 0
    state = MPI.Status()
    done = False
    def __init__(self, nm):
        self.nm = nm
        print("Consumer", nm,"created")
    def __del__(self):
        print("Consumer",self.nm , "sorted: ", self.counter)

    def consuming(self):
        Consumer_ready = 1
        while not self.done:
            comm.send(Consumer_ready, rank_queue, 2)
            comm.Probe(rank_queue,MPI.ANY_TAG, self.state)
            if self.state.tag == 0:
                array = np.empty(ARRSIZE)
                array = comm.recv(source=rank_queue, tag=0)
                sorted = np.sort(array)
                #print("Consumer checksum: ", np.sum(sorted))
                self.counter = self.counter + 1
            else:
                self.done = comm.recv(None, rank_queue, 1)
            

if __name__ == "__main__":
    MPI.Init
    rank = comm.Get_rank()
    start = time.time()
    if rank == rank_queue:
        que = Queue(6000, 7)
        que.Queue(100)
        end = time.time()
        print("Time: ", end-start, "s")
    if rank == 1:
        cons = Consumer(1)
        cons.consuming()
    if rank == 2:
        cons2 = Consumer(2)
        cons2.consuming()
    if rank == 3:
        cons3 = Consumer(3)
        cons3.consuming()
    if rank == 4:
        cons4 = Consumer(4)
        cons4.consuming()
    if rank == 5:
        cons5 = Consumer(5)
        cons5.consuming()
    if rank == 6:
        cons6 = Consumer(6)
        cons6.consuming()    
    if rank == 7:
        cons7 = Consumer(7)
        cons7.consuming()          
    MPI.Finalize
    