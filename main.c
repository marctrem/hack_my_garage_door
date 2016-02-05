#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define MAXPENDING 1
#define PASSWORD_LEN 8


void handle_conn(int soc_fd);

int main() {

    unsigned short port = 1337;

    int soc_fd, conn_fd;
    int pid;

    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(struct sockaddr_in));


    if ((soc_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {

    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);


    if (bind(soc_fd, (struct sockaddr *) &my_addr, sizeof(my_addr))) {
        perror("Error while binding.");
    }


    if (listen(soc_fd, MAXPENDING) < 0) {
        perror("server: listen");
    }


    while (1) {

        if ((conn_fd = accept(soc_fd, NULL, NULL)) < 0) {
            perror("Dank");
        }

        if ((pid = fork()) < 0) {
            perror("error forking");
        }

        if (pid == 0) {
            handle_conn(conn_fd);
            exit(0);
        }
        else {
            close(conn_fd);
        }
    }
    return 0;
}


void handle_conn(int soc_fd) {

    char answer_buffer[PASSWORD_LEN + 1];
    char sbuffer[PASSWORD_LEN + 1];
    char newchar;
    sbuffer[PASSWORD_LEN] = 0;

    uint32_t answer = 0;

    FILE *urandom = fopen("/dev/urandom", "r");
    fread(&answer, sizeof(uint32_t), 1, urandom);
    fclose(urandom);

    answer %= 100000000;

    sprintf(answer_buffer, "%08d", answer);
    printf("Answer: %s\n", answer_buffer);

    dprintf(soc_fd, "Please enter your garage door %d-digit code", PASSWORD_LEN);
    printf("%d\n", soc_fd);


    unsigned int total_bytes_read = 0;
    while (total_bytes_read < 100000001) {
        ssize_t nb_read = read(soc_fd, &newchar, sizeof(char));

        if (nb_read <= 0) {
            // Todo: Handle read cases
            if (errno == EINTR) {
                continue;
            }
            else {
                exit(0);
            }
        }

        total_bytes_read += nb_read;
        memmove(sbuffer, sbuffer + 1, sizeof(sbuffer) - 1);
        sbuffer[sizeof(sbuffer) - 1] = newchar;
        if(!strncmp(answer_buffer, sbuffer, PASSWORD_LEN)) {
            dprintf(soc_fd, "\nCongrats you found the password\n Your precious flag is #HOMEINVASION");
            exit(0);
        }
    }

    // Too many tries
    dprintf(soc_fd, "\nTo many tries! Goodbye.");
}


