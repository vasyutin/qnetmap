#ifndef _MEM_DEBUG_H
#define _MEM_DEBUG_H

#ifdef _DEBUG

void* operator new(unsigned int size, const char *filename, int line);
void operator delete(void *ptr);

#define QNETMAP_DEBUG_NEW new(__FILE__, __LINE__)
#define new QNETMAP_DEBUG_NEW
#endif

#endif