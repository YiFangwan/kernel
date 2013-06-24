
# PTHREAD detection for Salome on Windows

if(WINDOWS)
  set(PTHREAD_ROOT_DIR $ENV{PTHREAD_ROOT_DIR} CACHE PATH "Path to Pthread directory")  
  
  if(EXISTS ${PTHREAD_ROOT_DIR})    
    set(CMAKE_INCLUDE_PATH ${PTHREAD_ROOT_DIR}/include)
    set(CMAKE_LIBRARY_PATH ${PTHREAD_ROOT_DIR}/lib)    
  endif(EXISTS ${PTHREAD_ROOT_DIR})
  
  find_package(pthread REQUIRED)  
endif(WINDOWS)