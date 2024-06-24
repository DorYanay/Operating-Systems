#include "stnc.h"

#define BUFFER_SIZE 1024

int performance = 0, ipv4 = 0, ipv6 = 0, tcp = 0, udp = 0, uds = 0, dgram = 0, stream = 0,
    client_chat = 0, server_chat = 0, mmap_file = 0, pipe_file = 0, quiet = 0;

char *filename = NULL, *ip = NULL, *port = NULL, *type = NULL, *param = NULL;

void startClient(char *ip, char *port)
{
    char input[BUFFER_SIZE];
    int timeout = -1;

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(port));
    int tmp = inet_pton(AF_INET, ip, &server_address.sin_addr);
    if (tmp <= 0)
    {
        perror("unexpected error occurred in tmp: inet_pton() failed\n");
        exit(EXIT_FAILURE);
    }

    int client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_fd < 0)
    {
        perror("unexpected error occurred in client_fd: socket() failed\n");
        exit(EXIT_FAILURE);
    }
    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("unexpected error occurred: connect() failed\n");
        exit(EXIT_FAILURE);
    }

    if (!quiet)
        printf("Client connected to %s:%s\n", ip, port);

    struct pollfd fds[2] = {{.fd = STDIN_FILENO, .events = POLLIN}, {.fd = client_fd, .events = POLLIN}};

    while (1)
    {
        if (performance)
        {
            char *codeFile = "send.txt";
            // generate file to transfer
            generate_file(codeFile, 100 * 1024 * 1024);

            // make new port number for transfer file;
            char appended_port[10];
            sprintf(appended_port, "%d", atoi(port) + 1);

            int Size = getFileSize(codeFile);
            char size_string[20];
            sprintf(size_string, "%d", Size);

            // send size of the file to the server
            int bytesSent = send(client_fd, size_string, strlen(size_string), 0);
            if (bytesSent < 0)
            {
                perror("unexpected error occurred in bytesSent: send() failed\n");
                exit(EXIT_FAILURE);
            }

            sleep(1);

            uint32_t checksum = generate_checksum(codeFile);
            char check_string[20];
            sprintf(check_string, "%d", checksum);

            // send checksum of the file to the server
            bytesSent = send(client_fd, check_string, strlen(check_string), 0);
            if (bytesSent < 0)
            {
                perror("unexpected error occurred in bytesSent: send() failed\n");
                exit(EXIT_FAILURE);
            }

            sleep(1);

            struct timeval start;
            gettimeofday(&start, NULL);
            char start_time_str[20];
            sprintf(start_time_str, "%ld.%06ld", start.tv_sec, start.tv_usec);

            // send start time to the server
            bytesSent = send(client_fd, start_time_str, strlen(start_time_str), 0);
            if (bytesSent < 0)
            {
                perror("unexpected error occurred in bytesSent: send() failed\n");
                exit(EXIT_FAILURE);
            }

            sleep(1);

            if (tcp && ipv4)
                bytesSent = send(client_fd, "ipv4 tcp", 8, 0);
            else if (udp && ipv4)
                bytesSent = send(client_fd, "ipv4 udp", 8, 0);
            else if (tcp && ipv6)
                bytesSent = send(client_fd, "ipv6 tcp", 8, 0);
            else if (udp && ipv6)
                bytesSent = send(client_fd, "ipv6 udp", 8, 0);
            else if (uds && dgram)
                bytesSent = send(client_fd, "uds dgram", 9, 0);
            else if (uds && stream)
                bytesSent = send(client_fd, "uds stream", 10, 0);
            else if (mmap_file)
                bytesSent = send(client_fd, "mmap", 5, 0);
            else if (pipe_file)
                bytesSent = send(client_fd, "pipe", 5, 0);

            if (bytesSent < 0)
            {
                perror("unexpected error occurred in bytesSent: send() failed\n");
                exit(EXIT_FAILURE);
            }

            sleep(1);

            if (tcp && ipv4)
                send_data(ip, appended_port, codeFile, "ipv4", "tcp");
            else if (udp && ipv4)
                send_data(ip, appended_port, codeFile, "ipv4", "udp");
            else if (tcp && ipv6)
                send_data(ip, appended_port, codeFile, "ipv6", "tcp");
            else if (udp && ipv6)
                send_data(ip, appended_port, codeFile, "ipv6", "udp");
            else if (uds && dgram)
            {
                sleep(0.5);
                send_data(0, appended_port, codeFile, "uds", "dgram");
            }
            else if (uds && stream)
            {
                sleep(0.5);
                send_data(0, appended_port, codeFile, "uds", "stream");
            }
            else if (mmap_file)
            {
                send_data_mmap(codeFile, filename);
                bytesSent = send(client_fd, filename, strlen(filename), 0);
                if (bytesSent < 0)
                {
                    perror("unexpected error occurred in bytesSent: send() failed\n");
                    exit(EXIT_FAILURE);
                }
            }
            else if (pipe_file)
            {
                bytesSent = send(client_fd, filename, strlen(filename), 0);
                if (bytesSent < 0)
                {
                    perror("unexpected error occurred in bytesSent: send() failed\n");
                    exit(EXIT_FAILURE);
                }
                send_file_through_pipe(codeFile, filename);
            }
            delete_file(codeFile);
            exit(EXIT_FAILURE);
        }

        // Start chat TCP ipv4
        int myPoll = poll(fds, 2, timeout);
        if (myPoll < 0)
        {
            perror("unexpected error occurred in myPoll: poll() failed\n");
            exit(EXIT_FAILURE);
        }

        if (fds[0].revents & POLLIN)
        {
            int bytesRead = read(STDIN_FILENO, input, BUFFER_SIZE);
            if (bytesRead < 0)
            {
                perror("unexpected error occurred in bytesRead: read() failed\n");
                exit(EXIT_FAILURE);
            }

            input[bytesRead] = '\0';

            int bytesSent = send(client_fd, input, bytesRead, 0);
            if (bytesSent < 0)
            {
                perror("unexpected error occurred in bytesSent: send() failed\n");
                exit(EXIT_FAILURE);
            }
            bzero(input, BUFFER_SIZE);
        }

        if (fds[1].revents & POLLIN)
        {
            int bytesRecv = recv(client_fd, input, BUFFER_SIZE - 1, 0);
            if (bytesRecv < 0)
            {
                perror("unexpected error occurred in bytesSent: recv() failed\n");
                exit(EXIT_FAILURE);
            }
            if (bytesRecv == 0)
            {
                perror("Server ShutDown\n");
                exit(EXIT_FAILURE);
            }

            input[bytesRecv] = '\0';
            printf("Received: %s", input);
            bzero(input, BUFFER_SIZE);
        }
    }
    close(client_fd);
}

void startServer(char *port)
{
    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(atoi(port))};

    if (!quiet)
        printf("server is listening on port : %s\n", port);

    int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd < 0)
    {
        perror("unexpected error occurred in server_fd: socket() failed\n");
        exit(EXIT_FAILURE);
    }

    if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("unexpected error occurred: bind() failed\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_length = sizeof(client_addr);

        if (listen(server_fd, 1) < 0)
        {
            perror("unexpected error occurred: listen() failed\n");
            exit(EXIT_FAILURE);
        }

        int client_sock = accept(server_fd, (struct sockaddr *)&client_addr, &client_length);
        if (client_sock < 0)
        {
            perror("unexpected error occurred: accept() failed\n");
            exit(EXIT_FAILURE);
        }

        if (!quiet)
            printf("Client connected\n");

        struct pollfd fds[2] = {{.fd = STDIN_FILENO, .events = POLLIN}, {.fd = client_sock, .events = POLLIN}};
        char input[BUFFER_SIZE];

        while (1)
        {
            int mypoll = poll(fds, 2, -1);
            if (mypoll < 0)
            {
                perror("unexpected error occurred in mypoll: poll() failed\n");
                exit(EXIT_FAILURE);
            }

            if (fds[0].revents & POLLIN)
            {
                int bytesRead = read(STDIN_FILENO, input, BUFFER_SIZE);
                if (bytesRead < 0)
                {
                    perror("unexpected error occurred in bytesRead: read() failed\n");
                    exit(EXIT_FAILURE);
                }

                input[bytesRead] = '\0';

                if (send(client_sock, input, bytesRead, 0) < 0)
                {
                    perror("unexpected error occurred: send() failed\n");
                    exit(EXIT_FAILURE);
                }
                bzero(input, BUFFER_SIZE);
            }

            if (fds[1].revents & POLLIN)
            {
                int bytesRecv = recv(client_sock, input, BUFFER_SIZE - 1, 0);
                if (bytesRecv < 0)
                {
                    perror("unexpected error occurred: send() failed\n");
                    exit(EXIT_FAILURE);
                }
                if (bytesRecv == 0)
                {
                    if (!quiet)
                        printf("Client disconnected\n");
                    break;
                }

                input[bytesRecv] = '\0';

                if (!quiet)
                {
                    printf("Received: %s", input);
                }

                if (performance)
                {
                    int Size = 0;
                    int received = 0;
                    uint32_t checksum = 0;
                    struct timeval start, end;
                    Size = atoi(input);
                    bzero(input, BUFFER_SIZE);

                    bytesRecv = recv(client_sock, input, 20, 0);
                    if (bytesRecv <= 0)
                    {
                        perror("unexpected error occurred in bytesRecv: recv() failed\n");
                        exit(EXIT_FAILURE);
                    }

                    input[bytesRecv] = '\0';

                    sscanf(input, "%u", &checksum);
                    bzero(input, BUFFER_SIZE);

                    bytesRecv = recv(client_sock, input, 20, 0);
                    if (bytesRecv <= 0)
                    {
                        perror("unexpected error occurred in bytesRecv: recv() failed\n");
                        exit(EXIT_FAILURE);
                    }
                    char *appended_port[10];
                    sprintf(*appended_port, "%d", atoi(port) + 1);
                    input[bytesRecv] = '\0';

                    sscanf(input, "%ld.%06ld", &start.tv_sec, &start.tv_usec);
                    bzero(input, BUFFER_SIZE);

                    bytesRecv = recv(client_sock, input, 20, 0);
                    if (bytesRecv <= 0)
                    {
                        perror("unexpected error occurred in bytesRecv: recv() failed\n");
                        exit(EXIT_FAILURE);
                    }

                    input[bytesRecv] = '\0';

                    if (!quiet)
                        printf("performance: %s\n", input);
                    char *TYPEPARAM = NULL;
                    if (!strcmp(input, "ipv4 tcp"))
                    {
                        TYPEPARAM = "ipv4_tcp";
                        received = receive_data(*appended_port, "ipv4", "tcp", Size, quiet);
                    }
                    else if (!strcmp(input, "ipv4 udp"))
                    {
                        TYPEPARAM = "ipv4_udp";
                        received = receive_data(*appended_port, "ipv4", "udp", Size, quiet);
                    }
                    else if (!strcmp(input, "ipv6 tcp"))
                    {
                        TYPEPARAM = "ipv6_tcp";
                        received = receive_data(*appended_port, "ipv6", "tcp", Size, quiet);
                    }
                    else if (!strcmp(input, "ipv6 udp"))
                    {
                        TYPEPARAM = "ipv6_udp";
                        received = receive_data(*appended_port, "ipv6", "udp", Size, quiet);
                    }
                    else if (!strcmp(input, "uds dgram"))
                    {
                        TYPEPARAM = "uds_dgram";
                        received = receive_data(*appended_port, "uds", "dgram", Size, quiet);
                    }
                    else if (!strcmp(input, "uds stream"))
                    {
                        TYPEPARAM = "uds_stream";
                        received = receive_data(*appended_port, "uds", "stream", Size, quiet);
                    }
                    else if (!strcmp(input, "mmap"))
                    {
                        TYPEPARAM = "mmap";
                        bytesRecv = recv(client_sock, input, BUFFER_SIZE - 1, 0);
                        if (bytesRecv < 0)
                        {
                            perror("unexpected error occurred in bytesRecv: recv() failed\n");
                            exit(EXIT_FAILURE);
                        }
                        sleep(1);
                        receive_data_mmap("Received.txt", input, Size, quiet);
                        received = getFileSize("Received.txt");
                    }
                    else if (!strcmp(input, "pipe"))
                    {
                        TYPEPARAM = "pipe";
                        bytesRecv = recv(client_sock, input, BUFFER_SIZE - 1, 0);
                        if (bytesRecv < 0)
                        {
                            perror("unexpected error occurred in bytesRecv: recv() failed\n");
                            exit(EXIT_FAILURE);
                        }

                        sleep(1);
                        receive_file_through_pipe("Received.txt", input, quiet);
                        received = getFileSize("Received.txt");
                    }

                    gettimeofday(&end, NULL);
                    u_int32_t received_data_checksum = generate_checksum("Received.txt");
                    if ((received_data_checksum == checksum) && (!quiet))
                    {
                        printf("validation succeeded\n");
                    }
                    else
                    {
                        if (!quiet)
                        {
                            perror("validation failed.\n");
                            if (received != Size)
                            {
                                perror("lost packets.\n");
                            }
                        }
                    }
                    long elapsed_time = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;

                    printf("%s,%ld\n", TYPEPARAM, elapsed_time);

                    delete_file("Received.txt");
                }
                bzero(input, BUFFER_SIZE);
            }
        }
        close(client_sock);
    }
    close(server_fd);
}

void usage()
{
    printf("Usage:\n");
    printf("Part A: chat tool");
    printf("Client side: stnc -c IP PORT\n");
    printf("Server side: stnc -s port\n");
    printf("Part B: performance test");
    printf("Client side: stnc -c IP PORT -p <type> <param>\n");
    printf("Server side: stnc -s port -p (p for performance test) -q (q for quiet)\n");
    printf("\n");
    printf("Available connection types:\n");
    printf("ipv4 tcp\n");
    printf("ipv4 udp\n");
    printf("ipv6 tcp\n");
    printf("ipv6 udp\n");
    printf("uds dgram\n");
    printf("uds stream\n");
    printf("mmap filename\n");
    printf("pipe filename\n");
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    // Check if the cmd length correct
    if (argc < 3)
    {
        usage();
        exit(EXIT_FAILURE);
    }

    // parse the cmd to variables
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-c") == 0)
        {
            client_chat = 1;
            ip = argv[i + 1];
            port = argv[i + 2];
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            server_chat = 1;
            port = argv[i + 1];
        }
        else if (strcmp(argv[i], "-p") == 0)
        {
            performance = 1;
            type = argv[i + 1];
            param = argv[i + 2];
        }
        else if (strcmp(argv[i], "-q") == 0)
        {
            quiet = 1;
        }
    }
    // Check if the cmd correct
    if (performance && client_chat)
    {
        if (!type || !param)
        {
            usage();
            exit(EXIT_FAILURE);
        }
        if (strcmp(type, "ipv4") == 0)
        {
            ipv4 = 1;
        }
        else if (strcmp(type, "ipv6") == 0)
        {
            ipv6 = 1;
        }
        else if (strcmp(type, "uds") == 0)
        {
            uds = 1;
        }
        else if (strcmp(type, "mmap") == 0)
        {
            mmap_file = 1;
        }
        else if (strcmp(type, "pipe") == 0)
        {
            pipe_file = 1;
        }

        if (strcmp(param, "tcp") == 0)
        {
            tcp = 1;
        }
        else if (strcmp(param, "udp") == 0)
        {
            udp = 1;
        }
        else if (strcmp(param, "dgram") == 0)
        {
            dgram = 1;
        }
        else if (strcmp(param, "stream") == 0)
        {
            stream = 1;
        }
        else
        {
            filename = param;
        }
    }
    if (client_chat)
    {
        startClient(ip, port);
    }
    else if (server_chat)
    {
        startServer(port);
    }
    usage();
    exit(EXIT_FAILURE);
}