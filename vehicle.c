#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "trafficLight.h"
#include "vehicle.h"
#include "simulator.h"


// A vehicle starts be specifying (as command-line arguments) its incoming direction 
// (i.e., NORTHBOUND=0, SOUTHBOUND=1, EASTBOUND =2, WESTBOUND=3) as well as its speed 
// (i.e., 1 to 100% of its top speed).
int main(int argc, char * argv[]) {

	// WRITE CODE HERE TOO

  	// Go into an infinite loop to keep sending/getting updates from traffic monitor
  	while(1) {
    		
    		
    	usleep(50000);  // A delay to slow things down a little
  	}
}

