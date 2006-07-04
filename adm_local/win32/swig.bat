@rem echo off

%SWIGROOT%\swig -python  -c++ -I..\..\include -I%2 %1
