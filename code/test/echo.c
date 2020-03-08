/* echo program */

#include "syscall.h"
#include "copyright.h"
#define MAX_FILE_LEN 	255

int main(int argc, char* argv[])
{
	char buffer[MAX_FILE_LEN];
	OpenFileID inConsole = OpenFileFunc("stdin", 1);
	OpenFileID outConsole = OpenFileFunc("stdout", 0);
	if (inConsole == -1 || outConsole == -1)
	{
		PrintString("Can not open console\n");
		Halt();
	}
	ReadFile(buffer, MAX_FILE_LEN, inConsole); 
	
	
	WriteFile(buffer, MAX_FILE_LEN, outConsole);
	CloseFile(inConsole);
	CloseFile(outConsole);

	Halt();
}

