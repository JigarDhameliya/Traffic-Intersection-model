#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000

#include "trafficLight.h"
#include "vehicle.h"
#include "simulator.h"

/*
This file sends a stop signal that kills the whole program
*/

void main() {
 int                 clientSocket;
  struct sockaddr_in  clientAddress;
  int                 status, bytesRcv;

  char*                inStr = "stop";    // stores user input from keyboard
  char                buffer[80];   // stores sent and received data

  // Create socket
  clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (clientSocket < 0) {
    printf("*** CLIENT ERROR: Could open socket.\n");
    exit(-1);
  }

  // Setup address
  memset(&clientAddress, 0, sizeof(clientAddress));
  clientAddress.sin_family = AF_INET;
  clientAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
  clientAddress.sin_port = htons((unsigned short) SERVER_PORT);

  // Connect to server
  status = connect(clientSocket, (struct sockaddr *) &clientAddress, sizeof(clientAddress));
  if (status < 0) {
    printf("*** CLIENT ERROR: Could not connect.\n");
    exit(-1);
  }

  // Go into loop to commuincate with server now
  while (1) {
    // Get a command from the user
    //printf("CLIENT: Enter command to send to server ... ");
    //scanf("%s", inStr);

    // Send command string to server
    //strcpy(buffer, inStr);
    printf("CLIENT: Sending \"%s\" to server.\n", inStr);
    send(clientSocket, inStr, strlen(inStr), 0);

    // Get response from server, should be "OK"
    bytesRcv = recv(clientSocket, buffer, 80, 0);
    buffer[bytesRcv] = 0; // put a 0 at the end so we can display the string
    printf("CLIENT: Got back response \"%s\" from server.\n", buffer);

    if (strcmp(inStr,"stop") == 0)
      break;
  }
  close(clientSocket);  // Don't forget to close the socket !
  printf("CLIENT: Shutting down.\n");
}

