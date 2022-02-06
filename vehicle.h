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

/*In this fuction I send all the connect and updates request to the server and get the corresponding things
I am continuously sending and receiving the buffer and rsponse to get the server/client talking back and forth

The connect request function sends a connect request to the server. and returns the ID
*/

// A vehicle starts be specifying (as command-line arguments) its incoming direction
// (i.e., NORTHBOUND=0, SOUTHBOUND=1, EASTBOUND =2, WESTBOUND=3) as well as its speed
// (i.e., 1 to 100% of its top speed).
int connectRequest(int speed, int direction,int clientSocket){
  char buffer[30];
  int bytesRcv;
  char* inStr = "connect";
  char* requestID = "ID";
  int receivedID=0;
  // Send command string to server
  printf("CLIENT: Sending \"%s\" to server.\n", inStr);
  send(clientSocket, inStr, strlen(inStr), 0);

  // Get response from server, should be "OK"
  bytesRcv = recv(clientSocket, buffer, 80, 0);
  buffer[bytesRcv] = 0;
  printf("CLIENT: Got back response %s\n",buffer);
  if(strcmp(buffer,"OK") == 0){

    printf("CLIENT: Successfully connected the vehicle to the traffic monitor\n");

    sprintf(buffer,"%d",direction);
    send(clientSocket, buffer, strlen(buffer), 0);

    bytesRcv = recv(clientSocket, buffer, 80, 0);
    buffer[bytesRcv] = 0;
    printf("CLIENT: Got back response %s\n",buffer);

    sprintf(buffer,"%d",speed);
    send(clientSocket, buffer, strlen(buffer), 0);

    bytesRcv = recv(clientSocket, buffer, 80, 0);
    buffer[bytesRcv] = 0;
    printf("CLIENT: Got back response %s\n",buffer);

    send(clientSocket, requestID, strlen(requestID), 0);

    bytesRcv = recv(clientSocket, buffer, 80, 0);
    buffer[bytesRcv] = 0;
    if(strcmp(buffer,"ERROR") != 0){
      receivedID = (atoi)(buffer);
      printf("CLIENT: Got back id %s\n",buffer);
    }
  }
  return receivedID;
}

int main(int argc, char * argv[]) {
  int                 direction, speed;
  int                 receivedID;
  int                 clientSocket;
  struct sockaddr_in  clientAddress;
  int                 status, bytesRcv;
  char*               inStr = "connect";    // stores user input from keyboard
  char                buffer[80];   // stores sent and received data
  int                 result;
  char*               updateRequest = "update";
  char*               resultRequest = "result";
  char*               requestDistance = "distance";
  char*               requestState = "state";
  char*               requestPrevSpeed = "prevSpeed";
  char*               requestPrevDistance = "prevDistance";
  unsigned char       distance,stateOfLight,prevSpeed,prevDistance;
  int                 results = 0;
  if(argc != 3){
     printf("Input exactly 3 elements, your cmd line should be \"./exe direction speed\" where exe is the executible file,\
      direction is your preferred direction and speed is the vehicle speed you want your vehicle to be\n");
     exit(-1);
  }

  direction = (atoi)(argv[1]);
  if(direction < 0 || direction > 3){
    printf("Your direction is not appropriate please enter a number between 0-3 where ,\n");
    printf("0 means NORHTBOUND\n");
    printf("1 means SOUTHBOUND\n");
    printf("2 means EASTBOUND\n");
    printf("3 means WESTBOUND\n");
    exit(-1);
  }

  speed = (atoi)(argv[2]);
  if(speed < 0){
    printf("Enter a speed which is positive (>0)\n");
    exit(-1);
  }
  else if(speed >= 100){
    speed = 8;
  }
  else{
    speed = (int)speed/12.5;
    printf("JIGAR THE SPEED WAS: %d\n",speed);
  }

  clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (clientSocket < 0) {
    printf("*** CLIENT ERROR: Could open socket.\n");
    exit(-1);
  }

  memset(&clientAddress, 0, sizeof(clientAddress));
  clientAddress.sin_family = AF_INET;
  clientAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
  clientAddress.sin_port = htons((unsigned short) SERVER_PORT);

  status = connect(clientSocket, (struct sockaddr *) &clientAddress, sizeof(clientAddress));
  if (status < 0) {
    printf("*** CLIENT ERROR: Could not connect.\n");
    exit(-1);
  }
  receivedID = connectRequest(speed,direction,clientSocket);
  if(receivedID == 0)
     while(1){usleep(500000);}
     ///*
  char fakeBool1 = 1;
  while (fakeBool1) {
    printf("CLIENT: Sending \"%s\" request to server.\n", updateRequest);
    send(clientSocket, updateRequest, strlen(updateRequest), 0);
    printf("request sent\n");
    // Get response from server, should be "OK"
    bytesRcv = recv(clientSocket, buffer, 80, 0);
    buffer[bytesRcv] = 0;
    printf("CLIENT: Got back response %s\n",buffer);
    if(strcmp(buffer,"OK") == 0){
      printf("CLIENT: Update requesnt accepted by the traffic monitor\n");

      sprintf(buffer,"%d",direction);
      send(clientSocket, buffer, strlen(buffer), 0);

      bytesRcv = recv(clientSocket, buffer, 80, 0);
      buffer[bytesRcv] = 0;
      printf("CLIENT: Got back response %s\n",buffer);

      sprintf(buffer,"%d",speed);
      printf("SENDING SPEED: %d\n",speed);
      send(clientSocket, buffer, strlen(buffer), 0);

      bytesRcv = recv(clientSocket, buffer, 80, 0);
      buffer[bytesRcv] = 0;
      printf("CLIENT: Got back response %s\n",buffer);

      sprintf(buffer,"%d",receivedID);
      send(clientSocket, buffer, strlen(buffer), 0);

      bytesRcv = recv(clientSocket, buffer, 80, 0);
      buffer[bytesRcv] = 0;
      printf("CLIENT: Got back response %s\n",buffer);

      send(clientSocket, resultRequest, strlen(resultRequest), 0);

      strcpy(buffer,"");
      bytesRcv = recv(clientSocket, buffer, 80, 0);
      buffer[bytesRcv] = 0;
      printf("CLIENT: Got back response %s\n",buffer);
      results = (atoi)(buffer);
      if(results == YES){
        send(clientSocket, requestDistance, strlen(requestDistance), 0);

        strcpy(buffer,"");
        bytesRcv = recv(clientSocket, buffer, 80, 0);
        buffer[bytesRcv] = 0;
        printf("CLIENT: Got back response %s\n",buffer);
        distance = (atoi)(buffer);

        send(clientSocket, requestState, strlen(requestState), 0);

        strcpy(buffer,"");
        bytesRcv = recv(clientSocket, buffer, 80, 0);
        buffer[bytesRcv] = 0;
        printf("CLIENT: Got back response %s\n",buffer);
        stateOfLight = (atoi)(buffer);

        send(clientSocket, requestPrevSpeed, strlen(requestDistance), 0);

        strcpy(buffer,"");
        bytesRcv = recv(clientSocket, buffer, 80, 0);
        buffer[bytesRcv] = 0;
        printf("CLIENT: Got back response %s\n",buffer);
        prevSpeed = (atoi)(buffer);

        send(clientSocket, requestPrevDistance, strlen(requestState), 0);

        strcpy(buffer,"");
        bytesRcv = recv(clientSocket, buffer, 80, 0);
        buffer[bytesRcv] = 0;
        printf("CLIENT: Got back response %s\n",buffer);
        prevDistance = (atoi)(buffer);

        if(prevDistance == MAX_DISTANCE){
          if(stateOfLight != GREEN && distance != MAX_DISTANCE){
            if(distance <= ((2*VEHICLE_WIDTH)+speed)){
              speed = 0;
            }
            else if(distance <=((3*VEHICLE_WIDTH)*speed)){
              if(speed >= 3){
                speed = (int)speed /1.5;
              }
            }
          }
          else{
            if(speed != VEHICLE_TOP_SPEED)
              speed += 1;
          }
        }
        else{
          if(stateOfLight != GREEN && distance != MAX_DISTANCE){
            if(distance <= ((2*VEHICLE_WIDTH)+speed)){
              speed = 0;
            }
            else if(distance <=((3*VEHICLE_WIDTH)*speed)){
              if(speed >= 3){
                speed = (int)speed /1.5;
              }
            }
          }
          if(speed > prevSpeed){
            if(distance <= ((4*VEHICLE_WIDTH)+speed)){
              speed = 0;
            }
            else if(distance <=((3*VEHICLE_WIDTH)*speed)){
              if(speed >= 3){
                speed = (int)speed /1.5;
              }
            }
          }
          else if((prevSpeed > speed) && ((distance == MAX_DISTANCE) || (stateOfLight == GREEN))){
            if(speed != VEHICLE_TOP_SPEED)
              speed += 1;
          }
        }
        //logic ends
      }
    }
    usleep(500000);
    if(results == NO){
      fakeBool1 = 0;
      break;
    }
  }
 //*/
  printf("Shutting down the client socket\n");
  close(clientSocket);
}
