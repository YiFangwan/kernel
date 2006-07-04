#ifndef SALOME_WNT_HEADERFILE
#define SALOME_WNT_HEADERFILE

#include <sys/stat.h>
#include <windows.h>
#include <process.h>
//#include <pthread.h>

#if defined SALOME_WNT_EXPORTS
#if defined WIN32
#define SALOME_WNT_EXPORT __declspec( dllexport )
#else
#define SALOME_WNT_EXPORT
#endif
#else
#if defined WNT
#define SALOME_WNT_EXPORT __declspec( dllimport )
#else
#define SALOME_WNT_EXPORT
#endif
#endif

template<class TContainer, class TIterator>
void InsertInto(TContainer& theContainer, const TIterator& theFirst, const TIterator& theLast)
{
  for(TIterator anIter = theFirst; anIter != theLast; anIter++)
	theContainer.insert(*anIter);
}

//SALOME_WNT_EXPORT int access(const char* theFileName, int theMode);


SALOME_WNT_EXPORT void* dlsym( void*, const char* );
SALOME_WNT_EXPORT void* dlopen( const char*, int );
SALOME_WNT_EXPORT int   dlclose( void* );  // 0 if success
SALOME_WNT_EXPORT char *dlerror();


#endif