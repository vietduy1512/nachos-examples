#include "sem.h"

Sem::Sem(char* na, int i)
{
	strcpy(this->name, na);
	sem = new Semaphore(this->name, i);
}
	
Sem::~Sem()
{
	delete sem;
}
	
void Sem::Wait()
{
	sem->P();
}
	
void Sem::Signal()
{
	sem->V();
}

char* Sem::GetName()
{
	return name;
}

