# st_pipeline

st_pipeline is a multi-threaded software that performs a pipeline of tasks using Active Objects (AO). It receives an integer argument N, representing the number of tasks, and an optional argument random seed. If no random seed is provided, it generates one using the current time.

## Stage A: is_prime Function

The `is_prime` function determines whether an unsigned integer number is prime. It uses a simple algorithm that checks the divisibility of the number by odd numbers up to the square root. The function returns 0 if the number is not prime, or a non-zero value otherwise.

## Stage B: Queue Implementation

The software implements a multi-threaded environment with a queue. The queue is protected by a mutex and allows waiting for an item without busy waiting, using a condition variable.

The queue holds `void*` elements, which can be used to store any type of data.

## Stage C: ActiveObject Implementation

The `ActiveObject` class represents an active object that supports the following functions:

- `CreateActiveObject`: Creates and runs a thread for the active object. It enqueues the object and receives a pointer to a function to be called for each item in the queue. The active object's busy loop dequeues an item from the queue and calls the provided function for each item.
- `getQueue`: Returns a pointer to the active object's queue. This function can be used to enqueue an item.
- `stop`: Stops the active object and frees its memory.

## Stage D: Task Execution

The pipeline is built using a collection of Active Object (AO) objects. The software follows the following steps:

1. The first AO initializes the random number generator with the given seed (or a generated seed if not provided) and generates N 6-digit pseudo-random numbers. It passes each number to the next AO with a delay of 1 millisecond between each pass.
2. The second AO receives the number, prints it, checks if it is prime, and prints "true" or "false" accordingly. It adds 11 to the number and passes it to the next AO.
3. The third AO receives the number, prints it, checks if it is prime, and prints "true" or "false" accordingly. It subtracts 13 from the number and passes it to the next AO.
4. The fourth AO receives the number, prints it, adds 2 to it, and prints the new number. The pipeline should end with the last number being equal to the first.

## Usage

To compile and run the st_pipeline software, follow these steps:

1. Compile the code using the makefile command: make all

2. Run the executable with the required arguments:


- N: The number of tasks to perform in the pipeline.
- Seed (optional): The random seed to use for the random number generator. If not provided, a seed will be generated using the current time.

## Example Usage

To execute the st_pipeline software with 10 tasks and a random seed of 12345, use the following command:
./st_pipeline 10 12345

This will run the pipeline with 10 tasks and use the provided random seed.

If no random seed is provided, the software will generate a seed using the current time:



This will run the pipeline with 10 tasks and use the provided random seed.

If no random seed is provided, the software will generate a seed using the current time:


This will run the pipeline with 10 tasks and a randomly generated seed.

Note: The program will output the generated numbers, their primality, and the modified numbers as described in Stage D.

## Author: </div>
[Dor Yanay](https://github.com/DorYanay "Dor Yanay") **&** [DorHarizi](https://github.com/DorHarizi "DorHarizi")</div>
