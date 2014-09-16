CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8 FATAL_ERROR)

FILE ( COPY src/salomeContext.py src/salomeContextUtils.py.in src/parseConfigFile.py  DESTINATION bin/salome )
