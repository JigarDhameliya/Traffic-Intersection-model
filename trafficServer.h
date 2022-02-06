#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000

// Handle client requests coming in through the server socket.  This code should run
// indefinitiely.  It should wait for a client to send a request, process it, and then
// close the client connection and wait for another client.  The requests that may be
// handles are SHUTDOWN, CONNECT and UPDATE.
void *handleIncomingRequests(void *inter) {

  int 		            cmd;
  int                 serverSocket, clientSocket;
  struct sockaddr_in  serverAddress, clientAddr;
  int                 status, addrSize, bytesRcv;
  unsigned char       direction, updateDir;
  unsigned char       speed, updateSpeed;
  char                buffer[30];
  char*               okResponse = "OK";
  char                response[30] = "OK";
  char*               declineResponse = "ERROR";
  unsigned char       sendID, updateID;
  int                 distance = MAX_DISTANCE;
  int                 stateOfLight = GREEN;
  int                 prevSpeed = VEHICLE_TOP_SPEED + 1;
  int                 prevDistance = MAX_DISTANCE;
  int                 updateResult = YES;
  char                fakeBool2 = 0;
  Intersection        *intersection = inter;
  // Create the server socket
  serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serverSocket < 0) {
    printf("*** SERVER ERROR: Could not open socket.\n");
    exit(-1);
  }
  printf("Server created\n");

  // Setup the server address
  memset(&serverAddress, 0, sizeof(serverAddress)); // zeros the struct
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddress.sin_port = htons((unsigned short) SERVER_PORT);

  // Bind the server socket
  status = bind(serverSocket,  (struct sockaddr *)&serverAddress, sizeof(serverAddress));
  if (status < 0) {
    printf("*** SERVER ERROR: Could not bind socket.\n");
    exit(-1);
  }
  printf("Server binded\n");

  // Set up the line-up to handle up to 5 clients in line
  status = listen(serverSocket, 5);
  if (status < 0) {
    printf("*** SERVER ERROR: Could not listen on socket.\n");
    exit(-1);
  }
  printf("server is listening\n");
  // Wait for clients now
  while (1) {
    addrSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddr, &addrSize);
    if (clientSocket < 0) {
      printf("*** SERVER ERROR: Could not accept incoming client connection.\n");
      exit(-1);
    }
    printf("SERVER: Received client connection.\n");
    // Get the message from the client
    bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[bytesRcv] = 0; // put a 0 at the end so we can display the string
    printf("SERVER: Received client request: %s\n", buffer);
    // Respond with an "OK" message
    printf("SERVER: Sending \"%s\" to client\n", okResponse);
    send(clientSocket, okResponse, strlen(okResponse), 0);
    // Go into infinite loop to talk to client
    if (strcmp(buffer,"stop") == 0){
      cmd = SHUTDOWN;
    }
    else if(strcmp(buffer,"connect") == 0){
      cmd = CONNECT;
    }
    else if(strcmp(buffer,"update") == 0){
      cmd = UPDATE;
    }
    switch (cmd) {
      case SHUTDOWN:
        intersection->monitor.online = 0;
        break;
      case CONNECT:
        printf("CMD2: Receiving direction \n");
        bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
        direction = (atoi)(buffer);
        printf("CMD2: Direction received is: %d\n",direction);

        printf("SERVER: Sending \"%s\" to client\n", response);
        send(clientSocket, response, strlen(response), 0);

        printf("CMD2: Receiving Speed \n");
        bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
        speed = (atoi)(buffer);
        printf("CMD2: Speed received is: %d\n",speed);

        printf("SERVER: Sending \"%s\" to client\n", response);
        send(clientSocket, response, strlen(response), 0);

        printf("Line count is: %d\n",intersection->monitor.traffic[direction].lineCount);

        if(intersection->monitor.traffic[direction].lineCount < MAX_CONNECTIONS){
          int numVehicles = intersection->monitor.traffic[direction].lineCount;
          intersection->monitor.traffic[direction].lineup[numVehicles].x = intersection->monitor.traffic[direction].entryX;
          intersection->monitor.traffic[direction].lineup[numVehicles].y = intersection->monitor.traffic[direction].entryY;
          intersection->monitor.traffic[direction].lineup[numVehicles].speed = speed;
          intersection->monitor.traffic[direction].lineup[numVehicles].id = intersection->monitor.idCounter[direction];
          intersection->monitor.traffic[direction].lineCount++;

          printf("CMD2: Receiving ID request \n");
          bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
          buffer[bytesRcv] = 0;
          printf("CMD2: response received is: %s\n",buffer);

          //sprintf(sendID,"%d",intersection->monitor.idCounter[direction]);
          sendID = intersection->monitor.idCounter[direction];
          sprintf(buffer,"%d",sendID);
          send(clientSocket, buffer, strlen(buffer), 0);

          if(intersection->monitor.idCounter[direction] == 255){
            intersection->monitor.idCounter[direction] = 1;
          }
          else{
            intersection->monitor.idCounter[direction]++;
          }
          printf("The direction is: %d\n",direction);
          printf("The x is: %d\n",intersection->monitor.traffic[direction].lineup[numVehicles-1].x);
          printf("The y is: %d\n",intersection->monitor.traffic[direction].lineup[numVehicles-1].y);
          printf("The speed is: %d\n",intersection->monitor.traffic[direction].lineup[numVehicles-1].speed);
          printf("The id is: %d\n",intersection->monitor.traffic[direction].lineup[numVehicles-1].id);
          printf("The line count is: %d\n",intersection->monitor.traffic[direction].lineCount);
        }//if
        else{
          send(clientSocket, declineResponse, strlen(declineResponse), 0);
          printf("SORRY the number of Vehicles have already reached %d\n",MAX_CONNECTIONS);
        }
        if(updateResult == NO){
        	break;
        }
      case UPDATE:
          fakeBool2 = 0;
          printf("CMD3: Receiving direction \n");
          bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
          updateDir = (atoi)(buffer);
          printf("CMD3: Direction received is: %d\n",updateDir);

          strcpy(response,"gotDirection");
          printf("SERVER: Sending \"%s\" to client\n", response);
          send(clientSocket, response, strlen(response), 0);

          printf("CMD3: Receiving Speed \n");
          bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
          updateSpeed = (atoi)(buffer);
          printf("CMD3: Speed received is: %d\n",updateSpeed);

          strcpy(response,"gotSpeed");
          printf("SERVER: Sending \"%s\" to client\n", response);
          send(clientSocket, response, strlen(response), 0);

          printf("CMD3: Receiving ID \n");
          bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
          updateID = (atoi)(buffer);
          printf("CMD3: ID received is: %d\n",updateID);

          strcpy(response,"gotID");
          printf("SERVER: Sending \"%s\" to client\n", response);
          send(clientSocket, response, strlen(response), 0);

          char fakeBool = 1;
          int tempLineCount = intersection->monitor.traffic[updateDir].lineCount;
          for(int j=0; j<tempLineCount && fakeBool; j++){
            if(intersection->monitor.traffic[updateDir].lineup[j].id == updateID){
              fakeBool = 0;
              if(updateDir == 0 ){
                if(intersection->monitor.traffic[updateDir].exitY < intersection->monitor.traffic[updateDir].lineup[j].y){
                  if(intersection->monitor.traffic[updateDir].stopY < intersection->monitor.traffic[updateDir].lineup[j].y){
                    distance = intersection->monitor.traffic[updateDir].lineup[j].y - intersection->monitor.traffic[updateDir].stopY;
                    stateOfLight = intersection->lights[updateDir].currentState;
                  }
                  if(j != 0){
                    prevSpeed = intersection->monitor.traffic[direction].lineup[j-1].speed;
                    prevDistance = intersection->monitor.traffic[direction].lineup[j].y - intersection->monitor.traffic[direction].lineup[j-1].y;
                  }
                  updateResult = YES;
                }
                else{
                  for(int k=j;k<tempLineCount-1;k++){
                    intersection->monitor.traffic[updateDir].lineup[k].x = intersection->monitor.traffic[updateDir].lineup[k+1].x;
                    intersection->monitor.traffic[updateDir].lineup[k].y = intersection->monitor.traffic[updateDir].lineup[k+1].y;
                    intersection->monitor.traffic[updateDir].lineup[k].speed = intersection->monitor.traffic[updateDir].lineup[k+1].speed;
                    intersection->monitor.traffic[updateDir].lineup[k].id = intersection->monitor.traffic[updateDir].lineup[k+1].id;
                  }
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].x = 0;
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].y = 0;
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].speed = 0;
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].id = 0;
                  intersection->monitor.traffic[updateDir].lineCount -= 1;
                  updateResult = NO;
                }
              }
              else if(updateDir == 1 ){
                if(intersection->monitor.traffic[updateDir].exitY > intersection->monitor.traffic[updateDir].lineup[j].y){
                  if(intersection->monitor.traffic[updateDir].stopY > intersection->monitor.traffic[updateDir].lineup[j].y){
                    distance = intersection->monitor.traffic[updateDir].stopY - intersection->monitor.traffic[updateDir].lineup[j].y;
                    stateOfLight = intersection->lights[updateDir].currentState;
                  }
                  if(j != 0){
                    prevSpeed = intersection->monitor.traffic[direction].lineup[j-1].speed;
                    prevDistance = intersection->monitor.traffic[direction].lineup[j-1].y - intersection->monitor.traffic[direction].lineup[j].y;
                  }
                  updateResult = YES;
                }
                else{
                  for(int k=j;k<tempLineCount-1;k++){
                    intersection->monitor.traffic[updateDir].lineup[k].x = intersection->monitor.traffic[updateDir].lineup[k+1].x;
                    intersection->monitor.traffic[updateDir].lineup[k].y = intersection->monitor.traffic[updateDir].lineup[k+1].y;
                    intersection->monitor.traffic[updateDir].lineup[k].speed = intersection->monitor.traffic[updateDir].lineup[k+1].speed;
                    intersection->monitor.traffic[updateDir].lineup[k].id = intersection->monitor.traffic[updateDir].lineup[k+1].id;
                  }
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].x = 0;
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].y = 0;
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].speed = 0;
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].id = 0;
                  intersection->monitor.traffic[updateDir].lineCount -= 1;
                  updateResult = NO;
                }
              }
              else if(updateDir == 2 ){
                if(intersection->monitor.traffic[updateDir].exitX > intersection->monitor.traffic[updateDir].lineup[j].x){
                  if(intersection->monitor.traffic[updateDir].stopX > intersection->monitor.traffic[updateDir].lineup[j].x){
                    distance = intersection->monitor.traffic[updateDir].lineup[j].x + intersection->monitor.traffic[updateDir].stopX;
                    stateOfLight = intersection->lights[updateDir].currentState;
                  }
                  if(j != 0){
                    prevSpeed = intersection->monitor.traffic[direction].lineup[j-1].speed;
                    prevDistance = intersection->monitor.traffic[direction].lineup[j-1].x - intersection->monitor.traffic[direction].lineup[j].x;
                  }
                  updateResult = YES;
                }
                else{
                  for(int k=j;k<tempLineCount-1;k++){
                    intersection->monitor.traffic[updateDir].lineup[k].x = intersection->monitor.traffic[updateDir].lineup[k+1].x;
                    intersection->monitor.traffic[updateDir].lineup[k].y = intersection->monitor.traffic[updateDir].lineup[k+1].y;
                    intersection->monitor.traffic[updateDir].lineup[k].speed = intersection->monitor.traffic[updateDir].lineup[k+1].speed;
                    intersection->monitor.traffic[updateDir].lineup[k].id = intersection->monitor.traffic[updateDir].lineup[k+1].id;
                  }
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].x = 0;
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].y = 0;
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].speed = 0;
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].id = 0;
                  intersection->monitor.traffic[updateDir].lineCount -= 1;
                  updateResult = NO;
                }
              }
              else if(updateDir == 3 ){
                if(intersection->monitor.traffic[updateDir].exitX < intersection->monitor.traffic[updateDir].lineup[j].x){
                  if(intersection->monitor.traffic[updateDir].stopX < intersection->monitor.traffic[updateDir].lineup[j].x){
                    distance = intersection->monitor.traffic[updateDir].lineup[j].x - intersection->monitor.traffic[updateDir].stopX;
                    stateOfLight = intersection->lights[updateDir].currentState;
                  }
                  if(j != 0){
                    prevSpeed = intersection->monitor.traffic[direction].lineup[j-1].speed;
                    prevDistance = intersection->monitor.traffic[direction].lineup[j-1].x - intersection->monitor.traffic[direction].lineup[j].x;
                  }
                  updateResult = YES;
                }
                else{
                  for(int k=j;k<tempLineCount-1;k++){
                    intersection->monitor.traffic[updateDir].lineup[k].x = intersection->monitor.traffic[updateDir].lineup[k+1].x;
                    intersection->monitor.traffic[updateDir].lineup[k].y = intersection->monitor.traffic[updateDir].lineup[k+1].y;
                    intersection->monitor.traffic[updateDir].lineup[k].speed = intersection->monitor.traffic[updateDir].lineup[k+1].speed;
                    intersection->monitor.traffic[updateDir].lineup[k].id = intersection->monitor.traffic[updateDir].lineup[k+1].id;
                  }
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].x = 0;
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].y = 0;
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].speed = 0;
                  intersection->monitor.traffic[updateDir].lineup[tempLineCount].id = 0;
                  intersection->monitor.traffic[updateDir].lineCount -= 1;
                  updateResult = NO;
                }
              }
            }
          }
          if(fakeBool)
            updateResult = NO;
          printf("CMD3: Receiving result request\n");
          bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
          buffer[bytesRcv] = 0;
          printf("CMD3: request received is: %s\n",buffer);

          printf("SERVER: The result is: %d\n",updateResult);
          strcpy(buffer,"");
          sprintf(buffer,"%d",updateResult);
          send(clientSocket, buffer, strlen(buffer), 0);

          if(updateResult == YES){
            printf("CMD3: Receiving distance request\n");
            bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesRcv] = 0;
            printf("CMD3: request received is: %s\n",buffer);

            printf("SERVER: The distance is: %d\n",distance);
            strcpy(buffer,"");
            sprintf(buffer,"%d",distance);
            send(clientSocket, buffer, strlen(buffer), 0);

            printf("CMD3: Receiving stateOfLight request\n");
            bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesRcv] = 0;
            printf("CMD3: request received is: %s\n",buffer);

            printf("SERVER: The stateOfLight is: %d\n",stateOfLight);
            strcpy(buffer,"");
            sprintf(buffer,"%d",stateOfLight);
            send(clientSocket, buffer, strlen(buffer), 0);

            printf("CMD3: Receiving prevSpeed request\n");
            bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesRcv] = 0;
            printf("CMD3: request received is: %s\n",buffer);

            printf("SERVER: The prevSpeed is: %d\n",prevSpeed);
            strcpy(buffer,"");
            sprintf(buffer,"%d",prevSpeed);
            send(clientSocket, buffer, strlen(buffer), 0);

            printf("CMD3: Receiving prevDistance request\n");
            bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesRcv] = 0;
            printf("CMD3: request received is: %s\n",buffer);

            printf("SERVER: The prevDistance is: %d\n",prevDistance);
            strcpy(buffer,"");
            sprintf(buffer,"%d",prevDistance);
            send(clientSocket, buffer, strlen(buffer), 0);
            break;
          }
          break;
        if(updateResult == NO){
        	break;
        }
    }//switch case
    printf("SERVER: Closing client connection.\n");
    close(clientSocket);
    if(intersection->monitor.online != '1'){
      
      break;
    }
      
  }//while true
  // Don't forget to close the sockets!
  printf("SERVER: Shutting down SERVER.\n");
  close(serverSocket);
  //printf("SERVER: Shutting down.\n");
  pthread_exit(NULL);
}//main

