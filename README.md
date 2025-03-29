# OS-project1-Zexi-Yin

# Overview: Solve the producer-consumer problem.

# Idea of producer.cpp
The producer’s job is to generate random numbers and place them into a shared memory buffer (buffer[2]). The buffer only holds two items, so the producer must wait if the buffer is full.

A shared memory object /shared_mem_alt is created and mapped using shm_open() + mmap().

A SharedBuffer struct is placed in this shared memory.

Semaphores:
  empty: how many empty slots are available (starts at 2).
  full: how many slots are filled (starts at 0).
  mutex: ensures mutual exclusion when modifying the buffer.

# Idea of consumer.cpp
The consumer retrieves numbers from the shared buffer, prints them, and clears the slot. If the buffer is empty, it waits until the producer fills it.

It accesses the same shared memory object /shared_mem_alt via shm_open() + mmap().

It works on the same SharedBuffer struct (defined in both producer and consumer).

It consumes up to 6 items that the producer adds.

# Example output: ![Screenshot_20250328_230058](https://github.com/user-attachments/assets/130ae570-2d63-450e-a867-da95e2b8b1e9)

# Compilation
I used c++ to finish this project, the environment is Linux, use following commends to complie:

g++ producer.cpp -pthread -lrt -o producer

g++ consumer.cpp -pthread -lrt -o consumer

./producer & ./consumer
