
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <windows.h>

    // LPCWSTR               lpApplicationName;         // Ім'я виконуваного файлу (може бути NULL)
    // LPWSTR                lpCommandLine;             // Командний рядок (може бути NULL)
    // LPSECURITY_ATTRIBUTES lpProcessAttributes;       // Атрибути безпеки процесу (може бути NULL)
    // LPSECURITY_ATTRIBUTES lpThreadAttributes;        // Атрибути безпеки потоку (може бути NULL)
    // BOOL                  bInheritHandles;           // Успадковувати дескриптори від батьківського процесу (TRUE/FALSE)
    // DWORD                 dwCreationFlags;           // Прапори створення (наприклад, CREATE_NEW_CONSOLE)
    // LPVOID                lpEnvironment;             // Середовище для нового процесу (NULL для успадкування)
    // LPCWSTR               lpCurrentDirectory;        // Текуща робоча директорія (NULL для успадкування)
    // LPSTARTUPINFOW        lpStartupInfo;             // Структура з інформацією про початковий стан вікна
    // LPPROCESS_INFORMATION lpProcessInformation;      // Структура для отримання інформації про новий процес

    // CreateProcessW(
    //     lpApplicationName,
    //     lpCommandLine,
    //     lpProcessAttributes,
    //     lpThreadAttributes,
    //     bInheritHandles,
    //     dwCreationFlags,
    //     lpEnvironment,
    //     lpCurrentDirectory,
    //     lpStartupInfo,
    //     lpProcessInformation);

const int NUM_PROCESSES(5); 
int finishedProcesses(0); 



void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void clearArea(int x, int y, int width, int height) {

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

void tabulate(int processId) {
    std::this_thread::sleep_for(std::chrono::seconds(processId)); // Приклад обчислення
    gotoxy(0, 21+processId);
    std::cout << "Process " << processId << " finished tabulation." << std::endl;
    gotoxy(0, 0);
}

void controlThread() 
{
    int a;
    std::cin >> a;
    switch(a)
    {
    case 1:
        std::cout << "wariant\nwas choosen:\n1" << std::endl;
        std::cin.ignore();
        break;
    case 2:
        std::cout << "wariant\nwas choosen:\n2" << std::endl;
        std::cin.ignore();
        break;
    }
    
    clearArea(0, 0, 30, 9);
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
?MXT@Wx.~    :     ~"##*$$$$M~)";

    gotoxy(0, 27);
    std::cout << "All processes have finished. Control thread is exiting." << std::endl;
    gotoxy(0, 29);
}

int main() {
    std::vector<std::thread> processes;
    std::thread ctrlThread(controlThread);
    
    auto start = std::chrono::high_resolution_clock::now();
    // Створюємо та запускаємо процеси
    for (int i = 0; i < NUM_PROCESSES; ++i) 
    {
        processes.emplace_back(tabulate, i + 1);
    }
    
    // Чекаємо на завершення всіх процесів
    for (auto& process : processes) 
    {
        process.join();
    }
    auto end = std::chrono::high_resolution_clock::now();
    gotoxy(0, 20+5+3);
    std::cout << "total time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()/1000000.0;

    if(ctrlThread.joinable()){
        gotoxy(0,0);
    }else{
        std::cout << std::endl;
    }

    ctrlThread.join();
    getchar();

    return 0;
}