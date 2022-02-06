#include <unistd.h>

/*This function helps in making the vehicles move by changing their x OR y values
x for horizontal 
and 
y for vertical
*/

void *movementFunc(void *inter){
  Intersection *intersection = inter;
  while(1){
    for(int i=0;i<4;i++){
      for(int j=0;j<MAX_CONNECTIONS;j++){
        if(i == 0){
          intersection->monitor.traffic[i].lineup[j].y -= intersection->monitor.traffic[i].lineup[j].speed;
        }
        else if(i == 1){
          intersection->monitor.traffic[i].lineup[j].y += intersection->monitor.traffic[i].lineup[j].speed;
        }
        else if(i == 2){
          intersection->monitor.traffic[i].lineup[j].x += intersection->monitor.traffic[i].lineup[j].speed;
        }
        else{
          intersection->monitor.traffic[i].lineup[j].x -= intersection->monitor.traffic[i].lineup[j].speed;
        }
      }
      usleep(20000);
    }
  }
}
