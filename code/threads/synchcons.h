// CAE - MULTI - SYNCHCONSOLE DEFINITION
#ifndef SYNCHCONS_H
#define SYNCHCONS_H

#include "console.h"
#include "openfile.h"

class SynchConsole {
public:
	

	SynchConsole();				// A SynchConsole Constructor
	SynchConsole(char *in, char *out);	// Same with fn pointers
	~SynchConsole();			// Delete a console instance

	int Read(char *into, int numBytes);	// Read synch line
						// Ends in EOLN or ^A
	int Write(char *from, int numBytes);	// Write a synchronous line

	OpenFile* 	GetFile(int _index);
	int		GetFileType(int _index);
	int		GetCurrentIndex();

	void		SetFile(int _index, OpenFile* file, int type);
	void 		IncreaseIndex();

private:
	Console		*cons;			// Pointer to an async console

	OpenFile* 	fileList[10];
	int 		fileTypeList[10];
	int 		index;
} ;

#endif // SYNCHCONS_H
// CAE - MULTI - END SECTION


