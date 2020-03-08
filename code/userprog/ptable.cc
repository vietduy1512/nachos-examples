#include "copyright.h"
#include "syscall.h"
#include "synchcons.h"

#include "system.h"
#include "ptable.h"

	/*BitMap		*bm;	// Danh dau cac vi tri da duoc su dung trong pcb.
	PCB		*pcb[MAXPROCESS];	// Mot bang mo ta tien trinh.
	int		psize;
	Semaphore	*bmsem;		//Dung de ngan chan truong hop nap 2 tien trinh cung luc*/

//Khoi tao size doi tuong pcb de luu size process. Gan gia tri ban dau la null. Nho khoi tao *bm va *bmsem de su dung
PTable::PTable(int size)
{
	psize = size;
	if(size > MAXPROCESS)
	{
		printf("Unable to open new file");
		return;
	}
	for(int i = 0; i< MAXPROCESS; i++)
	{
		pcb[i] = NULL;
	}
	bm = new BitMap(size);
	bm->Mark(0);
	bmsem = new Semaphore("bmsem", 1);
	pcb[0] = new PCB(0);	// Tien trinh cha.
	pcb[0]->parentID = -1;
}

//Huy doi tuong da tao
PTable::~PTable()
{
	if(bm != NULL)
		delete bm;
	if(bmsem != NULL)
		delete bmsem;
	for(int i = 0; i< psize; i++)
	{
		if(bm->Test(i))
		{
			delete pcb[i];
			pcb[i] = NULL;
		}
	}
	delete pcb;
	psize = 0;
}	

//return PID. Xu ly cho system call SC_Exit.
int PTable::ExecUpdate(char* name)
{

	bmsem->P();	// Truy xuat doc quyen, dung de ngan chan nap 2 tien trinh cung 1 luc.

	//Kiem tra name co bang NULL khong.
	if(name == NULL)
	{
		printf("\nInvalid program name");
		bmsem->V();
		return -1;
	}

	// Kiem tra su ton tai cua chuong trinh "name".
	/*OpenFile* bcheckName = fileSystem->Open(name);
	if(bcheckName == NULL)
	{
		printf("\nProgram name does not exist");
		bmsem->V();
		return -1;
	}*/
	// Kiem tra xem chuong trinh co dang goi chinh no khong.
	if(strcmp(name, currentThread->getName()) == 0)
	{
		printf("\nProgram name is running");
		bmsem->V();
		return -1;
	}
	// Tim slot trong ma chua co tien trinh nao sai.
	int freeSlot = GetFreeSlot();
	if(freeSlot < 0)
	{
		printf("\nHet slot trong");
		bmsem->V();
		return -1;
	}
	pcb[freeSlot] = new PCB(freeSlot);
	pcb[freeSlot]->SetFileName(name);
	int result = pcb[freeSlot]->Exec(name, freeSlot);
	
	bmsem->V();
	return 0;
}

//Xu ly cho system call SC_Exit.
int PTable::ExitUpdate(int ec)
{
	int curID = currentThread->processID;
	if(curID  == 0)
	{
		// Main.
		printf("CurID = 0");
		interrupt->Halt();
		return -1;
	}
	if(IsExist(curID) == false)
	{
		printf("\nID does not exist");
		return -1;
	}
	//printf("CurID : %d", curID);
	pcb[curID]->SetExitCode(ec);
	pcb[pcb[curID]->parentID]->DecNumWait();//Giam so tien trinh cho cua tien trinh cha di 1
	pcb[curID]->JoinRelease();//Bao tien trinh cha co the thuc hien tiep
	pcb[curID]->ExitWait();

	Remove(curID);
	return ec;
}	

//Xu ly cho system call SC_Join.
int PTable::JoinUpdate(int id)
{
	if(id < 0)
	{
		printf("\nInvalid ID: ID < 0");
		return -1;
	}
	
	// Tien trinh khong the join vao chinh ban than no.
	if(pcb[id]->GetID() == currentThread->processID)
	{
		printf("Invalid ID: Khong the join vao chinh no");
		return -1;
	}
	// TIen trinh phai join dung vao tien trinh cha cua no.
	if(pcb[id]->parentID != currentThread->processID)
	{
		printf("\nInvalid ID: Join khong dung cha");
		return -1;
	}
	// Tang so luong tien trinh cho cua tien trinh cha.
	pcb[pcb[id]->parentID]->IncNumWait();
	pcb[id]->JoinWait();
	
	int eCode = pcb[id]->GetExitCode();
	
	pcb[id]->ExitRelease();	// Cho tien trinh con ket thuc.
	pcb[id]->boolBG = 0;
	
	return eCode;
	
}	

//Tim slot trong de luu thong tin cho tien trinh moi
int PTable::GetFreeSlot()
{
	return bm->Find();
	
}

//Kiem tra co ton tai process ID nay khong
bool PTable::IsExist(int pID)
{
	return bm->Test(pID);
}

//Xoa mot processID ra khoi mang quan ly no, khi ma tien trinh nay da ket thuc
void PTable::Remove(int pID)
{
	bm->Clear(pID);
	if(pcb[pID]!=NULL)
	{
		delete pcb[pID];
		pcb[pID]=NULL;
	}
}
//Lay ten cua tien trinh co processID la pID
char* PTable::GetName(int pID)
{
	return pcb[pID]->GetNameThread();
}

