#ifndef PCB_H
#define PCB_H

#include "thread.h"
#include "synch.h"

#define MAX_FILE_NAME 255

class PCB
{
private:
	Semaphore	*joinsem;	//semaphore cho qua trinh join
	Semaphore	*exitsem;	//semaphore cho qua trinh exit
	Semaphore	*mutex;		//semaphore cho qua trinh truy xuat doc quyen.
	char FileName[MAX_FILE_NAME];	// Ten cua chuong trinh dang duoc thuc thi.
	int		exitcode;
	Thread		*thread;
	int		pid;
	int		numwait;	//so tien trinh da join

public:
	int 		parentID;	//ID cua tien trinh cha
	int		JoinStatus;	//Trang thai co Join voi tien trinh nao khong? neu co thi gia tri chinh la ID cua tien trinh ma no Join
	char boolBG;	// Chay background
	
	PCB();
	PCB(int id);
	~PCB();
	int Exec(char *filename, int pID); // Tao 1 thread moi co ten la filename va pid laf pID
	
	int GetID();	// Tra ve processID cua tien trinh goi thuc hien.
	int GetNumWait();	// Tra ve so tien trinh cho.
	
	void JoinWait();	//1. Tien trinh cha doi tien trinh con ket thuc.
	void ExitWait();	//4. Tien trinh con ket thuc.
	
	void JoinRelease();	//2. Bao cho tien trinh cha thuc thi tiep.
	void ExitRelease();	//3. Cho phep tien trinh con ket thuc.
	
	void IncNumWait();	// Tang so tien trinh cho.
	void DecNumWait();	// Giam so tien trinh cho.
	
	void SetExitCode(int ec);	// Dat exitcode cua tien trinh.
	int GetExitCode();	// Tra ve exitcode.
	
	void SetFileName(char* fn);	// Dat tien cua tien trinh.
	char* GetNameThread();	// Tra ve ten cua tien trinh.
};

//*********************************************************************************************************************
void StartProcess(int pID);

#endif
