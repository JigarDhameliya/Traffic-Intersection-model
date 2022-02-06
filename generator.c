#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "trafficLight.h"
#include "vehicle.h"
#include "simulator.h"

//This function generates random vehicles

void main() {

  while(1){

    srand(time(NULL));

    char buffer1[17] = "./vehicle 0 50 &";
    char buffer2[17] = "./vehicle 1 50 &";
    char buffer3[17] = "./vehicle 2 50 &";
    char buffer4[17] = "./vehicle 3 50 &";

    int randomNumber = rand()%4;
    if(randomNumber == 0){
      system(buffer1);
    }
    else if(randomNumber == 1){
      system(buffer2);
    }
    else if(randomNumber == 2){
      system(buffer3);
    }
    else{
      system(buffer4);
    }

    usleep(500000);
  }
}
