#ifndef SEM_H
#define SEM_H

#include "synch.h"

class Sem
{
public:
	// Khoi tao doi tuong Sem, gan gia tri ban dau la NULL.
	// Nho khoi tao bm de su dung.
	Sem(char* na, int i);
	~Sem();	// Huy cac doi tuong da tao.
	
	void Wait();	// Thuc hien thao tac cho.
	
	void Signal();	// Thuc hien thao tac giai phong Semaphore.
	
	char* GetName();	// Tra ve ten cua Semaphore.
	
private:
	char name[50];
	Semaphore *sem; // Tao Semaphore de quan ly.
};


#endif

