#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "list.h"

#define PORT 9001
#define ACK "ACK: "

static list_t *mylist = NULL;
static int servSockD = -1;
static int clientSocket = -1;

void sigint_handler(int sig) {
    if (mylist != NULL) {
        list_free(mylist);
        mylist = NULL;
    }
    if (clientSocket != -1) {
        close(clientSocket);
    }
    if (servSockD != -1) {
        close(servSockD);
    }
    exit(0);
}

int main(int argc, char const *argv[]) {
    int n, val, idx;
    char buf[1024];
    char sbuf[1024];
    char *token;
    struct sockaddr_in servAddr;

    signal(SIGINT, sigint_handler);

    servSockD = socket(AF_INET, SOCK_STREAM, 0);
    if (servSockD < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    int opt = 1;
    setsockopt(servSockD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(servSockD, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("Bind failed");
        close(servSockD);
        exit(1);
    }

    if (listen(servSockD, 5) < 0) {
        perror("Listen failed");
        close(servSockD);
        exit(1);
    }

    mylist = list_alloc();

    int keep_running = 1;
    while (keep_running) {
        clientSocket = accept(servSockD, NULL, NULL);
        if (clientSocket < 0) {
            break;
        }

        while (1) {
            memset(buf, '\0', sizeof(buf));
            memset(sbuf, '\0', sizeof(sbuf));

            n = recv(clientSocket, buf, sizeof(buf) - 1, 0);
            if (n <= 0) {
                break;
            }
            buf[n] = '\0';

            token = strtok(buf, " \n");
            if (token == NULL) {
                continue;
            }

            if (strcmp(token, "exit") == 0) {
                keep_running = 0;
                break;
            } else if (strcmp(token, "get_length") == 0) {
                val = list_length(mylist);
                sprintf(sbuf, "Length = %d", val);
            } else if (strcmp(token, "add_front") == 0) {
                token = strtok(NULL, " \n");
                if (token != NULL) {
                    val = atoi(token);
                    list_add_to_front(mylist, val);
                    sprintf(sbuf, "%s%d", ACK, val);
                }
            } else if (strcmp(token, "add_back") == 0) {
                token = strtok(NULL, " \n");
                if (token != NULL) {
                    val = atoi(token);
                    list_add_to_back(mylist, val);
                    sprintf(sbuf, "%s%d", ACK, val);
                }
            } else if (strcmp(token, "add_position") == 0) {
                token = strtok(NULL, " \n");
                if (token != NULL) {
                    idx = atoi(token);
                    token = strtok(NULL, " \n");
                    if (token != NULL) {
                        val = atoi(token);
                        list_add_at_index(mylist, val, idx);
                        sprintf(sbuf, "%s%d", ACK, val);
                    }
                }
            } else if (strcmp(token, "remove_front") == 0) {
                val = list_remove_from_front(mylist);
                sprintf(sbuf, "%d", val);
            } else if (strcmp(token, "remove_back") == 0) {
                val = list_remove_from_back(mylist);
                sprintf(sbuf, "%d", val);
            } else if (strcmp(token, "remove_position") == 0) {
                token = strtok(NULL, " \n");
                if (token != NULL) {
                    idx = atoi(token);
                    val = list_remove_at_index(mylist, idx);
                    sprintf(sbuf, "%d", val);
                }
            } else if (strcmp(token, "get") == 0) {
                token = strtok(NULL, " \n");
                if (token != NULL) {
                    idx = atoi(token);
                    val = list_get_elem_at(mylist, idx);
                    sprintf(sbuf, "%d", val);
                }
            } else if (strcmp(token, "print") == 0) {
                char *list_str = listToString(mylist);
                snprintf(sbuf, sizeof(sbuf), "%s", list_str);
            } else {
                snprintf(sbuf, sizeof(sbuf), "Unknown Command");
            }

            send(clientSocket, sbuf, strlen(sbuf) + 1, 0);
        }

        close(clientSocket);
        clientSocket = -1;
    }

    if (mylist != NULL) {
        list_free(mylist);
        mylist = NULL;
    }
    if (servSockD != -1) {
        close(servSockD);
        servSockD = -1;
    }
    return 0;
}