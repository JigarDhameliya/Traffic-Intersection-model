#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>



// Handle client requests coming in through the server socket.  This code should run
// indefinitiely.  It should wait for a client to send a request, process it, and then
// close the client connection and wait for another client.  The requests that may be
// handles are SHUTDOWN, CONNECT and UPDATE.  
void *handleIncomingRequests(void *inter) {
	int cmd;
	
  	// Wait for clients now
  	while (1) {
    	switch(cmd) {
    		case SHUTDOWN: // Someone has requested that we shut down, so do it.
      			break;
      
    		case CONNECT:  // A vehicle wants to connect to the traffic monitor
      			break;
      
    		case UPDATE:   // A vehicle is sending new speed data
      			break;
    	}
  	}
}
