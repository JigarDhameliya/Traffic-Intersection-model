#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include "trafficLight.h"
#include "vehicle.h"
#include "simulator.h"

#include "display.c"
#include "movementTimer.c"
#include "trafficLight.c"
#include "trafficServer.c"

/*
In this function I create threads and join them.
it is the main function of the assignment as it controls everything.
*/


int main() {
	Intersection     intersection;

	// Add the traffic lights to the intersection
  	setState(&intersection.lights[WESTBOUND], DELAY_RED_1);
  	intersection.lights[WESTBOUND].verticalDirection = 0;
  	intersection.lights[WESTBOUND].horizontalDirection = -1;
  	intersection.lights[WESTBOUND].xOffset = INTERSECTION_WIDTH/2 + ROAD_WIDTH;
  	intersection.lights[WESTBOUND].yOffset = INTERSECTION_HEIGHT/2 - ROAD_WIDTH - TRAFFIC_LIGHT_SIZE-4;
  
  	setState(&intersection.lights[NORTHBOUND], DELAY_RED_2);
  	intersection.lights[NORTHBOUND].verticalDirection = -1;
  	intersection.lights[NORTHBOUND].horizontalDirection = 0;
  	intersection.lights[NORTHBOUND].xOffset = INTERSECTION_WIDTH/2 + ROAD_WIDTH;
  	intersection.lights[NORTHBOUND].yOffset = INTERSECTION_HEIGHT/2 + ROAD_WIDTH;
	
  	setState(&intersection.lights[EASTBOUND], DELAY_RED_1);
  	intersection.lights[EASTBOUND].verticalDirection = 0;
  	intersection.lights[EASTBOUND].horizontalDirection = 1;
  	intersection.lights[EASTBOUND].xOffset = INTERSECTION_WIDTH/2 - ROAD_WIDTH - TRAFFIC_LIGHT_SIZE*3-4;
  	intersection.lights[EASTBOUND].yOffset = INTERSECTION_HEIGHT/2 + ROAD_WIDTH;
	
  	setState(&intersection.lights[SOUTHBOUND], DELAY_RED_2);
  	intersection.lights[SOUTHBOUND].verticalDirection = 1;
  	intersection.lights[SOUTHBOUND].horizontalDirection = 0;
  	intersection.lights[SOUTHBOUND].xOffset = INTERSECTION_WIDTH/2 - ROAD_WIDTH - TRAFFIC_LIGHT_SIZE-4;
  	intersection.lights[SOUTHBOUND].yOffset = INTERSECTION_HEIGHT/2 - ROAD_WIDTH - TRAFFIC_LIGHT_SIZE*3-4;
  
  	// Set up the Traffic Lineups to the Traffic Monitor for all 4 directions
  	short entrypointsX[4] = {INTERSECTION_WIDTH/2+ROAD_WIDTH/2, INTERSECTION_WIDTH/2-ROAD_WIDTH/2, 
  			  			     0-(VEHICLE_WIDTH*2), INTERSECTION_WIDTH+(VEHICLE_WIDTH*2)};
  	short entrypointsY[4] = {INTERSECTION_HEIGHT+(VEHICLE_WIDTH*2), 0-(VEHICLE_WIDTH*2), 
  					  		 INTERSECTION_HEIGHT/2+ROAD_WIDTH/2, INTERSECTION_HEIGHT/2-ROAD_WIDTH/2};
  	short stoppointsX[4] = {INTERSECTION_WIDTH/2+ROAD_WIDTH/2, INTERSECTION_WIDTH/2-ROAD_WIDTH/2, 
  					 		INTERSECTION_WIDTH/2-ROAD_WIDTH-VEHICLE_WIDTH, INTERSECTION_WIDTH/2+ROAD_WIDTH+VEHICLE_WIDTH};
  	short stoppointsY[4] = {INTERSECTION_HEIGHT/2+ROAD_WIDTH+VEHICLE_WIDTH,INTERSECTION_HEIGHT/2-ROAD_WIDTH-VEHICLE_WIDTH, 
  					 		INTERSECTION_HEIGHT/2+ROAD_WIDTH/2, INTERSECTION_HEIGHT/2-ROAD_WIDTH/2};
  	short exitpointsX[4] = {INTERSECTION_WIDTH/2+ROAD_WIDTH/2, INTERSECTION_WIDTH/2-ROAD_WIDTH/2, 
  					 		INTERSECTION_WIDTH+(VEHICLE_WIDTH*2), 0-(VEHICLE_WIDTH*2)};
  	short exitpointsY[4] = {0-(VEHICLE_WIDTH*2), INTERSECTION_HEIGHT+(VEHICLE_WIDTH*2), 
  					 		INTERSECTION_HEIGHT/2+ROAD_WIDTH/2, INTERSECTION_HEIGHT/2-ROAD_WIDTH/2};
  	for (int i=0; i<4; i++) {
  		intersection.monitor.online = 0;
  		intersection.monitor.idCounter[i] = 1;
  		intersection.monitor.traffic[i].entryX = entrypointsX[i];
  		intersection.monitor.traffic[i].entryY = entrypointsY[i];
  		intersection.monitor.traffic[i].stopX = stoppointsX[i];
  		intersection.monitor.traffic[i].stopY = stoppointsY[i];
  		intersection.monitor.traffic[i].exitX = exitpointsX[i];
  		intersection.monitor.traffic[i].exitY = exitpointsY[i];
  		intersection.monitor.traffic[i].lineCount = 0;
  	}
  	
  	// Spawn the thread to handle display
  	pthread_t  thread_draw;
  	pthread_t threadsLights[4];
	pthread_t threadMonitor;
	pthread_t moveVehicle;
  	pthread_create(&thread_draw, NULL, showSimulation, (void *)(&intersection));
	pthread_create(&threadsLights[0],NULL,runTrafficLight,(void *)(&intersection.lights[NORTHBOUND]));
	pthread_create(&threadsLights[1],NULL,runTrafficLight,(void *)(&intersection.lights[SOUTHBOUND]));
	pthread_create(&threadsLights[2],NULL,runTrafficLight,(void *)(&intersection.lights[EASTBOUND]));
	pthread_create(&threadsLights[3],NULL,runTrafficLight,(void *)(&intersection.lights[WESTBOUND]));
	intersection.monitor.online = '1';
	pthread_create(&threadMonitor,NULL,handleIncomingRequests,(void *)(&intersection));
	pthread_create(&moveVehicle,NULL,movementFunc,(void *)(&intersection));
	pthread_join(threadMonitor,NULL);
	
}
