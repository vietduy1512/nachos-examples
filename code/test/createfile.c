#include "syscall.h"
#include "copyright.h"

int
main()
{
	OpenFileID open;

	if (CreateFile("TESTFILE.txt") == -1)
	{
		PrintString("Create File Failed");
	}
	else
	{
		PrintString("Create File Successful");

		CloseFile(open);
	}

	Halt();
}

