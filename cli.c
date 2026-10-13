#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 9001
#define MAX_COMMAND_LINE_LEN 1024

char *getCommandLine(char *command_line) {
    do {
        if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
            fprintf(stderr, "fgets error");
            exit(0);
        }
    } while (command_line[0] == 0x0A); // while just ENTER pressed
    command_line[strlen(command_line) - 1] = '\0';
    return command_line;
}

int main(int argc, char const *argv[]) {
    int sockID = socket(AF_INET, SOCK_STREAM, 0);
    if (sockID < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    char *token;
    char temp_buf[MAX_COMMAND_LINE_LEN];
    char buf[MAX_COMMAND_LINE_LEN];
    char responeData[MAX_COMMAND_LINE_LEN];
    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    int connectStatus = connect(sockID, (struct sockaddr *)&servAddr, sizeof(servAddr));
    if (connectStatus == -1) {
        printf("Error connecting to server...\n");
        exit(1);
    }

    while (1) {
        printf("Enter Command (or menu): ");
        getCommandLine(buf);

        // Check command locally before sending
        strncpy(temp_buf, buf, sizeof(temp_buf));
        token = strtok(temp_buf, " ");

        if (token != NULL && strcmp(token, "menu") == 0) {
            printf("COMMANDS:\n---------\n1. print\n2. get_length\n3. add_back <value>\n4. add_front <value>\n5. add_position <index> <value>\n6. remove_back\n7. remove_front\n8. remove_position <index>\n9. get <index>\n10. exit\n");
            continue; // Do not send 'menu' to server
        }

        if (token != NULL && strcmp(token, "exit") == 0) {
            send(sockID, buf, strlen(buf), 0);
            close(sockID);
            exit(0); // Exit cleanly with code 0
        }

        // Send valid command
        send(sockID, buf, strlen(buf), 0);

        memset(responeData, '\0', sizeof(responeData));
        int n = recv(sockID, responeData, sizeof(responeData) - 1, 0);
        if (n <= 0) {
            break;
        }
        responeData[n] = '\0';
        printf("\nSERVER RESPONSE: %s\n", responeData);
        memset(buf, '\0', MAX_COMMAND_LINE_LEN);
    }

    close(sockID);
    return 0;
}