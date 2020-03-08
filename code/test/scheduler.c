#include "syscall.h"

int main() 
{	

	
	SpaceId pingPID, pongPID;
	PrintString("Ping-Pong test starting ...\n\n");
	
	pingPID = Exec("./test/ping");
	pongPID = Exec("./test/pong");
	
	Join(pingPID);
	Join(pongPID);

	//Halt();
} 

