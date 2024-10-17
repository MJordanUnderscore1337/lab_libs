#include "labUtilities.h"
#include <iostream>
#include <map>
#include <mutex>

#define ERROR_MARGIN 50

void gotoxy(int x, int y) 
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void gotoxy(COORD coord) 
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void clearArea(int x, int y, int width, int height) 
{

    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    
    for (int i = 0; i < height; ++i) 
    {
        for (int j = 0; j < width; ++j) 
        {
            std::cout << ' '; 
        }
        std::cout << '\n'; 
    }
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

COORD GetCursorPosition() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    return csbi.dwCursorPosition;
}


void controls(std::vector<PROCESS_INFORMATION> &pi_vector, std::mutex &mtx)
{   
    COORD prev_pos;
    int key_code = 10;
    int n = 0;
    int error_count = 0;
    HANDLE hProcess;
    DWORD priorityClass[6] = {IDLE_PRIORITY_CLASS, 
                              BELOW_NORMAL_PRIORITY_CLASS, 
                              NORMAL_PRIORITY_CLASS, 
                              ABOVE_NORMAL_PRIORITY_CLASS, 
                              HIGH_PRIORITY_CLASS, 
                              REALTIME_PRIORITY_CLASS};
    while(key_code)
    {
        mtx.lock();
        clearArea(0, 0, ERROR_MARGIN-1, 22);
        gotoxy(0, 0);
        std::cout << R"(Options:
1-> kill process
2-> Renice process
3-> Suspend process
4-> Resume process
0-> exit
)";     
        mtx.unlock();
        std::cin >> key_code;

        switch(key_code)
        {
            //kill
            case(1): 
                mtx.lock();
                std::cout << std::endl << "Enter process ID from 1 to " << pi_vector.size() << ": " <<  std::endl;
                mtx.unlock();
                std::cin >> n; 
                hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pi_vector[n-1].dwProcessId);
                 if (hProcess == NULL) 
                {
                    prev_pos = GetCursorPosition();
                    gotoxy(ERROR_MARGIN, error_count);
                    std::cerr << "error: can not to open the proces" << GetLastError() << std::endl;
                    gotoxy(prev_pos);
                    error_count++;
                }
                // Завершуємо процес
                if (!TerminateProcess(hProcess, 1)) 
                {
                    prev_pos = GetCursorPosition();
                    gotoxy(ERROR_MARGIN, error_count);
                    std::cerr << "error: can not to kill the process" << GetLastError() << std::endl;
                    gotoxy(prev_pos);
                    error_count++;
                }else 
                {
                    mtx.lock();
                    gotoxy(0, 25+n-1+pi_vector.size()+5);
                    std::cout << "process with id " << n << " was killed" << std::endl;
                    mtx.unlock();
                }
            break;
            //nice-renice
            case(2): 
                mtx.lock();
                std::cout << "choose the process from 1 to " << pi_vector.size() << ": "; 
                mtx.unlock();
                std::cin >> n;
                mtx.lock();
                std::cout << std::endl << R"(choose priority to set up)" << std::endl;
                std::cout << 
R"(options:
[1] - IDLE
[2] - BELOW_NORMAL
[3] - NORMAL
[4] - ABOVE_NORMAL
[5] - HIGH_PRIORITY
[6] - REALTIME       ya nenavydju windows X_X
)"              << std::endl;
                mtx.unlock();
                
                std::cin >> key_code; 
                // Відкриваємо процес з правами зміни пріоритету
                hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_SET_INFORMATION, FALSE, pi_vector[n-1].dwProcessId);
                if (hProcess == NULL) 
                {
                    gotoxy(ERROR_MARGIN, error_count);
                    std::cerr << "error: can not ot open the process" << GetLastError() << std::endl;
                    error_count++;
                }

                // Встановлюємо пріоритет
                if (!SetPriorityClass(hProcess, priorityClass[key_code-1])) 
                {
                    prev_pos = GetCursorPosition();
                    gotoxy(ERROR_MARGIN, error_count);
                    std::cerr << "error: can not set priority " << GetLastError() << std::endl;
                    gotoxy(prev_pos);
                    error_count++;
                } else 
                {
                    mtx.lock();
                    gotoxy(0, 25+n-1+pi_vector.size()+5);
                    std::cout << "process with ID " << n << " priority was changed to " << GetPriorityClass(hProcess) << std::endl;
                    mtx.unlock();
                }
                
            break;

            case (3): 
                mtx.lock();
                std::cout << "choose the process from 1 to " << pi_vector.size() << " to suspend: "; 
                mtx.unlock();
                std::cin >> n;
                
                hProcess = OpenProcess(THREAD_SUSPEND_RESUME, FALSE, pi_vector[n-1].dwProcessId);
                if (hProcess == NULL) 
                {
                    gotoxy(ERROR_MARGIN, error_count);
                    std::cerr << "error: can not ot open the process " << GetLastError() << std::endl;
                    error_count++;
                }
                if(SuspendThread(pi_vector[n-1].hThread))
                {
                    prev_pos = GetCursorPosition();
                    gotoxy(ERROR_MARGIN, error_count);
                    std::cout << "error: can not suspend thread " << GetLastError() << std::endl;
                    gotoxy(prev_pos);
                    error_count++;
                } else
                {
                    mtx.lock();
                    gotoxy(0, 25+n-1+pi_vector.size()+5);
                    std::cout << "process with ID " << n << " suspended" << std::endl;
                    mtx.unlock();
                }
            break;
            //release
            case(4):
                mtx.lock();
                std::cout << "choose the process from 1 to " << pi_vector.size() << " to resume: "; 
                mtx.unlock();
                std::cin >> n;
                
                hProcess = OpenProcess(THREAD_SUSPEND_RESUME, FALSE, pi_vector[n-1].dwProcessId);
                if (hProcess == NULL) 
                {
                    gotoxy(ERROR_MARGIN, error_count);
                    std::cerr << "error: can not ot open the process" << GetLastError() << std::endl;
                    error_count++;
                }
                if(!ResumeThread(pi_vector[n-1].hThread))
                {
                    prev_pos = GetCursorPosition();
                    gotoxy(ERROR_MARGIN, error_count);
                    std::cerr << "error: can not resume thread  " << GetLastError() << std::endl;
                    gotoxy(prev_pos);
                    error_count++;
                } else
                {
                    mtx.lock();
                    gotoxy(0, 25+n-1+pi_vector.size()+5);
                    std::cout << "process with ID " << n << " resumed " << std::endl;
                    mtx.unlock();
                } 
                ;
            break;
            case(0):
                clearArea(0, 0, ERROR_MARGIN-1, 22);
                gotoxy(0, 0);
                std::cout << R"(
                      :::!~!!!!!:.
                  .xUHWH!! !!?M88WHX:.
                .X*#M@$!!  !X!M$$$$$$WWx:.
               :!!!!!!?H! :!$!$$$$$$$$$$8X:
              !!~  ~:~!! :~!$!#$$$$$$$$$$8X:
             :!~::!H!<   ~.U$X!?R$$$$$$$$MM!
             ~!~!!!!~~ .:XW$$$U!!?$M8L8TXMM!
               !:~~~ .:!M"T#$$$$WX??#MRRMMM!
               ~?WuxiW*`   `"#$$$$8!!!!??!!!
             :X- M$$$$       `"T#$T~!8$WUXU~
            :%`  ~#$$$m:        ~!~ ?$$$$$$
          :!`.-   ~T$$$$8xx.  .xWW- ~""##*"
.....   -~~:<` !    ~?T#$$@@W@*?$$      /`
W$@@M!!! .!~~ !!     .:XUW$W!~ `"~:    :
#"~~`.:x%`!!  !H:   !WM$$$$Ti.: .!WUn+!`
:::~:!!`:X~ .: ?H.!u "$$$B$$$!W:U!T$$M~
.~~   :X@!.-~   ?@WTWo("*$$$W$TH$! `
Wi.~!X$?!-~    : ?$$$B$Wu("**$RM!
$R@i.~~ !     :   ~$$$$$B$$en:``
?MXT@Wx.~    :     ~"##*$$$$M~
)";
    gotoxy(0, 0);
            break;
        }
    }

}


PROCESS_INFORMATION create_child(LPWSTR lpCommandLine)
{
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if(!CreateProcessW(NULL, 
                       lpCommandLine,
                       NULL,
                       NULL,
                       FALSE,
                       0,
                       NULL,
                       NULL,
                       &si,
                       &pi))
    {
        std::cerr << "failed to create process: " << lpCommandLine << std::endl;
    }
    return pi;
}

void MonitorProcesses(std::vector<std::pair<int, HANDLE>> pIdHandlsPairs, std::mutex &mtx)
{
    // Масив дескрипторів процесів
    int procesess_count = pIdHandlsPairs.size();
    int processes_count1 = procesess_count;
    std::vector<HANDLE> pHandles;
    for(int i = 0; i < procesess_count; ++i)
    {
        pHandles.push_back(pIdHandlsPairs[i].second);
    }
    HANDLE* handles = pHandles.data();
    
    
    // Очікуємо завершення всіх процесів
    while (!pHandles.empty()) 
    {
        DWORD result = WaitForMultipleObjects(
            pHandles.size(),        // Кількість процесів
            pHandles.data(),        // Масив дескрипторів
            FALSE,                  // Чекаємо завершення тільки одного процесу
            INFINITE                // Не обмежуємо час очікування
        );

        if (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + procesess_count) 
        {
            // Отримуємо індекс завершеного процесу
            int finishedIndex = result - WAIT_OBJECT_0;
            DWORD exitCode;
            GetExitCodeProcess(pHandles[finishedIndex], &exitCode);
            int realIndex = pIdHandlsPairs[finishedIndex].first;
            mtx.lock();
            COORD prev_pos = GetCursorPosition();
            gotoxy(0, 25+realIndex);
            std::cout << "Process " << realIndex + 1 << " has finished with exit code " << exitCode << std::endl;
            gotoxy(prev_pos);
            mtx.unlock();
            // Закриваємо дескриптор завершеного процесу
            CloseHandle(pHandles[finishedIndex]);

            // Видаляємо дескриптор із вектора
            pHandles.erase(pHandles.begin() + finishedIndex);
            pIdHandlsPairs.erase(pIdHandlsPairs.begin() + finishedIndex);
            // Оновлюємо масив дескрипторів, якщо є інші процеси
            procesess_count--;
            
        } else {
            std::cerr << "Error waiting for processes." << std::endl;
            break;
        }
    }
    COORD prev_pos = GetCursorPosition();
    gotoxy(0, 25+ processes_count1 +1);
    std::cout << "All processes have finished." << std::endl;
    gotoxy(prev_pos);
}