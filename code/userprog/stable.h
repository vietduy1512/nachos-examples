#ifndef STATE_H
#define STATE_H

#include "synch.h"
#include "bitmap.h"
#include "sem.h"

#define MAX_SEMAPHORE 10 

class STable
{
private:
	BitMap* bm;	// Quan ly slot trong.
	Sem* semTab[MAX_SEMAPHORE];	// Quan ly toi da 10 doi tuong Sem.

public:
	// Khoi tao size doi tuong Sem de quan ly 10 Semaphore. Gan gia tri ban dau la NULL.
	// Nho khoi tao bm de su dung.
	STable();
	
	~STable();	// Huy cac doi tuong da tao.
	
	int Create(char* name, int init);	// Kiem tra Semaphore "name", neu chua ton tai thi tao Semaphore moi, nguoc lai bao loi. 

	int Wait(char* name); // Neu ton tai Semaphore "name" thi goi this->P() de thuc thi, nguoc lai thi bao loi.

	int Signal(char* name);	// Neu ton tai Semaphore "name" thi goi this->V() de thuc thi, nguoc lai thi bao loi.

	int FindFreeSlot(int id);	// Tim slot trong.

};


#endif

