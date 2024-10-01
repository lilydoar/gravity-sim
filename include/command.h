#ifndef COMMAND_H
#define COMMAND_H

#include <stdlib.h>
#include <stdio.h>

// Status codes for command execution
typedef enum {
    COMMAND_SUCCESS,
    COMMAND_FAILURE
} CommandStatus;

// Struct representing a command
typedef struct {
    CommandStatus (*execute)(void*);  // Function pointer to execute the command
    void* data;                       // Pointer to the data the command operates on
} Command;

// CommandQueue structure
typedef struct {
    Command** commands;  // Array of Command pointers
    int size;            // Total size of the queue (capacity)
    int count;           // Current number of commands in the queue
    int head;            // Index of the front of the queue
    int tail;            // Index of the rear of the queue
} CommandQueue;

// Function prototypes

// Create a new command with the given execute function and data
Command* create_command(CommandStatus (*execute)(void*), void* data);

// Free a command
void free_command(Command* command);

// Create a new command queue with a fixed size
CommandQueue* create_command_queue(int size);

// Free a command queue
void free_command_queue(CommandQueue* queue);

// Add a command to the queue
CommandStatus enqueue_command(CommandQueue* queue, Command* command);

// Process the next command in the queue
CommandStatus dequeue_and_execute_command(CommandQueue* queue);

// Process all commands in the queue
CommandStatus process_all_commands(CommandQueue* queue);

#endif // COMMAND_H


#ifdef COMMAND_IMPLEMENTATION

// Create a new command
Command* create_command(CommandStatus (*execute)(void*), void* data) {
    Command* cmd = (Command*)malloc(sizeof(Command));
    if (!cmd) {
        return NULL;  // Memory allocation failure
    }
    cmd->execute = execute;
    cmd->data = data;
    return cmd;
}

// Free a command
void free_command(Command* command) {
    if (command) {
        free(command);  // Only free the command, not the data it operates on
    }
}

// Create a new command queue
CommandQueue* create_command_queue(int size) {
    CommandQueue* queue = (CommandQueue*)malloc(sizeof(CommandQueue));
    if (!queue) {
        return NULL;  // Memory allocation failure
    }

    queue->commands = (Command**)malloc(sizeof(Command*) * size);
    if (!queue->commands) {
        free(queue);
        return NULL;
    }

    queue->size = size;
    queue->count = 0;
    queue->head = 0;
    queue->tail = 0;

    return queue;
}

// Free a command queue
void free_command_queue(CommandQueue* queue) {
    if (queue) {
        free(queue->commands);  // Free the array of command pointers
        free(queue);            // Free the queue itself
    }
}

// Add a command to the queue
CommandStatus enqueue_command(CommandQueue* queue, Command* command) {
    if (queue->count >= queue->size) {
        return COMMAND_FAILURE;  // Queue is full
    }

    queue->commands[queue->tail] = command;
    queue->tail = (queue->tail + 1) % queue->size;
    queue->count++;

    return COMMAND_SUCCESS;
}

// Process and execute the next command in the queue
CommandStatus dequeue_and_execute_command(CommandQueue* queue) {
    if (queue->count == 0) {
        return COMMAND_FAILURE;  // Queue is empty
    }

    Command* cmd = queue->commands[queue->head];
    CommandStatus status = cmd->execute(cmd->data);  // Execute the command

    // Free the command after execution
    free_command(cmd);

    queue->head = (queue->head + 1) % queue->size;
    queue->count--;

    return status;
}

// Process and execute all commands in the queue
CommandStatus process_all_commands(CommandQueue* queue) {
    while (queue->count > 0) {
        CommandStatus status = dequeue_and_execute_command(queue);
        if (status == COMMAND_FAILURE) {
            return COMMAND_FAILURE;  // Stop processing if a command fails
        }
    }

    return COMMAND_SUCCESS;
}

#endif // COMMAND_IMPLEMENTATION
