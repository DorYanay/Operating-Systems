#include "sidefunctions.h"

#define BUFFER_SIZE 1024


/*
generate_file(char *filename, long size_in_bytes)
This function is creating a file with random content.
It opens a file for writing and handles any error during opening.
It writes chunks of data to the file until it reaches the specified size.
*/
void generate_file(char *filename, long size_in_bytes)
{
    FILE *myfile = fopen(filename, "w");
    if (myfile == NULL)
    {
        perror("unexpected error occurred: fopen() failed\n");
        return;
    }

    const int chunk_size = 1024 * 1024; // 1MB
    char buffer[chunk_size];
    int bytes_written = 0;

    while (bytes_written < size_in_bytes)
    {
        int bytes_to_write = chunk_size;
        if (bytes_written + bytes_to_write > size_in_bytes)
        {
            bytes_to_write = size_in_bytes - bytes_written;
        }
        fwrite(buffer, bytes_to_write, 1, myfile);
        bytes_written += bytes_to_write;
    }

    fclose(myfile);
}

/*
generate_checksum(char *filename)
This function computes a checksum of a file.
It reads each byte of the file, adding its value to a running total (checksum), which is returned at the end.
This function also handles file opening errors.
*/

uint32_t generate_checksum(char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        perror("fopen()");
        return -1;
    }

    const int chunk_size = 1024 * 1024; // 1MB
    char buffer[chunk_size];
    uint32_t checksum = 0;

    while (!feof(fp))
    {
        int BytesRead = fread(buffer, 1, chunk_size, fp);
        for (int i = 0; i < BytesRead; i++)
        {
            checksum += (uint32_t)buffer[i];
        }
    }

    fclose(fp);

    return checksum;
}

/*
delete_file(char *filename)
This function deletes a specified file and handles any deletion errors.
If the deletion is successful, it returns 0; otherwise, it returns 1
*/
int delete_file(char *filename)
{
    int status = remove(filename);

    if (status == 0)
    {
        return 0;
    }
    else
    {
        perror("file deletion");
        return 1;
    }
}

/*
getFileSize(char *filename)
This function gets the size of a file by opening it in binary mode, seeking to the end, and then returning the current position.
*/
int getFileSize(char *filename)
{
    FILE *file = fopen(filename, "rb"); // Open the file in binary mode

    if (file == NULL)
    {
        // Error handling - file could not be opened
        return -1;
    }

    // Seek to the end of the file
    fseek(file, 0, SEEK_END);

    // Get the current position, which is the size of the file
    int size = ftell(file);

    // Close the file
    fclose(file);

    return size;
}

/*
receive_data(char *port, char *type, char *comm, int datasize, int quiet)
This function receives data over network sockets.
It initializes the socket, binds it, and then listens for incoming data.
Depending on the protocol (TCP/UDP), it receives the data and writes it to a file.
*/

int receive_data(char *port, char *type, char *comm, int datasize, int quiet)
{
    int Type = 0;
    int Comm = 0;
    int protocol = 0;
    if (!strcmp(type, "ipv4"))
    {
        Type = AF_INET;
        if (!strcmp(comm, "tcp"))
        {
            Comm = SOCK_STREAM;
            protocol = IPPROTO_TCP;
        }
        else if (!strcmp(comm, "udp"))
        {
            Comm = SOCK_DGRAM;
        }
    }
    else if (!strcmp(type, "ipv6"))
    {
        Type = AF_INET6;
        if (!strcmp(comm, "tcp"))
        {
            Comm = SOCK_STREAM;
            protocol = IPPROTO_TCP;
        }
        else if (!strcmp(comm, "udp"))
        {
            Comm = SOCK_DGRAM;
        }
    }
    else if (!strcmp(type, "uds"))
    {
        Type = AF_UNIX;
        if (!strcmp(comm, "stream"))
        {
            Comm = SOCK_STREAM;
        }
        else if (!strcmp(comm, "dgram"))
        {
            Comm = SOCK_DGRAM;
        }
    }
    struct sockaddr_storage serveraddr, clientaddr;
    socklen_t addr_size;
    int socketfd = socket(Type, Comm, protocol);
    if (socketfd < 0)
    {
        perror("unexpected error occurred: socket() failed\n");
        exit(EXIT_FAILURE);
    }

    if (Type == AF_INET)
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)&serveraddr;
        memset((char *)addr4, 0, sizeof(*addr4));
        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(atoi(port));
        addr4->sin_addr.s_addr = INADDR_ANY;

        addr_size = sizeof(*addr4);
        serveraddr = *(struct sockaddr_storage *)addr4;
    }
    else if (Type == AF_INET6)
    {
        int val = 1;
        setsockopt(socketfd, IPPROTO_IPV6, IPV6_V6ONLY, &val, sizeof(val));

        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)&serveraddr;
        memset((char *)addr6, 0, sizeof(*addr6));
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons(atoi(port));
        addr6->sin6_addr = in6addr_any;

        addr_size = sizeof(*addr6);
        serveraddr = *(struct sockaddr_storage *)addr6;
    }

    else if (Type == AF_UNIX)
    {
        struct sockaddr_un *unix_addr = (struct sockaddr_un *)&serveraddr;
        memset((char *)unix_addr, 0, sizeof(*unix_addr));
        unix_addr->sun_family = AF_UNIX;
        strncpy(unix_addr->sun_path, port, sizeof(unix_addr->sun_path) - 1);

        addr_size = sizeof(*unix_addr);
        serveraddr = *(struct sockaddr_storage *)unix_addr;
    }

    else
    {
        perror("unexpected error occurred: unknown type\n");
        exit(EXIT_FAILURE);
    }

    if (bind(socketfd, (struct sockaddr *)&serveraddr, addr_size) < 0)
    {
        perror("unexpected error occurred:bind()\n");
        exit(EXIT_FAILURE);
    }

    struct pollfd fds[2];
    fds[0] = (struct pollfd){.fd = socketfd, .events = POLLIN};
    int newsocketfd;

    if (Comm == SOCK_STREAM)
    {
        listen(socketfd, 1);
        if (!quiet)
        {
            printf("Server is listening...\n");
        }

        newsocketfd = accept(socketfd, (struct sockaddr *)&serveraddr, &addr_size);
        if (newsocketfd < 0)
        {
            perror("unexpected error occurred:accept()\n");
            exit(EXIT_FAILURE);
        }

        if (!quiet)
            printf("connection granted. \n");

        fds[1] = (struct pollfd){.fd = newsocketfd, .events = POLLIN};
    }

    char buffer[BUFFER_SIZE] = {0};
    FILE *fp = fopen("Received.txt", "wb");
    int size = 0;

    while (size < datasize)
    {
        int mypoll = poll(fds, 2, 3000);
        if (mypoll == 0)
        {
            if (!quiet)
                perror("TimeOut error occurred:mypoll\n");
            break;
        }
        if (mypoll < 0)
        {
            perror("unexpected error occurred:accept()\n");
            exit(EXIT_FAILURE);
        }

        int received = -1;

        if (Comm == SOCK_STREAM)
            received = recv(newsocketfd, buffer, BUFFER_SIZE, 0);
        else if (Comm == SOCK_DGRAM)
            received = recvfrom(socketfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientaddr, &addr_size);

        if (received < 0)
        {
            perror("unexpected error occurred in received: recvfrom()\n");
            exit(EXIT_FAILURE);
        }

        if (received == 0)
            break;
        size += received;
        fwrite(buffer, received, 1, fp);
        bzero(buffer, BUFFER_SIZE);
    }

    fclose(fp);

    if (Comm == SOCK_STREAM)
        close(newsocketfd);

    close(socketfd);

    if (Type == AF_UNIX)
        unlink(port);

    return size;
}

/*
send_data_mmap(char *fnFrom, char *inputFilename)
This function sends data by copying a file to shared memory.
The data in the file is accessed via memory-mapped files and then copied to shared memory.
*/

void send_data_mmap(char *fnFrom, char *inputFilename)
{
    printf("Copying file to shared memory\n");
    int fd_received = open(fnFrom, O_RDONLY);
    if (fd_received < 0)
    {
        perror("open\n");
        exit(EXIT_FAILURE);
    }
    int datasize = getFileSize(fnFrom);

    int shm_fd = shm_open(inputFilename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (shm_fd < 0)
    {
        perror("SHAREDMEMORY\n");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(shm_fd, datasize) < 0)
    {
        perror("TRUNCATE\n");
        exit(EXIT_FAILURE);
    }

    char *shm_p = mmap(NULL, datasize, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_p == MAP_FAILED)
    {
        perror("MMAPING\n");
        exit(EXIT_FAILURE);
    }
    char *fp = mmap(NULL, datasize, PROT_READ, MAP_SHARED, fd_received, 0);
    if (fp == MAP_FAILED)
    {
        perror("MMAPING\n");
        exit(EXIT_FAILURE);
    }

    memcpy(shm_p, fp, datasize);

    if (munmap(shm_p, datasize) < 0 || munmap(fp, datasize) < 0)
    {
        perror("unmapping shared memory\n");
        exit(EXIT_FAILURE);
    }
    close(fd_received);
    close(shm_fd);
}

/*
receive_data_mmap(char *filenameTo, char *inputFilename, int datasize, int quiet)
This function receives data from shared memory and writes it to a file.
The data in shared memory is accessed and then copied to a newly created or truncated file.
*/

void receive_data_mmap(char *filenameTo, char *inputFilename, int datasize, int quiet)
{

    int fd = open(filenameTo, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        perror("opening file to\n");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, datasize) < 0)
    {
        perror("truncating file\n");
        exit(EXIT_FAILURE);
    }
    int shm_fd = shm_open(inputFilename, O_RDWR, S_IRUSR | S_IWUSR);
    if (shm_fd < 0)
    {
        perror("opening shared memory\n");
        exit(EXIT_FAILURE);
    }

    char *shm_p = mmap(NULL, datasize, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_p == MAP_FAILED)
    {
        perror("mapping shared memory\n");
        exit(EXIT_FAILURE);
    }
    char *fp = mmap(NULL, datasize, PROT_WRITE, MAP_SHARED, fd, 0);
    if (fp == MAP_FAILED)
    {
        perror("mapping file\n");
        exit(EXIT_FAILURE);
    }
    memcpy(fp, shm_p, datasize);

    if (munmap(shm_p, datasize) < 0 || munmap(fp, datasize) < 0)
    {
        perror("unmapping shared memory\n");
        exit(EXIT_FAILURE);
    }

    // Unlink shared memory to free resources
    if (shm_unlink(inputFilename) < 0)
    {
        perror("unlinking shared memory\n");
        exit(EXIT_FAILURE);
    }

    close(fd);
    close(shm_fd);
}

/*
send_file_through_pipe(char *fnFrom, char *pipename)
This function sends a file through a named pipe (FIFO).
It reads from a file and writes the content to a pipe.
*/

void send_file_through_pipe(char *fnFrom, char *pipename)
{
    printf("Sending data\n");
    char buffer[BUFFER_SIZE] = {0};
    int fd_received = open(fnFrom, O_RDONLY);
    if (fd_received < 0)
    {
        perror("open()\n");
        exit(EXIT_FAILURE);
    }

    // Create fifo
    if (mkfifo(pipename, 0666) < 0 && errno != EEXIST)
    {
        printf("ERROR creating fifo\n");
        exit(EXIT_FAILURE);
    }

    int fdpipe = open(pipename, O_WRONLY);
    if (fdpipe < 0)
    {
        printf("ERROR opening fifo\n");
        exit(EXIT_FAILURE);
    }

    ssize_t read_res, write_res;
    while ((read_res = read(fd_received, buffer, BUFFER_SIZE)) > 0)
    {
        write_res = write(fdpipe, buffer, read_res);
        if (write_res != read_res)
        {
            printf("ERROR writing to fifo\n");
            exit(EXIT_FAILURE);
        }
    }

    close(fd_received);
    close(fdpipe);
}

/*
send_data(char *ip, char *port, char *filename, char *type, char *comm)
This function is responsible for sending data over a network.
It opens a file, creates a socket, and initializes necessary network structures based on the communication protocol (TCP/UDP).
Data is then read from the file and sent over the network through the socket.
*/

void send_data(char *ip, char *port, char *filename, char *type, char *comm)
{
    int Type = 0;
    int Comm = 0;
    int protocol = 0;
    if (!strcmp(type, "ipv4"))
    {
        Type = AF_INET;
        if (!strcmp(comm, "tcp"))
        {
            Comm = SOCK_STREAM;
            protocol = IPPROTO_TCP;
        }
        else if (!strcmp(comm, "udp"))
        {
            Comm = SOCK_DGRAM;
        }
    }
    else if (!strcmp(type, "ipv6"))
    {
        Type = AF_INET6;
        if (!strcmp(comm, "tcp"))
        {
            Comm = SOCK_STREAM;
            protocol = IPPROTO_TCP;
        }
        else if (!strcmp(comm, "udp"))
        {
            Comm = SOCK_DGRAM;
        }
    }
    else if (!strcmp(type, "uds"))
    {
        Type = AF_UNIX;
        if (!strcmp(comm, "stream"))
        {
            Comm = SOCK_STREAM;
        }
        else if (!strcmp(comm, "dgram"))
        {
            Comm = SOCK_DGRAM;
        }
    }
    printf("Sending data\n");
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("ERROR opening file\n");
        exit(EXIT_FAILURE);
    }
    int datasize = getFileSize(filename);

    int sockfd = socket(Type, Comm, protocol);
    if (sockfd < 0)
    {
        printf("ERROR opening socket\n");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_storage addr;
    socklen_t addr_len;

    if (Type == AF_INET)
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)&addr;
        memset((char *)addr4, 0, sizeof(*addr4));
        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(atoi(port));
        inet_pton(AF_INET, ip, &addr4->sin_addr);

        addr_len = sizeof(*addr4);
        addr = *(struct sockaddr_storage *)addr4;
    }

    else if (Type == AF_INET6)
    {
        int optval = 1;
        setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &optval, sizeof(optval));
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)&addr;
        memset((char *)addr6, 0, sizeof(*addr6));
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons(atoi(port));
        inet_pton(AF_INET6, ip, &addr6->sin6_addr);

        addr_len = sizeof(*addr6);
        addr = *(struct sockaddr_storage *)addr6;
    }

    else if (Type == AF_UNIX)
    {
        struct sockaddr_un *unix_addr = (struct sockaddr_un *)&addr;
        memset((char *)unix_addr, 0, sizeof(*unix_addr));
        unix_addr->sun_family = AF_UNIX;
        strcpy(unix_addr->sun_path, port);

        addr_len = sizeof(*unix_addr);
        addr = *(struct sockaddr_storage *)unix_addr;
    }

    if (Comm == SOCK_STREAM || Type == AF_UNIX)
    {
        if (connect(sockfd, (struct sockaddr *)&addr, addr_len) < 0)
        {
            perror("connect()\n");
            exit(EXIT_FAILURE);
        }
    }

    char buffer[BUFFER_SIZE] = {0};
    int totalbytes = 0;
    int BytesRead = 0;
    while (totalbytes < datasize)
    {
        BytesRead = BytesRead = (BUFFER_SIZE < (datasize - totalbytes)) ? BUFFER_SIZE : (datasize - totalbytes);
        fread(buffer, 1, BytesRead, fp);
        int sent;
        if (Comm == SOCK_STREAM || Type == AF_UNIX)
        {
            sent = send(sockfd, buffer, BytesRead, 0);
        }
        else if (Comm == SOCK_DGRAM)
        {
            sleep(0.01);
            sent = sendto(sockfd, buffer, BytesRead, 0, (struct sockaddr *)&addr, sizeof(addr));
        }
        if (sent < 0)
        {
            perror("sendto()\n");
            exit(EXIT_FAILURE);
        }
        totalbytes += sent;
        bzero(buffer, BUFFER_SIZE);
    }
    fclose(fp);
    close(sockfd);
}


void receive_file_through_pipe(char *filenameTo, char *pipename, int quiet)
{
    char buffer[BUFFER_SIZE] = {0};
    int fd = open(filenameTo, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        perror("open()\n");
        exit(EXIT_FAILURE);
    }

    int fdpipe = open(pipename, O_RDONLY);
    if (fdpipe < 0)
    {
        perror("open()\n");
        exit(EXIT_FAILURE);
    }

    ssize_t read_res, write_res;
    while ((read_res = read(fdpipe, buffer, BUFFER_SIZE)) > 0)
    {
        write_res = write(fd, buffer, read_res);
        if (write_res != read_res)
        {
            perror("write()\n");
            exit(EXIT_FAILURE);
        }
    }

    // Unlink fifo to free resources
    if (unlink(pipename) < 0)
    {
        perror("unlink()\n");
        exit(EXIT_FAILURE);
    }

    close(fd);
    close(fdpipe);
}
