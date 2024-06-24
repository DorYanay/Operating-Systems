#pragma once

#include <netinet/tcp.h>
#include <poll.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/time.h>

int delete_file(char *filename);
void send_data(char *ip, char *port, char *filename, char *type, char *comm);
int receive_data(char *port, char *type, char *comm, int datasize, int quiet);
int getFileSize(char *filename);
void send_data_mmap(char *filenameFrom, char *sharedFilename);
void receive_data_mmap(char *filenameTo, char *sharedFilename, int fileSize, int quiet);
void receive_file_through_pipe(char *filenameTo, char *fifoName, int quiet);
void send_file_through_pipe(char *filenameFrom, char *fifoName);
void generate_file(char *filename, long size_in_bytes);
uint32_t generate_checksum(char *filename);
