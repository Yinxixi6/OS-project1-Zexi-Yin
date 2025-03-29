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

// Define a struct to hold the shared memory data
struct SharedBuffer {
    int buffer[2];
    int index = 0;
    sem_t full;
    sem_t empty;
    sem_t mutex;
};

int main() {
    const char* shm_name = "/shared_mem_alt";

// Ensure no previous shared memory object remains
    shm_unlink(shm_name);

// Create the shared memory object
    int shm_fd = shm_open(shm_name, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Failed to create shared memory." << std::endl;
        return EXIT_FAILURE;
    }

// Allocate space for the shared structure
    if (ftruncate(shm_fd, sizeof(SharedBuffer)) == -1) {
        std::cerr << "Failed to allocate shared memory size." << std::endl;
        return EXIT_FAILURE;
    }

// Map the shared memory to the process's address space
    SharedBuffer* buffer = static_cast<SharedBuffer*>(
        mmap(nullptr, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));

    if (buffer == MAP_FAILED) {
        perror("mmap");
        return EXIT_FAILURE;
    }

// Initialize semaphores: empty (2 slots), full (0 used), mutex (unlocked)
    sem_init(&buffer->empty, 1, 2);
    sem_init(&buffer->full, 1, 0);
    sem_init(&buffer->mutex, 1, 1);

    std::srand(std::time(nullptr));

// Main production loop (produce 6 items)
    for (int i = 0; i < 6; ++i) {
        sem_wait(&buffer->empty);
        sem_wait(&buffer->mutex);

        int value = rand() % 100;
        buffer->buffer[buffer->index] = value;

        std::cout << "Producer: " << value << std::endl;
        buffer->index++;

        sem_post(&buffer->mutex);
        sem_post(&buffer->full);
    }

    return EXIT_SUCCESS;
}
