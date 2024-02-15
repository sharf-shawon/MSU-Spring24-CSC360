#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SEQUENCE_LENGTH 1000 // Maximum sequence length

// Structure for passing arguments to the thread
struct ThreadArgs
{
    long long *sequence;
    int sequence_length;
};

// Function to generate Fibonacci sequence
void *generate_fibonacci(void *args)
{
    struct ThreadArgs *thread_args = (struct ThreadArgs *)args;
    long long *sequence = thread_args->sequence;
    int length = thread_args->sequence_length;

    sequence[0] = 0;
    if (length > 1)
        sequence[1] = 1;
    for (int i = 2; i < length; i++)
    {
        sequence[i] = sequence[i - 1] + sequence[i - 2];
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <sequence_length>\n", argv[0]);
        return 1;
    }

    int sequence_length = atoi(argv[1]);
    if (sequence_length <= 0 || sequence_length > MAX_SEQUENCE_LENGTH)
    {
        printf("Sequence length must be a positive integer less than or equal to %d\n", MAX_SEQUENCE_LENGTH);
        return 1;
    }

    unsigned long long sequence[MAX_SEQUENCE_LENGTH];

    // Initialize pthread variables
    pthread_t tid;
    struct ThreadArgs thread_args;
    thread_args.sequence = sequence;
    thread_args.sequence_length = sequence_length;

    // Create a thread to generate the Fibonacci sequence
    if (pthread_create(&tid, NULL, generate_fibonacci, (void *)&thread_args) != 0)
    {
        perror("pthread_create");
        return 1;
    }

    // Wait for the thread to finish
    if (pthread_join(tid, NULL) != 0)
    {
        perror("pthread_join");
        return 1;
    }

    // Output the generated Fibonacci sequence
    printf("Generated Fibonacci sequence:\n");
    for (int i = 0; i < sequence_length; i++)
    {
        printf("%llu ", sequence[i]);
    }
    printf("\n");

    return 0;
}
