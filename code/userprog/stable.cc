#include "stable.h"
#include "system.h"

STable::STable()
{
	for(int i = 0; i < MAX_SEMAPHORE; i++)
	{
		semTab[i] = NULL;
	}
	bm = new BitMap(MAX_SEMAPHORE);
}

STable::~STable()
{
	for(int i = 0; i < MAX_SEMAPHORE; i++)
	{	
		if(semTab[i] != NULL)
			delete semTab[i];
	}
	delete bm;
}

int STable::Create(char* name, int init)
{
	for(int i = 0; i < MAX_SEMAPHORE; i++)
	{
		if(semTab[i] != NULL)
		{
			if(strcmp(name, semTab[i]->GetName()) == 0)
			{
				printf("\nSemaphore existed");
				return -1;
			}
		}
	}
	if(init < 0)
	{
		printf("\nsemaphore value >= 0");
		return -1;		
	}
	int id = this->bm->Find();
	if(id > 0)
	{
		semTab[id] = new Sem(name, init);
		return 0;
	}
	return -1;
}

int STable::Wait(char* name)
{
	for(int i = 0; i < MAX_SEMAPHORE; i++)
	{
		if(semTab[i] != NULL) // Kiem tra neu semaphore ton tai
		{
			char* fname = semTab[i]->GetName();
			if(strcmp(fname, name) == 0) // Dung semaphore
			{
				semTab[i]->Wait();
				return 0;
			}
		}
	}
	return -1;
}

int STable::Signal(char* name)
{
	for(int i = 0; i < MAX_SEMAPHORE; i++)
	{
		if(semTab[i] != NULL) // Kiem tra neu semaphore ton tai
		{
			char* fname = semTab[i]->GetName();
			if(strcmp(fname, name) == 0)//Dung semaphore
			{
				semTab[i]->Signal();
				return 0;
			}
		}
	}
	return -1;
}

int STable::FindFreeSlot(int id)
{
	return this->bm->Find();
}

