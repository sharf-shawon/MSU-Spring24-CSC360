// CSC360 : OS Project 4 Spring 2024
// Author : Mohammed Sharfuddin Shawon
// Date   : 02/14/2024

#include "buffer.c"
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/queue.h>

sem_t sem_mutex;
buffer myBuffer;

int simulationTime;
int maxThreadSleepTime;
int producerThreads;
int consumerThreads;
int bufferSize;

int rLoc = 0; // Buffer read location
int wLoc = 0; // Buffer write location

int itemsProduced = 0;
int itemsConsumed = 0;
int bufferFull = 0;
int bufferEmpty = 0;

bool stopProducer = false;
bool stopConsumer = false;

void *producer(void *param);
void *consumer(void *param);

int main(int argc, char *argv[], void *param)
{
  if (argc != 6)
  {
    printf("usage: a.out <integer simulationTime> <integer maxThreadSleepTime> <integer producerThreads> <integer consumerThreads> <integer bufferSize>\n");
    return -1;
  }
  simulationTime = atoi(argv[1]);
  maxThreadSleepTime = atoi(argv[2]);
  producerThreads = atoi(argv[3]);
  consumerThreads = atoi(argv[4]);
  bufferSize = atoi(argv[5]);

  // Using one variable to keep continuous numbering of threads for the report
  int threadCounts[producerThreads + consumerThreads];

  sem_init(&sem_mutex, 0, 1);
  initBuffer(&myBuffer, bufferSize);

  pthread_t ptid[producerThreads];
  pthread_t ctid[consumerThreads];
  pthread_attr_t attr;

  printf("Starting Threads...\n");
  pthread_attr_init(&attr);

  for (int i = 0; i < producerThreads; i++)
  {
    threadCounts[i] = 0;
    pthread_create(&ptid[i], &attr, producer, (void *)&threadCounts[i]);
  }
  sleep(1);

  for (int i = 0; i < consumerThreads; i++)
  {
    // Since we are using one variable to keep count of both producers and consumers
    threadCounts[producerThreads + i] = 0;
    pthread_create(&ctid[i], &attr, consumer, (void *)&threadCounts[producerThreads + i]);
  }

  sleep(simulationTime);
  stopProducer = true;
  stopConsumer = true;

  for (int i = 0; i < producerThreads; i++)
  {
    pthread_join(ptid[i], NULL);
  }
  for (int i = 0; i < consumerThreads; i++)
  {
    pthread_join(ctid[i], NULL);
  }

  printf("\nPRODUCER / CONSUMER SIMULATION COMPLETE\n");
  printf("=======================================\n");
  printf("Simulation Time:\t\t%d\n", simulationTime);
  printf("Maximum Thread Sleep Time:\t%d\n", maxThreadSleepTime);
  printf("Number of Producer Threads:\t%d\n", producerThreads);
  printf("Number of Consumer Threads:\t%d\n", consumerThreads);
  printf("Size of Buffer:\t\t\t%d\n", bufferSize);

  printf("Total Number of Items Produced:\t%d\n", itemsProduced);
  for (int i = 0; i < producerThreads; i++)
    printf("Thread %d :\t\t\t%d\n", i, threadCounts[i]);

  printf("Total Number of Items Consumed:\t%d\n", itemsConsumed);
  for (int i = producerThreads; i < (consumerThreads + producerThreads); i++)
    printf("Thread %d :\t\t\t%d\n", i, threadCounts[i]);

  printf("Number Of Items Remaining in Buffer :\t%d\n", buffers_occupied(&myBuffer));
  printf("Number Of Times Buffer Was Full :\t%d\n", bufferFull);
  printf("Number Of Times Buffer Was Empty :\t%d\n", bufferEmpty);

  return 0;
}

bool isPrime(int n)
{
  if (n <= 1)
    return false;
  for (int i = 2; i < n; i++)
    if (n % i == 0)
      return false;
  return true;
}

void *producer(void *param)
{
  pthread_t tid = pthread_self();

  /* generate a random number */
  while (!stopProducer)
  {
    int item = (rand() % 100) + 2;
    int *threadCounter = (int *)param;
    int threadCount = *threadCounter;
    /* sleep for a random period of time */
    sleep(rand() % maxThreadSleepTime + 1);
    if (!is_buffer_full(&myBuffer))
    {
      if (buffer_get_item(&myBuffer, wLoc) == -1)
      {

        sem_wait(&sem_mutex);
        buffer_print_status(&myBuffer, wLoc, rLoc);
        buffer_insert_item(&myBuffer, item, wLoc);
        wLoc = (wLoc + 1) % bufferSize;
        printf("Producer %lu writes %d\n", (unsigned long)tid, item);
        itemsProduced += 1;
        *threadCounter += 1;
        sem_post(&sem_mutex);
      }
    }
    else
    {
      printf("Buffer Full...\n");
      bufferFull++;
    }
  }
  pthread_exit(0);
}

void *consumer(void *param)
{
  pthread_t tid = pthread_self();
  int *threadCounter = (int *)param;
  int threadCount = *threadCounter;

  while (!stopConsumer)
  {
    /* sleep for a random period of time */
    sleep(rand() % maxThreadSleepTime + 1);

    int item = buffer_get_item(&myBuffer, rLoc);
    if (item != -1)
    {
      sem_wait(&sem_mutex);
      buffer_print_status(&myBuffer, wLoc, rLoc);
      printf("Consumer %lu reads %d", (unsigned long)tid, item);
      if (isPrime(item))
        printf("\t* * * PRIME * * *\n");
      else
        printf("\n");
      buffer_remove_item(&myBuffer, rLoc);
      itemsConsumed += 1;
      *threadCounter += 1;
      rLoc = (rLoc + 1) % bufferSize;
      sem_post(&sem_mutex);
    }
    else
    {
      printf("All buffers empty.  Consumer %lu waits\n", (unsigned long)tid);
      bufferEmpty++;
    }
  }
  pthread_exit(0);
}
