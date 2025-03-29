#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <ctime>

// Define the same shared structure used by the producer
struct SharedBuffer {
    int buffer[2];
    int index = 0;
    sem_t full;
    sem_t empty;
    sem_t mutex;
};

int main() {
    const char* shm_name = "/shared_mem_alt";

// Open existing shared memory (created by the producer)
    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to open shared memory." << std::endl;
        return EXIT_FAILURE;
    }

// Map the shared memory into the process's address space
    SharedBuffer* buffer = static_cast<SharedBuffer*>(
        mmap(nullptr, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));

    if (buffer == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

// Main consumer loop
    for (int i = 0; i < 6; ++i) {
        sem_wait(&buffer->full);
        sem_wait(&buffer->mutex);

        buffer->index--;
        int item = buffer->buffer[buffer->index];

        std::cout << "[Consumer] Buffer: ";
        for (int j = 0; j < 2; ++j) {
            std::cout << buffer->buffer[j] << " ";
        }
        std::cout << std::endl;

        buffer->buffer[buffer->index] = 0;
        std::cout << "[Consumer] Consumed: " << item << std::endl;

        sem_post(&buffer->empty);
        sem_post(&buffer->mutex);
    }

    shm_unlink(shm_name);

    return EXIT_SUCCESS;
}
