#ifdef _DEBUG

#include <malloc.h>

void* operator new(unsigned int size, const char* filename, int line)
{
   void *ptr = (void*)malloc(size);
   //AddTrack((DWORD)ptr, size, filename, line);
   return(ptr);
}

void operator delete(void* ptr)
{
   //RemoveTrack((DWORD)ptr);
   free(ptr);
}

#endif