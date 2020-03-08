/* copy program */
#include "syscall.h"
#include "copyright.h"

#define MAX_FILE_LEN 255

int main(int argc, char* argv[])
{
	
	OpenFileID srcId;
	OpenFileID dstId;
	char source[MAX_FILE_LEN], dest[MAX_FILE_LEN];
	int filesz,srcsz, i;
	char c;
	
	OpenFileID inConsole = OpenFileFunc("stdin", 1);
	if(inConsole == -1)
	{
		PrintString("\nCan not open console");
		Halt();
	}
		
	PrintString("Input source file:");
	ReadFile(source, MAX_FILE_LEN, inConsole);
	
	PrintString("Input destination file:");
	ReadFile(dest, MAX_FILE_LEN, inConsole);
	
	srcId = OpenFileFunc(source, 1);
	
	if( CreateFile(dest) == -1)
	{
		dstId = OpenFileFunc(dest, 0);
		if (srcId == -1 || dstId == -1)
		{
			PrintString("\nCan not open file");
			PrintString("\nTerminate program\n");
			Halt();
		}
	}
	dstId = OpenFileFunc(dest, 0);
	filesz = SeekFile(-1, srcId);
	
	SeekFile(0, srcId);
	SeekFile(0, dstId);
	
	// Can viet ham ReadFile va WriteFile. ??
	for (i = 0; i < filesz; ++i)
	{
		ReadFile(&c, 1, srcId);
		WriteFile(&c, 1, dstId);
	}
	CloseFile(srcId);
	CloseFile(dstId);
	Halt();
}

