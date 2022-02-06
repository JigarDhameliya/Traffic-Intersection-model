#include <unistd.h>

/*
This program makes all the light change their states accordingly to their delays
*/

char lightColorsV[] = {DELAY_RED_2,GREEN,YELLOW,DELAY_RED_1,RED};
char lightColorsH[] = {DELAY_RED_1,RED,DELAY_RED_2,GREEN,YELLOW};
char timeV[] = {DELAY_RED_COUNTDOWN,GREEN_COUNTDOWN,YELLOW_COUNTDOWN,DELAY_RED_COUNTDOWN,RED_COUNTDOWN};
char timeH[] = {DELAY_RED_COUNTDOWN,RED_COUNTDOWN,DELAY_RED_COUNTDOWN,GREEN_COUNTDOWN,YELLOW_COUNTDOWN};

// Set the state of the given traffic light to the newState
void setState(TrafficLight *t, char newState) {
	t->currentState = newState;
}

// This code should run in an infinite loop continuously simulating a timed traffic light.
void *runTrafficLight(void *t) {
	TrafficLight  *light = t;
	if(light->verticalDirection == -1 || light->verticalDirection == 1){
		while(1) {
			for(int i=0;i<5;i++){
				setState(light,lightColorsV[i]);
				light->countDownTimer = timeV[i];
				int temp = light->countDownTimer;
				while(temp != 0){
					usleep(500000);
					temp--;
				}
			}
	 	}
	}
	else{
		while(1) {
			for(int i=0;i<5;i++){
				setState(light,lightColorsH[i]);
				light->countDownTimer = timeH[i];
				int temp = light->countDownTimer;
				temp = timeH[i];
				while(temp != 0){
					usleep(500000);
					temp--;
				}
			}
	 	}
	}
	pthread_exit(NULL);
}
