
#include "system.h"
#include "pcb.h"
	
	
extern void StartProcess(int pID);

/*Semaphore	*joinsem;	//semaphore cho qua trinh join
	Semaphore	*exitsem;	//semaphore cho qua trinh exit
	Semaphore	*mutex;		//semaphore cho qua trinh truy xuat doc quyen.
	int		exitcode;
	Thread		*thread;
	int		pid;
	int		numwait;	//so tien trinh da join
	int 		parentID;	//ID cua tien trinh cha
	int		JoinStatus;	//Trang thai co Join voi tien trinh nao khong? neu co thi gia tri chinh la ID cua tien trinh ma no Join*/

PCB::PCB()
{
	pid = 0;
	parentID = -1;
	numwait = 0;
	exitcode = 0;
	thread = NULL;
	joinsem = new Semaphore("joinsem", 0);
	exitsem = new Semaphore("exitsem", 0);
	mutex = new Semaphore("mutex", 1);
	JoinStatus = 0;
	boolBG = 0;
}
	
PCB::PCB(int id)
{
	pid = id;
	parentID = -1;
	numwait = 0;
	exitcode = 0;
	thread = NULL;
	joinsem = new Semaphore("joinsem", 0);
	exitsem = new Semaphore("exitsem", 0);
	mutex = new Semaphore("mutex", 1);
	JoinStatus = 0;
	boolBG = 1;
}

PCB::~PCB()
{
	if(joinsem != NULL)
		delete joinsem;
	if(exitsem != NULL)
		delete exitsem;
	if(mutex != NULL)
		delete mutex;
	if(thread != NULL)
	{
		thread->FreeSpace();
		if(pid == 0)
			interrupt->Halt();
		if(boolBG == 0)
			thread->Finish();
	}
}

// Tao 1 thread moi co ten la filename va pid laf pID
int PCB::Exec(char *filename, int pID)
{
	mutex->P();	// Truy xuat doc quyen, de tranh tinh trang 2 tien trinh cung nap 1 luc.

	thread = new Thread(filename);	// Tao tien trinh.

	if(thread == NULL)
	{
		printf("\nKhong du bo nho de tao tien trinh!");
		mutex->V();
		return -1;
	}
	thread->processID = pID;
	parentID = currentThread->processID;
	thread->Fork(StartProcess, pID);
	
	mutex->V();
	return pID;
	
}

// Tra ve processID cua tien trinh goi thuc hien.
int PCB::GetID()
{
	return pid;
}

// Tra ve so tien trinh cho.
int PCB::GetNumWait()
{
	return numwait;
}

//1. Tien trinh cha doi tien trinh con ket thuc.
void PCB::JoinWait()
{
	joinsem->P();	// Chuyen sang trang thai block va dung lai cho JoinRelease de thuc hien tiep.
}

//4. Tien trinh con ket thuc.
void PCB::ExitWait()
{
	exitsem->V();
}

//2. Bao cho tien trinh cha thuc thi tiep.
void PCB::JoinRelease()
{
	joinsem->V();
}
	//3. Cho phep tien trinh con ket thuc.
void PCB::ExitRelease()
{
	exitsem->V();	// Giai phong tien trinh dang cho.
}

// Tang so tien trinh cho.
void PCB::IncNumWait()
{
	numwait++;
}

// Giam so tien trinh cho.
void PCB::DecNumWait()
{
	numwait--;	
}

// Dat exitcode cua tien trinh.
void PCB::SetExitCode(int ec)
{
	exitcode = ec;
}

// Tra ve exitcode.
int PCB::GetExitCode()
{
	return exitcode;
}	

// Dat tien cua tien trinh.
void PCB::SetFileName(char* fn)
{
	strcpy(FileName, fn);
}

// Tra ve ten cua tien trinh.
char* PCB::GetNameThread()
{
	return FileName;
}



