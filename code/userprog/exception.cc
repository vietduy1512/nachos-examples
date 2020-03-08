// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "synchcons.h"

#define MAXFILELENGTH 255

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space

// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space

char* User2System(int virtAddr,int limit)
{
	int i;// index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit +1];//need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf,0,limit+1);
	for (i = 0 ; i < limit ;i++)
	{
		machine->ReadMem(virtAddr+i,1,&oneChar);
		kernelBuf[i] = (char)oneChar;
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}


// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr,int len,char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0 ;
	do{
		oneChar= (int) buffer[i];
		machine->WriteMem(virtAddr+i,1,oneChar);
		i ++;
	}while(i < len && oneChar != 0);
	return i;
}


void
ExceptionHandler(ExceptionType which)
{
	static SynchConsole gSynchConsole;

	int type = machine->ReadRegister(2);
	
	// Kien truc bat exception va syscall dang switch case.
	switch (which) 
	{
	case NoException:
		return;
	case SyscallException:
	{
		switch (type)
		{
			case SC_Halt:
				DEBUG('a', "\n Shutdown, initiated by user program.");
				printf("\n\n Shutdown, initiated by user program.");
				interrupt->Halt();
				break;
			case SC_CreateFile:
			{
				int name = machine->ReadRegister(4);
				char* filename = User2System(name, MAXFILELENGTH);

				if (filename == NULL) {
					DEBUG('f', "\nCreateFile: Not enough memory in system");
					machine->WriteRegister(2, -1);	// Khong du bo nho de tao
				}

				if (!fileSystem->Create(filename, 0)) {
					DEBUG('f', "\nCreateFile: Error create file");
					machine->WriteRegister(2, -1);	// Tao File that bai
				}

				DEBUG('f', "\nCreateFile: Create file successfully");
				machine->WriteRegister(2, 0);	// Tao File thanh cong
				delete[] filename;
				break;
			}
			case SC_OpenFileFunc:
			{
				
				int name = machine->ReadRegister(4);	// Ten cua File duoc Open
				int type = machine->ReadRegister(5);	// Mode su dung de mo (0 - ReadWrite, 1 - ReadOnly)
				char* filename = User2System(name, MAXFILELENGTH);
				
				// Quan ly File dc luu trong synchcons.cc
				gSynchConsole.SetFile(gSynchConsole.GetCurrentIndex(), fileSystem->Open(filename), type); // type duoc luu cho Mode
				
				// Kiem tra so luong file duoc mo
				if (gSynchConsole.GetCurrentIndex() > 10)
				{
					DEBUG('f',"\nOpenFile: So luong File da qua tai");
					delete[] filename;
					break;
				}
				if (strcmp(filename,"stdin") == 0)
				{
					machine->WriteRegister(2, 0);
					break;
				}
				if (strcmp(filename,"stdout") == 0)
				{
					machine->WriteRegister(2, 1);
					break;
				}
				
				// Cach doc theo mode ko phai 0 va 1
				if (type != 0 && type != 1) {
					DEBUG('f',"\nOpenFile: Truyen tham so sai");
					delete[] filename;
					break;
				}
				
				// Mo file
				if (gSynchConsole.GetFile(gSynchConsole.GetCurrentIndex()) != NULL) {	// Mo thanh cong
					DEBUG('f',"\nOpenFile: Open file successfully");
					machine->WriteRegister(2, gSynchConsole.GetCurrentIndex());
					gSynchConsole.IncreaseIndex();	// Tang vi tri trong quan ly file
				} else {							// Mo that bai
					DEBUG('f',"\nOpenFile: Error open file");
					machine->WriteRegister(2, -1);
				};
				delete[] filename;
				break;
			}
			case SC_CloseFile:
			{
				int id = machine->ReadRegister(4); // id cua File duoc dong
				DEBUG('f',"\nCloseFile: %d ------- %d", gSynchConsole.GetFile(id), id);
				if (gSynchConsole.GetFile(id) != NULL) {	// Neu co id ton tai
					delete gSynchConsole.GetFile(id);
					gSynchConsole.SetFile(id, NULL, NULL);
				} else {	// Neu khong thi se dung
					break;
				}
				DEBUG('f',"\nCloseFile: Close file successfully");
				break;
			}
			case SC_ReadFile:	// **Luu y: ham GetCurrentPos() duoc them vao tai openfile.h
			{
				int bufAddr = machine->ReadRegister(4);
				int charcount = machine->ReadRegister(5);
				int id = machine->ReadRegister(6);	
				int OldPos;	// De tinh so bytes doc that su
				int NewPos;
				
				char *buffer = new char[charcount];
				
				// Kiem tra Truong hop doc file bi loi
				if (id < 0 || id > 10)	// Vuot qua quan ly file
				{
					machine->WriteRegister(2, -1);
					break;
				}

				// Read data from console.
				if (id == 0 )
				{
					buffer = User2System(bufAddr, charcount);
					int sz = gSynchConsole.Read(buffer, charcount);
					if(sz != 0 )
					{
						System2User(bufAddr, sz, buffer);
						machine->WriteRegister(2, sz);
					}
					else 
					{
						machine->WriteRegister(2, -2);	// Truong hop da o cuoi file	
						DEBUG('f',"\nReadFile: Read file at EOF");
						
					}
					delete[] buffer;
					break;
				}

				if (gSynchConsole.GetFile(id) == NULL)	// Khong co file ton tai o vi tri do
				{
					machine->WriteRegister(2, -1);
					DEBUG('f', "Khong ton tai file tai vi tri %d", id);
					break;
				}
					
				OldPos = gSynchConsole.GetFile(id)->GetCurrentPos();
				buffer = User2System(bufAddr, charcount);
				DEBUG('f',"\nReadFile: Ready Read file successfully %s", buffer);
				
			
				if (gSynchConsole.GetFile(id)->Read(buffer, charcount) > 0)		// Doc noi dung cua file
				{
				  	NewPos = gSynchConsole.GetFile(id)->GetCurrentPos();	
					System2User(bufAddr, NewPos - OldPos +1, buffer);
					machine->WriteRegister(2, NewPos - OldPos + 1);		// Tra ve so ky tu doc that su	
					DEBUG('f',"\nReadFile: Read file successfully");
					delete[] buffer;
					break;
				}
				else
				{
					machine->WriteRegister(2, -2);	// Truong hop da o cuoi file	
					DEBUG('f',"\nReadFile: Read file at EOF");
					delete[] buffer;
					break;
				}
			}
			case SC_WriteFile:	// **Luu y: ham GetCurrentPos() duoc them vao tai openfile.h
			{
				int bufAddr = machine->ReadRegister(4);
				int charcount = machine->ReadRegister(5);
				int id =  machine->ReadRegister(6);
				int OldPos;	// De tinh so bytes ghi that su
				int NewPos;

				// Kiem tra Truong hop doc file bi loi
				if (id < 0 || id > 10)	// Vuot qua quan ly file
				{
					machine->WriteRegister(2, -1);
					break;
				}

				char *buffer = new char[charcount];

				// Write data to console
				if ( id == 1 )
				{
					if(gSynchConsole.GetFileType(id) == 0){
						int i = 0;
						buffer = User2System(bufAddr, charcount);
						while (buffer[i] != 0 && buffer[i] != '\n')
						{
							gSynchConsole.Write(buffer+i, 1);
							i++;
						}
						buffer[i] = '\n';
						gSynchConsole.Write(buffer+i,1);
						machine->WriteRegister(2, i-1);		// Ghi so byte thuc su.
						
						break;
					}
					else
					{
						DEBUG('f', "\nMode for write console error!");
						machine->WriteRegister(2, -1);	// Truong hop chi mo file de doc, ko cho ghi	
						DEBUG('f',"\nWriteFile: Write console failed");
						delete[] buffer;
					break;
					}
				}
				
				if (gSynchConsole.GetFile(id) == NULL)	// Khong co file ton tai o vi tri do
				{
					machine->WriteRegister(2, -1);
					break;
				}

					
				OldPos = gSynchConsole.GetFile(id)->GetCurrentPos();
				buffer = User2System(bufAddr, charcount);
				
						
				if (gSynchConsole.GetFileType(id) == 0)	// ReadWrite Mode
				{	
					if ((gSynchConsole.GetFile(id)->Write(buffer, charcount)) > 0) // Ghi noi dung cua file
					{
						NewPos = gSynchConsole.GetFile(id)->GetCurrentPos();
						machine->WriteRegister(2, NewPos - OldPos + 1);		// Tra ve so ky tu ghi that su
						DEBUG('f',"\nWriteFile: Write file successfully %s", buffer);
						delete[] buffer;
						break;
					}
					else
					{
						machine->WriteRegister(2, -2);	// Truong hop da o cuoi file	
						DEBUG('f',"\nWriteFile: Write file at EOF");
						delete[] buffer;
						break;
					}
				}
				else 
				{
					DEBUG('f', "\nMode for write file error!");
				}
				
				machine->WriteRegister(2, -1);	// Truong hop chi mo file de doc, ko cho ghi	
				DEBUG('f',"\nWriteFile: Write file failed");
				delete[] buffer;
				break;
				
			}
			case SC_SeekFile:	// **Luu y: ham Seek() duoc them vao tai openfile.h
			{
				int pos = machine->ReadRegister(4);
				int id = machine->ReadRegister(5);

				// Kiem tra Truong hop doc file bi loi
				if (id < 0 || id > 10)		// Vuot qua quan ly file
				{
					machine->WriteRegister(2, -1);
					break;
				}
				
				if(id == 0 || id == 1)
				{
					// Goi seek tren console.
					DEBUG('f', "Can not call seek syscall");
					machine->WriteRegister(2, -1);
					break;
				}
				
				if (gSynchConsole.GetFile(id) == NULL)	// Khong co file ton tai o vi tri do
				{
					machine->WriteRegister(2, -1);
					break;
				}

				// Ko tim duoc vi tri
				if (pos == -1) {	
					pos = gSynchConsole.GetFile(id)->Length();
				} 

				// Tra ve vi tri
				if (pos > gSynchConsole.GetFile(id)->Length() || pos < 0) {
					machine->WriteRegister(2, -1);
				} 
				else {
					gSynchConsole.GetFile(id)->Seek(pos);
					machine->WriteRegister(2, pos);
				}
				break;
			}
			case SC_PrintString:
			{
				int virtAddr = machine->ReadRegister(4);
				char* filename = User2System(virtAddr, MAXFILELENGTH);
				int i = 0;
				while (filename[i] != 0 && filename[i] != '\n')
				{
					gSynchConsole.Write(filename+i, 1);
					i++;
				}
				filename[i] = '\n';
				gSynchConsole.Write(filename+i,1);
				delete[] filename;
				break;
			}
			case SC_PrintChar:
			{
				char* onechar=new char[1];
				onechar[0]=(char)machine->ReadRegister(4);
				gSynchConsole.Write(onechar,1);
				delete onechar;
				break;
			}
			case SC_Exec:
			{
				int virAddr = machine->ReadRegister(4);

				char* name = User2System(virAddr, MAXFILELENGTH);
			
				if(name == NULL)
				{
					printf("\nError: Unable to open file");
					machine->WriteRegister(2, -1);
					delete name;
					break;
				}
				int result = pTab->ExecUpdate(name);
				machine->WriteRegister(2, result);
								
				delete name;
				break;
			}
			case SC_Join:
			{
				// ID cua tien trinh can Join.
				int id = machine->ReadRegister(4);
				int result = pTab->JoinUpdate(id);
				machine->WriteRegister(2, result);
				
				break;
			}
			case SC_Exit:
			{
				int exitStatus = machine->ReadRegister(4);
				int result = pTab->ExitUpdate(exitStatus);
				machine->WriteRegister(2, result);
				
				break;
			}
			case SC_CreateSemaphore:
			{
				int bufAddr = machine->ReadRegister(4);
				int semval = machine->ReadRegister(5);
				char* name = User2System(bufAddr, MAXFILELENGTH);
				int result = semTab->Create(name, semval);

				if(result==-1)
				{
					DEBUG('f',"\nCreate Semaphore failed");
					machine->WriteRegister(2, -1);
					break;
				}
				machine->WriteRegister(2, 0);
				break;
			}
			case SC_Up:
			{
				int bufAddr = machine->ReadRegister(4);
				char* name = User2System(bufAddr, MAXFILELENGTH);
				int result = semTab->Signal(name);

				if(result==-1)
				{
					DEBUG('f',"\nNot exist in STable!");
					machine->WriteRegister(2, -1);
					break;
				}
				machine->WriteRegister(2, 0);
				break;
				}
			case SC_Down:
			{
				int bufAddr = machine->ReadRegister(4);
				char* name = User2System(bufAddr, MAXFILELENGTH);
				int result = semTab->Wait(name);

				if(result==-1)
				{
					DEBUG('f',"Not exist in STable!");
					machine->WriteRegister(2, -1);
					break;
				}
				machine->WriteRegister(2, 0);
				break;
			}
		}
	// Advance program counters.
	machine->registers[PrevPCReg] = machine->registers[PCReg];	// for debugging, in case we
					// are jumping into lala-land
	machine->registers[PCReg] = machine->registers[NextPCReg];
	machine->registers[NextPCReg] += 4;
	break;
	}
	case PageFaultException:
		DEBUG('a', "\nNo valid translation found.");
		interrupt->Halt();
		break;
	case ReadOnlyException:   
		DEBUG('a', "\nWrite attempted to page marked read only.");
		interrupt->Halt();
		break;
	case BusErrorException:   
		DEBUG('a', "\nTranslation resulted in an invalid physical address.");
		interrupt->Halt();
		break;
	case AddressErrorException: 
		DEBUG('a', "\nUnaligned reference or one that was beyond the end of the address space.");
		interrupt->Halt();
		break;
	case OverflowException: 
		DEBUG('a', "\nInteger overflow in add or sub.");
		interrupt->Halt();
		break;
	case IllegalInstrException: 
		DEBUG('a', "\nUnimplemented or reserved instr.");
		interrupt->Halt();
		break;

	default:
		printf("\n Unexpected user mode exception (%d %d)", which, type);
		interrupt->Halt();
		break;
	}

	// Old exception struct.
	/*if ((which == SyscallException) && (type == SC_Halt)) {
	} else {
	printf("Unexpected user mode exception %d %d\n", which, type);
	ASSERT(FALSE);
	}*/
}




