@echo off
cls
make clean
make
bin\inmemorydb_test.exe %*
pause