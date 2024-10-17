#ifndef LAB_UTILITIES
#define LAB_UTILITIES

#include <windows.h>
#include <vector>
#include <map>
#include <mutex>

PROCESS_INFORMATION create_child(LPWSTR lpCommandLine);
void controls(std::vector<PROCESS_INFORMATION> &pi_vector, std::mutex &mtx);
void MonitorProcesses(std::vector<std::pair<int, HANDLE>> pIdHandlsPairs, std::mutex &mtx);
void gotoxy(int x, int y);
void gotoxy(COORD coord);
COORD GetCursorPosition();
void clearArea(int x, int y, int width, int height); 

#endif