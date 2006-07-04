@set PYTHONPATH=%OMNILIB%\..\python;
%OMNIROOT%\bin\x86_win32\omniidl.exe -I../../idl -bcxx -C../../stub %1

move %~d0%~p0..\..\stub\%2SK.cc %~d0%~p0..\..\stub\%2SK.cxx
