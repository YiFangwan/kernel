#include "SALOME_WNT.hxx"

#include <sys/stat.h>
#include <windows.h>

int access(const char* theFileName, int theMode) {
	if(theMode == 0) {
		// file existence checking
		struct stat aStat;
		return stat(theFileName, &aStat);
	}
	return -1;
}


// Shared objects

void* dlsym( void* handle, const char* name )
{
#ifndef WIN32
    return dlsym( handle, name );
#else
    return ::GetProcAddress( ( HINSTANCE ) handle, name );
#endif
}

void* dlopen( const char* name, int mode )
{
#ifndef WIN32
  return dlopen( name, mode );
#else
  return ::LoadLibrary( name );
#endif
}

int dlclose( void* handle )
{
#ifndef WIN32
  return dlclose( handle );
#else
  return !FreeLibrary( ( HMODULE )handle );
#endif
}

char *dlerror()
{
#ifndef WIN32
  return dlerror();
#else
  return NULL;
#endif
}
