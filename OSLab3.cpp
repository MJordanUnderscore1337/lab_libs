#include <iostream>
#include <vector>
#include <windows.h>
#include <string>
#include <thread>
#include <filesystem>
#include <map>
#include <mutex>

#include "labUtilities.h"


int main(int argc, char* argv[])
{
    system("cls");
    HWND hConsole = GetConsoleWindow();
    MoveWindow(hConsole, 50, 50, 1280, 950, TRUE);

    auto start = std::chrono::high_resolution_clock::now();
    std::mutex mtx;   
    if(false){return 2;}

    double A = std::atof(argv[1]); 
    double B = std::atof(argv[2]); 
    int steps = std::atoi(argv[3]); 
    int processes = std::atoi(argv[4]); 
    char mode = *argv[5]; //s - slow; x - extra slow;

    if(B - A <= 0 | A <= 0 | B <= 0){return 3;}

    double step_size = (B - A) / steps;
    double start_ofset = (B - A)/ processes; 
        
    //processes creation
    std::vector<PROCESS_INFORMATION> pi_vector;
    std::vector<std::pair<int, HANDLE>> pIdHandlsPairs;
    std::wstring output_folder(L"output");

    if(!std::filesystem::exists(output_folder))
    {
        std::filesystem::create_directories(output_folder);
    }

    for (const auto& entry : std::filesystem::directory_iterator(output_folder)) 
    {
        std::filesystem::remove(entry);
    }

    for(int processID = 0; processID < processes; ++processID)
    {

        double start = A + start_ofset * processID;
        double end = A + start_ofset * (processID+1);

        std::wstring cmdArgs = L"..\\child\\child.exe " + 
                               std::to_wstring(start)             + L" " + 
                               std::to_wstring(end)               + L" " + 
                               std::to_wstring(step_size)         + L" " + 
                               std::to_wstring(processID)         + L" " +
                               std::to_wstring(mode)              + L" " +
                               output_folder;  
        
        wchar_t* cmdLine = &cmdArgs[0];  
        
        PROCESS_INFORMATION pi = create_child(cmdLine);

        pi_vector.push_back(pi);
        pIdHandlsPairs.push_back(std::make_pair(processID, pi.hProcess)) ; 
    }



    //виклик окремого потоку для опцій керування процесами
    std::thread control_thread(controls, std::ref(pi_vector), std::ref(mtx));
    std::thread status_thread(MonitorProcesses, std::ref(pIdHandlsPairs), std::ref(mtx));

    status_thread.join();
    auto end = std::chrono::high_resolution_clock::now();
    COORD prev_pos = GetCursorPosition();
    gotoxy(0, 25+processes+2);
    /*PLACEHOLDER FOR END TIME MANIPULATION!!!!!!!!!!!!!!!!!*/
    std::cout << "total time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()/1000000.0 << " seconds";
    gotoxy(prev_pos);
    control_thread.join();

    for(int i = 0; i < processes; i++)
    {
        CloseHandle(pi_vector[i].hThread);
        CloseHandle(pi_vector[i].hProcess);
    }
    system("pause");
    system("cls");
}