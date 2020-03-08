/*CAT program*/

#include "copyright.h"
#include "syscall.h"

#define MAX_FILE_LEN 255


int main(int argc, char* argv[])
{
	char c;
	char fileName[MAX_FILE_LEN];
	int fileSize;
	int i ;
	
	
	OpenFileID inConsole = OpenFileFunc("stdin", 1);
	OpenFileID fileID ;
	
	if (inConsole == -1 )
	{
		PrintString("Can not open console\n");
		Halt();
	}
	// Read file name.
	PrintString("Input file name: ");
	ReadFile(fileName, MAX_FILE_LEN, inConsole); 
	
	// Read file.
	fileID = OpenFileFunc(fileName, 1);
	if(fileID == -1)
	{
		PrintString("Can not open file");
		CloseFile(fileID);
		Halt();
	}
	else
	{
		fileSize = SeekFile(-1, fileID);	
		SeekFile(0, fileID);
		i = 0;
		for (; i < fileSize; ++i)
		{
			ReadFile(&c, 1, fileID);
			PrintChar(c);
		}
	}
	
	CloseFile(fileID);
	CloseFile(inConsole);

	Halt();
}
