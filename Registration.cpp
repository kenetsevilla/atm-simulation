#include "Registration.h"

#include <AccountManager.h>

#include <iostream>
///#include <cctype>
#include <string>
#include <conio.h>
#include <thread>
#include <fstream>
#include <windows.h>
#include <sstream>

using namespace std;

Registration::Registration()
{
    retrieve_file();
    areFilesInitialized = initialize_files();
    if(areFilesInitialized) {
        play_animation(2);
    }
}

/// Helper methods
bool Registration::isFull() {
    if(accList.last == MAX) {
        return true;
    }
    return false;
}

void Registration::date_formatter(int& i, int birthday[]) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD currentCursorPosition;
    currentCursorPosition.Y = 18;

    switch(i) {
        /// dd
        case 0: { // Validate first d
            if(birthday[0] >= ASCII_ZERO && birthday[0] <= ASCII_THREE) { // Allow only 0 up to 3 for first 'd'
                putch(birthday[i]);
                i++;

                if(i % 2 == 0 && i < 6) {
                    putch('/');
                }
            }
        }break;

        case 1: { // Validate second d
            if(birthday[0] == ASCII_ZERO && birthday[1] >= ASCII_ONE && birthday[1] <= ASCII_NINE) { // If first 'd' is 0, allow 1 to 9 for second 'd'
                putch(birthday[i]);
                i++;

                if(i % 2 == 0 && i < 6) {
                    putch('/');
                }
            }
            else if(birthday[0] == ASCII_THREE && (birthday[1] == ASCII_ZERO || birthday[1] == ASCII_ONE)) { // If first 'd' is 3, allow 0 and 1 for second 'd'
                putch(birthday[i]);
                i++;

                if(i % 2 == 0 && i < 6) {
                    putch('/');
                }
            }
            else if(birthday[0] != ASCII_ZERO && birthday[0] != ASCII_THREE && birthday[1] >= ASCII_ZERO && birthday[1] <= ASCII_NINE) { // If first 'd' is not 0 nor 3, allow 0 to 9 for second 'd'
                putch(birthday[i]);
                i++;

                if(i % 2 == 0 && i < 6) {
                    putch('/');
                }
            }
        }break;

        /// mm
        case 2: { // Validate first m
            if(birthday[2] == ASCII_ZERO || birthday[2] == ASCII_ONE) { // Allow only 0 and 1 for first m
                putch(birthday[i]);
                i++;

                if(i % 2 == 0 && i < 6) {
                    putch('/');
                }
            }
        }break;

        case 3: { // Validate second m
            if(birthday[2] == ASCII_ZERO && birthday[3] >= ASCII_ONE && birthday[3] <= ASCII_NINE) { // If first 'm' is 0, allow 1 to 9 for second 'm'
                putch(birthday[i]);
                i++;

                if(i % 2 == 0 && i < 6) {
                    putch('/');
                }
            }
            else if(birthday[2] == ASCII_ONE && birthday[3] >= ASCII_ZERO && birthday[3] <= ASCII_TWO) { // If first 'm' is 1, allow 0 to 2 for second 'm'
                putch(birthday[i]);
                i++;

                if(i % 2 == 0 && i < 6) {
                    putch('/');
                }
            }

            // Format if a month has 30+ days
            if(birthday[0] == ASCII_THREE) {
                // If month only has 30 days
                if((birthday[2] == ASCII_ZERO && (birthday[3] == ASCII_FOUR || birthday[3] == ASCII_SIX || birthday[3] == ASCII_NINE)) || (birthday[2] == ASCII_ONE && birthday[3] == ASCII_ONE)) {
                    currentCursorPosition.X = 40;
                    SetConsoleCursorPosition(hConsole, currentCursorPosition);
                    cout << "0";
                    birthday[1] = ASCII_ZERO;
                    currentCursorPosition.X = 45;
                    SetConsoleCursorPosition(hConsole, currentCursorPosition);
                }
                else if(birthday[2] == ASCII_ZERO && birthday[3] == ASCII_TWO){ // If month is February
                    currentCursorPosition.X = 39;
                    SetConsoleCursorPosition(hConsole, currentCursorPosition);
                    cout << "29";
                    birthday[0] = ASCII_TWO;
                    birthday[1] = ASCII_NINE;
                    currentCursorPosition.X = 45;
                    SetConsoleCursorPosition(hConsole, currentCursorPosition);
                }
            }
        }break;

        /// yyyy
        case 4: { // Limit first y to 1 and 2
            if(birthday[4] == ASCII_ONE || birthday[4] == ASCII_TWO) {
                putch(birthday[i]);
                i++;

                if(i % 2 == 0 && i < 6) {
                    putch('/');
                }

                if(birthday[4] == ASCII_ONE) {
                    birthday[5] = ASCII_NINE;
                    putch('9');
                    i++;
                }
                else {
                    birthday[5] = ASCII_ZERO;
                    putch('0');
                    i++;
                }
            }
        }break;

        case 6: {
            if(birthday[4] == ASCII_ONE && birthday[6] >= ASCII_ZERO && birthday[6] <= ASCII_NINE) {
                putch(birthday[i]);
                i++;

                if(i % 2 == 0 && i < 6) {
                    putch('/');
                }
            }
            else if(birthday[6] <= ASCII_TWO) {
                putch(birthday[i]);
                i++;

                if(i % 2 == 0 && i < 6) {
                    putch('/');
                }
            }
        }break;

        case 7: {
            if(birthday[4] == ASCII_ONE && birthday[7] >= ASCII_ZERO && birthday[7] <= ASCII_NINE) {
                putch(birthday[i]);
                i++;

                if(i % 2 == 0 && i < 6) {
                    putch('/');
                }
            }
            else if(birthday[6] != ASCII_TWO && birthday[7] >= ASCII_ZERO && birthday[7] <= ASCII_NINE) {
                putch(birthday[i]);
                i++;

                if(i % 2 == 0 && i < 6) {
                    putch('/');
                }
            }
            else if(birthday[7] <= ASCII_THREE) {
                putch(birthday[i]);
                i++;

                if(i % 2 == 0 && i < 6) {
                    putch('/');
                }
            }

            // Check for leap year
            int year = (birthday[4] * 1000) + (birthday[5] * 100) + (birthday[6] * 10) + birthday[7];
            if(!(((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) && (birthday[2] == ASCII_ZERO && birthday[3] == ASCII_TWO)) {
                currentCursorPosition.X = 39;
                SetConsoleCursorPosition(hConsole, currentCursorPosition);
                cout << "28";
                birthday[1] = ASCII_EIGHT;
                currentCursorPosition.X = 49;
                SetConsoleCursorPosition(hConsole, currentCursorPosition);
            }
        }break;
    }
}

void Registration::clear_field(int xPosition, int yPosition, int eraseLength, int newXPos, int newYPos) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD currentCursorPosition;

    CONSOLE_CURSOR_INFO cursorInfo = {1, false};
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    currentCursorPosition.X = xPosition;
    currentCursorPosition.Y = yPosition;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    for(int i = 0; i < eraseLength; i++) {
       cout << " ";
    }

    currentCursorPosition.X = newXPos;
    currentCursorPosition.Y = newYPos;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    cursorInfo = {1, true};
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

/// Display animations
void Registration::missing_card_animation(int timeout) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD currentCursorPosition;

    cout << "\t\t___                    " << endl;
    cout << "\t\t  _|    ___------------" << endl;
    cout << "\t\t   |   | = |          |" << endl;
    cout << "\t\t  _|   |_=_|          |" << endl;
    cout << "\t\t___|       ------------" << endl;
    this_thread::sleep_for(chrono::milliseconds(250));

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                    " << endl;
    cout << "\t\t  _|   ___------------ " << endl;
    cout << "\t\t   |  | = |          | " << endl;
    cout << "\t\t  _|  |_=_|          | " << endl;
    cout << "\t\t___|      ------------ " << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                    " << endl;
    cout << "\t\t  _|  ___------------  " << endl;
    cout << "\t\t   | | = |          |  " << endl;
    cout << "\t\t  _| |_=_|          |  " << endl;
    cout << "\t\t___|     ------------  " << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                    " << endl;
    cout << "\t\t  _| ___------------   " << endl;
    cout << "\t\t   || = |          |   " << endl;
    cout << "\t\t  _||_=_|          |   " << endl;
    cout << "\t\t___|    ------------   " << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                    " << endl;
    cout << "\t\t  _|___------------    " << endl;
    cout << "\t\t   | = |          |    " << endl;
    cout << "\t\t  _|_=_|          |    " << endl;
    cout << "\t\t___|   ------------    " << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                    " << endl;
    cout << "\t\t  _|__------------     " << endl;
    cout << "\t\t   |= |          |     " << endl;
    cout << "\t\t  _|=_|          |     " << endl;
    cout << "\t\t___|  ------------     " << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                    " << endl;
    cout << "\t\t  _|_------------      " << endl;
    cout << "\t\t   | |          |      " << endl;
    cout << "\t\t  _|_|          |      " << endl;
    cout << "\t\t___| ------------      " << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___ \\\\   /             " << endl;
    cout << "\t\t  _|_\\\\-/--------      " << endl;
    cout << "\t\t   | |\\\\        |      " << endl;
    cout << "\t\t  _|_|/\\\\       |      " << endl;
    cout << "\t\t___| /--\\\\-------      " << endl;
    cout << "\t\t    /    \\\\            " << endl;
}

void Registration::booting_atm_animation(int timeout) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD currentCursorPosition;

    cout << "\t\t___                    " << endl;
    cout << "\t\t  _|    ___------------" << endl;
    cout << "\t\t   |   | = |          |" << endl;
    cout << "\t\t  _|   |_=_|          |" << endl;
    cout << "\t\t___|       ------------" << endl;
    this_thread::sleep_for(chrono::milliseconds(250));

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                     " << endl;
    cout << "\t\t  _|     ___------------" << endl;
    cout << "\t\t   |    | = |          |" << endl;
    cout << "\t\t  _|    |_=_|          |" << endl;
    cout << "\t\t___|        ------------" << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                      " << endl;
    cout << "\t\t  _|      ___------------" << endl;
    cout << "\t\t   |     | = |          |" << endl;
    cout << "\t\t  _|     |_=_|          |" << endl;
    cout << "\t\t___|         ------------" << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                       " << endl;
    cout << "\t\t  _|       ___------------" << endl;
    cout << "\t\t   |      | = |          |" << endl;
    cout << "\t\t  _|      |_=_|          |" << endl;
    cout << "\t\t___|          ------------" << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                       " << endl;
    cout << "\t\t  _|       ___------------" << endl;
    cout << "\t\t   |      | = |          |" << endl;
    cout << "\t\t  _|      |_=_|          |" << endl;
    cout << "\t\t___|          ------------" << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                        " << endl;
    cout << "\t\t  _|        ___------------" << endl;
    cout << "\t\t   |       | = |          |" << endl;
    cout << "\t\t  _|       |_=_|          |" << endl;
    cout << "\t\t___|           ------------" << endl;

    for(int i = 0; i < 5; i++) {
        currentCursorPosition.X = 0;
        currentCursorPosition.Y = 9;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);

        this_thread::sleep_for(chrono::milliseconds(timeout + 50));
        cout << "\t\t___                        " << endl;
        cout << "\t\t  _|        ___------------" << endl;
        cout << "\t\t   |       | = |          |" << endl;
        cout << "\t\t  _|       |_=_|          |" << endl;
        cout << "\t\t___|           ------------" << endl;

        currentCursorPosition.X = 0;
        currentCursorPosition.Y = 9;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);

        this_thread::sleep_for(chrono::milliseconds(timeout + 50));
        cout << "\t\t___   |||                  " << endl;
        cout << "\t\t  _|  ||    ___------------" << endl;
        cout << "\t\t   |   ||  | = |          |" << endl;
        cout << "\t\t  _|   |   |_=_|          |" << endl;
        cout << "\t\t___|   O       ------------" << endl;
    }
}

void Registration::reinsert_card_animation(int timeout) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD currentCursorPosition;

    cout << "\t\t___                    " << endl;
    cout << "\t\t  _|_------------      " << endl;
    cout << "\t\t   | |          |      " << endl;
    cout << "\t\t  _|_|          |      " << endl;
    cout << "\t\t___| ------------      " << endl;
    this_thread::sleep_for(chrono::milliseconds(250));

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                    " << endl;
    cout << "\t\t  _|__------------     " << endl;
    cout << "\t\t   |= |          |     " << endl;
    cout << "\t\t  _|=_|          |     " << endl;
    cout << "\t\t___|  ------------     " << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                    " << endl;
    cout << "\t\t  _|___------------    " << endl;
    cout << "\t\t   | = |          |    " << endl;
    cout << "\t\t  _|_=_|          |    " << endl;
    cout << "\t\t___|   ------------    " << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                    " << endl;
    cout << "\t\t  _| ___------------   " << endl;
    cout << "\t\t   || = |          |   " << endl;
    cout << "\t\t  _||_=_|          |   " << endl;
    cout << "\t\t___|    ------------   " << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                    " << endl;
    cout << "\t\t  _|  ___------------  " << endl;
    cout << "\t\t   | | = |          |  " << endl;
    cout << "\t\t  _| |_=_|          |  " << endl;
    cout << "\t\t___|     ------------  " << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                    " << endl;
    cout << "\t\t  _|   ___------------ " << endl;
    cout << "\t\t   |  | = |          | " << endl;
    cout << "\t\t  _|  |_=_|          | " << endl;
    cout << "\t\t___|      ------------ " << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                    " << endl;
    cout << "\t\t  _|    ___------------" << endl;
    cout << "\t\t   |   | = |          |" << endl;
    cout << "\t\t  _|   |_=_|          |" << endl;
    cout << "\t\t___|       ------------" << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                      " << endl;
    cout << "\t\t  _|      ___------------" << endl;
    cout << "\t\t   |     | = |          |" << endl;
    cout << "\t\t  _|     |_=_|          |" << endl;
    cout << "\t\t___|         ------------" << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                       " << endl;
    cout << "\t\t  _|       ___------------" << endl;
    cout << "\t\t   |      | = |          |" << endl;
    cout << "\t\t  _|      |_=_|          |" << endl;
    cout << "\t\t___|          ------------" << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t___                         " << endl;
    cout << "\t\t  _|         ___------------" << endl;
    cout << "\t\t   |  / ==  | = |          |" << endl;
    cout << "\t\t  _|  \\ ==  |_=_|          |" << endl;
    cout << "\t\t___|            ------------" << endl;
}

void Registration::atm_is_full_animation(int timeout) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD currentCursorPosition;

    cout << "\t\t        |||                " << endl;
    cout << "\t\t        ||                 " << endl;
    cout << "\t\t         ||                " << endl;
    cout << "\t\t         |                 " << endl;
    cout << "\t\t         O                 " << endl;
    this_thread::sleep_for(chrono::milliseconds(400));

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout));
    cout << "\t\t                           " << endl;
    cout << "\t\t                           " << endl;
    cout << "\t\t                           " << endl;
    cout << "\t\t                           " << endl;
    cout << "\t\t                           " << endl;

    for(int i = 0; i < 5; i++) {
        currentCursorPosition.X = 0;
        currentCursorPosition.Y = 9;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);

        this_thread::sleep_for(chrono::milliseconds(timeout + 50));
        cout << "\t\t    ||      ||             " << endl;
        cout << "\t\t    ||      ||             " << endl;
        cout << "\t\t        __                 " << endl;
        cout << "\t\t      /    \\               " << endl;
        cout << "\t\t     |      |              " << endl;

        currentCursorPosition.X = 0;
        currentCursorPosition.Y = 9;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);

        this_thread::sleep_for(chrono::milliseconds(timeout + 50));
        cout << "\t\t                           " << endl;
        cout << "\t\t                           " << endl;
        cout << "\t\t                           " << endl;
        cout << "\t\t                           " << endl;
        cout << "\t\t                           " << endl;
    }

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(timeout + 50));
    cout << "\t\t    ||      ||             " << endl;
    cout << "\t\t    ||      ||             " << endl;
    cout << "\t\t        __                 " << endl;
    cout << "\t\t      /    \\               " << endl;
    cout << "\t\t     |      |              " << endl;
}

void Registration::play_animation(int playAnimation) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD currentCursorPosition;

    // Hide the cursor
    CONSOLE_CURSOR_INFO cursorInfo = {1, false};
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    static bool isPrintedOnce = false;
    int timeout = 10;
    string loadingBar = ">";
    const int totalFrames = 23;

    if(!isPrintedOnce) {
        // 4-7-7-4
        cout <<  endl;
        cout <<  endl;
        cout << "\t:|~                            ~|:" << endl;
        cout << "\t   ============================" << endl;
        if(playAnimation == 1) {
            cout << "\t   ||    Card Not Detected   ||" << endl;
        }
        else if(playAnimation == 2) {
            cout << "\t   ||     ATM Booting Up     ||" << endl;
        }
        else if(playAnimation == 3) {
            cout << "\t   ||     Reinsert Card      ||" << endl;
        }
        else {
            cout << "\t   || Not Taking New Clients ||" << endl;
        }
        cout << "\t   ============================" << endl; // 28 characters
        cout << "\t:|~                            ~|:" << endl;
        cout << endl;
        cout << endl;
        // Card animation
        cout << "\n\n\n\n\n\n\n" << endl;
        // Please insert card message
        if(playAnimation == 1) {
            cout << "\n\t   :[                        ]:" << endl;
        }
        else if(playAnimation == 2) {
            cout << "\n" << endl;
            cout << "\n\t   :[------------X-----------]:" << endl;
        }
        else {
            cout << "\n\t   :[------------X-----------]:" << endl;
            cout << "\n" << endl;
        }


        currentCursorPosition.X = 0;
        currentCursorPosition.Y = 9;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);

        if(playAnimation == 1) {
            missing_card_animation(timeout * 5);
        }
        else if(playAnimation == 2) {
            booting_atm_animation(timeout * 5);
        }
        else if(playAnimation == 3) {
            reinsert_card_animation(timeout * 5);
        }
        else {
            atm_is_full_animation(timeout * 5);
        }

        currentCursorPosition.X = 0;
        currentCursorPosition.Y = 17;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        if(playAnimation == 1) {
            string insertCardMessage = "\t    Please insert card...      ";
            int insertCardMessageLength = 26;

            for(int i = 0; i < insertCardMessageLength; i++) {
                cout << insertCardMessage[i];
                this_thread::sleep_for(chrono::milliseconds(25));
            }
        }
        else if(playAnimation == 2) {
            int totalMessages = 3;
            string relaunchProgramMessage[totalMessages] = {
                "\t    File Error. Creating missing files...",
                "\t    File creation complete.\n",
                "\t    Please relaunch the program...      "
            };
            int relaunchMessageLength[totalMessages] = {40, 28, 35};

            for(int i = 0; i < totalMessages; i++) {
                for(int j = 0; j < (relaunchMessageLength[i]); j++) {
                    cout << relaunchProgramMessage[i][j];
                    this_thread::sleep_for(chrono::milliseconds(25));
                }
                cout << endl;
            }
        }
        else if(playAnimation == 3) {
            string reinsertCardMessage = "\t    Safely remove and reinsert card...        ";
            int reinsertCardMessageLength = 39;

            for(int i = 0; i < reinsertCardMessageLength; i++) {
                cout << reinsertCardMessage[i];
                this_thread::sleep_for(chrono::milliseconds(25));
            }
        }
        else {
            string listFullMessage = "\t    List is full...";
            string noRegistrationsMessage = "\t    Currently not accepting new registrations...";
            int listFullMessageLength = 20;
            int noRegistrationsMessageLength = 49;

            currentCursorPosition.Y = 16;
            SetConsoleCursorPosition(hConsole, currentCursorPosition);

            for(int i = 0; i < listFullMessageLength; i++) {
                cout << listFullMessage[i];
                this_thread::sleep_for(chrono::milliseconds(25));
            }
            cout << endl;

            for(int i = 0; i < noRegistrationsMessageLength; i++) {
                cout << noRegistrationsMessage[i];
                this_thread::sleep_for(chrono::milliseconds(25));
            }
        }

        currentCursorPosition.X = 13;
        currentCursorPosition.Y = 18;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);

        isPrintedOnce = true;
    }

    if(playAnimation != 1) {
        if(playAnimation == 2)
            this_thread::sleep_for(chrono::seconds(1));
        currentCursorPosition.X = 0;
        currentCursorPosition.Y = 22;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        return;
    }

    for(int frame = 0; frame <= totalFrames; frame++) {
        cout << loadingBar;
        this_thread::sleep_for(chrono::milliseconds(timeout));
    }

    currentCursorPosition.X = 13;
    currentCursorPosition.Y = 18;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    for(int frame = 0; frame <= totalFrames; frame++) {
        cout << " ";
        this_thread::sleep_for(chrono::milliseconds(timeout));
    }

    for(int i = 0; i <= totalFrames; i++)
        cout << "\b";
}

void Registration::animate_loading_bar(int iterations, int xPosition, int yPosition) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD currentCursorPosition;

    cout << "\n\n\t:[                       ]:" << endl;

    // Animate loading
    currentCursorPosition.X = xPosition;
    currentCursorPosition.Y = yPosition;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(100));
    string loadingMessage = "\n\tLoading...";
    for(int i = 0; i < iterations; i++) {
        if(i == 0){
            for(int j = 0; j < 12; j++) {
                cout << loadingMessage[j];
                this_thread::sleep_for(chrono::milliseconds(25));
            }

            currentCursorPosition.Y = 11;
            SetConsoleCursorPosition(hConsole, currentCursorPosition);
        }

        currentCursorPosition.X = 10;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        for(int k = 0; k < 23; k++) {
            cout << ">";
            this_thread::sleep_for(chrono::milliseconds(10));
        }

        currentCursorPosition.X = 10;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        for(int l = 0; l < 23; l++) {
            cout << " ";
            this_thread::sleep_for(chrono::milliseconds(10));
        }
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

/// Public methods
AccountList Registration::menu() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD currentCursorPosition;

    CONSOLE_CURSOR_INFO cursorInfo = {1, false};
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    // Initialized variables to "" and -1 to indicate as null for input reattempting purpose. If not null, print value
    string accountNumber = "";
    string name = "";
    string dateOfBirth  = "";
    string contactNumber = "";
    int initialDeposit = -1;
    string pin = "";
    string reenterPin = "";
    bool isRegistrationComplete = false;

    AccountList newAccount;
    newAccount.last = 0;

    system("cls");
    cout << "\n" << endl;
    cout << "\t      Please complete the following information.\n" << endl;
    cout << "\t[ Registration Guide ]======================================o" << endl;
    cout << "\t|                                                           |" << endl;
    cout << "\t|     Name must be at least 3 characters long.              |" << endl;
    cout << "\t|     Date of birth is recorded in (dd/mm/yyyy) format.     |" << endl;
    cout << "\t|     Your initial deposit must be a minimum of $5,000.     |" << endl;
    cout << "\t|     Maximum of $40,000 initial deposit.                   |" << endl;
    cout << "\t|     Your PIN code must be 6 digits in length.             |" << endl;
    cout << "\t|                                                           |" << endl;
    cout << "\to===========================================================o" << endl;

    cout << "\n" << endl;
    cout << "\to-----------------------------------------------------------o" << endl;
    cout << "\t| Enter your name           :                               |" << endl;
    cout << "\t|-----------------------------------------------------------|" << endl;
    cout << "\t| Enter your date of birth  :                               |" << endl;
    cout << "\t|-----------------------------------------------------------|" << endl;
    cout << "\t| Enter your contact number : +0?                           |" << endl;
    cout << "\t|-----------------------------------------------------------|" << endl;
    cout << "\t| Enter your initial deposit:                               |" << endl;
    cout << "\t|-----------------------------------------------------------|" << endl;
    cout << "\t| Enter your PIN code       :                               |" << endl;
    cout << "\t|-----------------------------------------------------------|" << endl;
    cout << "\t| Reenter your PIN code     :                               |" << endl;
    cout << "\to-----------------------------------------------------------o" << endl;
    cout << "\t                          [ Esc to return to previous field ]" << endl;

    int initialFieldSpace = 30;
    while(!isRegistrationComplete) {
        cursorInfo = {1, false};
        SetConsoleCursorInfo(hConsole, &cursorInfo);
        /// Cursor position
        int initialX = 39;
        int initialY = 16;
        currentCursorPosition.X = initialX;
        currentCursorPosition.Y = initialY;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        bool didUserEsc = false;

        /// Enter name
        if(name == "") {
            cursorInfo = {1, true};
            SetConsoleCursorInfo(hConsole, &cursorInfo);
            int letter = 0;
            int nameLength = 0;

            while(letter != 13) { // 13 ascii code of 'Enter'
                letter = getch();

                if(letter == ASCII_ESCAPE) {
                    clear_field(initialX - 1, initialY, initialFieldSpace, initialX, initialY);
                    name = "";
                    didUserEsc = true;
                    break;
                }

                if(nameLength != 28 && ((letter >= 65 && letter <= 90) || (letter >= 97 && letter <= 122) || letter == 32)) { // Ascii of letters and space
                    if(!(nameLength == 0 && letter == 32)) {
                        putch(letter);
                        name += string(1, static_cast<char>(letter));
                        nameLength++;
                    }
                }
                else if(letter == 8 && nameLength != 0) {
                    cout << "\b \b";
                    name.pop_back();
                    nameLength--;
                }

                if(nameLength <= 2)
                    letter = 0; // Prevent user from entering without a name
            }
            if(didUserEsc) {
                continue;
            }

            putch(' ');
            cout << endl;
            newAccount.info[newAccount.last].name = name;
        }
        else {
            cout << name << endl;
        }
        initialY += 2;

        /// Enter birthday
        currentCursorPosition.X = initialX;
        currentCursorPosition.Y = initialY;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        if(dateOfBirth == "") {
            cursorInfo = {1, true};
            SetConsoleCursorInfo(hConsole, &cursorInfo);
            int maxInputSize = 9;
            int birthdayInput[maxInputSize]; // Every 2 elements represents mm, dd, and yyyy respectively. Last index dedicated to the 'Enter' key to validate completion

            int i = 0;
            while(i < maxInputSize) {
                birthdayInput[i] = getch();

                if(birthdayInput[i] == ASCII_ESCAPE) {
                    if(i <= 0) {
                        name = "";
                    }
                    dateOfBirth = "";
                    clear_field(initialX - 1, initialY, initialFieldSpace, initialX, initialY);

                    // Go back to name
                    initialY -= 2;
                    clear_field(initialX - 1, initialY, initialFieldSpace, initialX, initialY);

                    didUserEsc = true;
                    break;
                }

                if(i > 0 && birthdayInput[i] == ASCII_BACKSPACE) { // For backspacing input
                    if(i != 0 && i % 2 == 0 && i < 7) {
                        cout << "\b\b  \b\b";
                    }
                    else {
                        cout << "\b \b";
                    }

                    if(i == 6) {
                        i -= 2;
                    }
                    else {
                        i--;
                    }
                }
                else if(i == 8) {
                    if(birthdayInput[i] == ASCII_ENTER) { // For entering (to finalize input)
                        putch(' ');
                        i++;
                    }
                }
                else if(birthdayInput[i] != ASCII_BACKSPACE && birthdayInput[i] >= ASCII_ZERO && birthdayInput[i] <= ASCII_NINE) { // For formatting and displaying input
                    date_formatter(i, birthdayInput);
                }
            }
            if(didUserEsc) {
                continue;
            }

            // Format the birthday as one whole string
            dateOfBirth = format_as_string(birthdayInput, maxInputSize - 1, 2, 5, "/");

            cout << endl;
            newAccount.info[newAccount.last].dateOfBirth = dateOfBirth;
        }
        else {
            cout << dateOfBirth << endl;
        }
        initialX += 3;
        initialY += 2;

        /// Enter contact no.
        currentCursorPosition.X = initialX;
        currentCursorPosition.Y = initialY;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        if(contactNumber == "") {
            cursorInfo = {1, true};
            SetConsoleCursorInfo(hConsole, &cursorInfo);
            const int maxNumberSize = 11;
            int numberInput[maxNumberSize]; // 10 digit contact number. Last index dedicated to the 'Enter' key to validate completion

            int i = 0;
            while(i < maxNumberSize) {
                numberInput[i] = getch();

                if(numberInput[i] == ASCII_ESCAPE) {
                    if(i <= 0) {
                        dateOfBirth = "";
                    }
                    contactNumber = "";
                    clear_field(initialX - 1, initialY, initialFieldSpace - 3, initialX, initialY);

                    // Go back to dateOfBirth
                    initialX -= 3;
                    initialY -= 2;
                    clear_field(initialX - 1, initialY, initialFieldSpace, initialX, initialY);

                    didUserEsc = true;
                    break;
                }

                if(i > 0 && numberInput[i] == ASCII_BACKSPACE) {
                    i--;

                    if(i == 4) {
                        cout << "\b\b  \b\b";
                    }
                    else {
                        cout << "\b \b";
                    }
                }
                else if(i == maxNumberSize - 1) {
                    if(numberInput[i] == ASCII_ENTER) { // For entering (to finalize input)
                        putch(' ');
                        i++;
                    }
                }
                else if(numberInput[i] >= ASCII_ZERO && numberInput[i] <= ASCII_NINE) {
                    if(numberInput[0] == ASCII_ZERO && numberInput[1] == ASCII_NINE) {
                        cout << "\b";
                        i--;
                        numberInput[0] = numberInput[1];
                    }
                    putch(numberInput[i]);
                    i++;

                    if(i == 4) {
                        putch(' ');
                    }
                }
            }
            if(didUserEsc) {
                continue;
            }


            cout << endl;
            contactNumber = format_as_string(numberInput, maxNumberSize - 1);
            newAccount.info[newAccount.last].contactNumber = "0" + contactNumber;
        }
        else {
            int i = 0;
            for(char c : contactNumber) {
                cout << c;
                i++;
                if(i % 3 == 0 && i <= 6) {
                    cout << " ";
                }
            }
        }
        initialX -= 3;
        initialY += 2;

        /// Enter initial deposit
        currentCursorPosition.X = initialX;
        currentCursorPosition.Y = initialY;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        if(initialDeposit == -1) {
            cursorInfo = {1, true};
            SetConsoleCursorInfo(hConsole, &cursorInfo);
            const int maximumDeposit = 40000;
            const int minimumDeposit = 5000;
            const int digitLength = 6;
            int depositInput[digitLength]; // 5 digit initial deposit. Last index dedicated to the 'Enter' key
            int totalDeposit;

            int i = 0;
            while(i < digitLength) {
                depositInput[i] = getch();

                if(depositInput[i] == ASCII_ESCAPE) {
                    if(i <= 0) {
                        contactNumber = "";
                    }
                    initialDeposit = -1;
                    clear_field(initialX - 1, initialY, initialFieldSpace, initialX, initialY);

                    // Go back to contactNumber
                    initialX += 3;
                    initialY -= 2;
                    clear_field(initialX - 1, initialY, initialFieldSpace - 3, initialX, initialY);

                    didUserEsc = true;
                    break;
                }

                if(i > 0 && depositInput[i] == ASCII_BACKSPACE) {
                    cout << "\b \b";
                    i--;
                }
                else if((depositInput[i] == ASCII_ENTER) || i == digitLength - 1) {  // For entering (to finalize input)
                    if(depositInput[i] == ASCII_ENTER && totalDeposit >= minimumDeposit && totalDeposit <= maximumDeposit) {
                        putch(' ');
                        break;
                    }
                }
                else if(depositInput[i] >= ASCII_ZERO && depositInput[i] <= ASCII_NINE) {
                    if(depositInput[0] == ASCII_ZERO && i == 0) {
                        continue;
                    }
                    else {
                        putch(depositInput[i]);
                        i++;
                    }
                }

                if(i > 0)
                    totalDeposit = stoi(format_as_string(depositInput, i));
            }
            if(didUserEsc) {
                continue;
            }

            initialDeposit = totalDeposit;
            newAccount.info[newAccount.last].initialDeposit = initialDeposit;
        }
        else {
            cout << initialDeposit << endl;
        }
        initialY += 2;

        /// Enter pin code
        currentCursorPosition.X = initialX;
        currentCursorPosition.Y = initialY;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        if(pin == "") {
            cursorInfo = {1, true};
            SetConsoleCursorInfo(hConsole, &cursorInfo);
            const int maxPinLength = 7;
            int pinInput[maxPinLength]; // 6 digit pin code. Last index dedicated to the 'Enter' key to validate completion

            int i = 0;
            while(i < maxPinLength) {
                pinInput[i] = getch();

                if(pinInput[i] == ASCII_ESCAPE) {
                    if(i <= 0) {
                        initialDeposit = -1;
                    }
                    pin = "";
                    clear_field(initialX - 1, initialY, initialFieldSpace, initialX, initialY);

                    // Go back to initialDeposit
                    initialY -= 2;
                    clear_field(initialX - 1, initialY, initialFieldSpace, initialX, initialY);

                    didUserEsc = true;
                    break;
                }

                if(i > 0 && pinInput[i] == ASCII_BACKSPACE) {
                    cout << "\b \b";
                    i--;
                }
                else if(i == maxPinLength - 1) {
                    if(pinInput[i] == ASCII_ENTER) { // For entering (to finalize input)
                        putch(' ');
                        i++;
                    }
                }
                else if(pinInput[i] >= ASCII_ZERO && pinInput[i] <= ASCII_NINE) {
                    putch('*');
                    i++;
                }
            }
            if(didUserEsc) {
                continue;
            }

            pin = format_as_string(pinInput, maxPinLength - 1);
        }
        else {
            cout << "******" << endl;
        }
        initialY += 2;

        /// Re-enter pin code
        currentCursorPosition.X = initialX;
        currentCursorPosition.Y = initialY;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        if(reenterPin == "") {
            cursorInfo = {1, true};
            SetConsoleCursorInfo(hConsole, &cursorInfo);
            const int maxPinLength = 7;
            int reenterPinInput[maxPinLength]; // Re-enter 6 digit pin code. Last index dedicated to the 'Enter' key to validate completion

            int i = 0;
            while(i < maxPinLength) {
                reenterPinInput[i] = getch();

                if(reenterPinInput[i] == ASCII_ESCAPE) {
                    if(i <= 0) {
                        pin = "";
                    }
                    reenterPin = "";
                    clear_field(initialX - 1, initialY, initialFieldSpace, initialX, initialY);

                    // Go back to pin
                    initialY -= 2;
                    clear_field(initialX - 1, initialY, initialFieldSpace, initialX, initialY);

                    didUserEsc = true;
                    break;
                }

                if(i > 0 && reenterPinInput[i] == ASCII_BACKSPACE) {
                    if(i == maxPinLength - 1) {
                        currentCursorPosition.X = initialX + 7;
                        SetConsoleCursorPosition(hConsole, currentCursorPosition);
                        cout << "                 ";
                        cursorInfo = {1, true};
                        SetConsoleCursorInfo(hConsole, &cursorInfo);
                        currentCursorPosition.X = initialX + 6;
                        SetConsoleCursorPosition(hConsole, currentCursorPosition);
                    }
                    cout << "\b \b";
                    i--;
                }
                else if(i == maxPinLength - 1 && reenterPin != pin) {
                    if(reenterPinInput[i] == ASCII_ENTER) {
                        cursorInfo = {1, false};
                        SetConsoleCursorInfo(hConsole, &cursorInfo);
                        currentCursorPosition.X = initialX + 7;
                        SetConsoleCursorPosition(hConsole, currentCursorPosition);
                        cout << "PINs do not match";
                    }
                }
                else if(i == maxPinLength - 1 && reenterPin == pin) {
                    if(reenterPinInput[i] == ASCII_ENTER) { // For entering (to finalize input)
                        putch(' ');
                        i++;
                    }
                }
                else if(i != maxPinLength - 1 && reenterPinInput[i] >= ASCII_ZERO && reenterPinInput[i] <= ASCII_NINE) {
                    putch('*');
                    i++;
                }

                reenterPin = format_as_string(reenterPinInput, maxPinLength - 1);
                newAccount.info[newAccount.last].pinCode = encrypt(reenterPin);
            }
            if(didUserEsc) {
                continue;
            }

            // Final process
            string prefix = dateOfBirth.substr(8, 2);
            int accNumDigits = to_string(accList.last).length();
            switch(accNumDigits) {
            case 1:
                newAccount.info[newAccount.last].accountNumber = prefix + "00" + to_string(accList.last);
                break;

            case 2:
                newAccount.info[newAccount.last].accountNumber = prefix + "0" + to_string(accList.last);
                break;

            case 3:
                newAccount.info[newAccount.last].accountNumber = prefix + to_string(accList.last);
                break;
            }
            newAccount.isBlocked[newAccount.last] = false;
            isRegistrationComplete = true; // Complete registration
        }
        else {
            cout << reenterPin << endl;
        }
    }

    cursorInfo = {1, false};
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    system("cls");
    cout << "\n\n" << endl;
    cout << "\t[ Automated Theft Machine ]===============================================o" << endl;
    cout << "\t|                                                                         |" << endl;
    cout << "\t|     Congratulations! Your account has been successfully registered.     |" << endl;
    cout << "\t|                                                                         |" << endl;
    cout << "\to=========================================================================o" << endl;
    cout << endl;

    animate_loading_bar(2, 10, 9);

    return newAccount;
}

int Registration::find_atm_card() {
    update_total_drives(); // Perform necessary changes

    int initialDriveCount;

    ifstream fileReader(TOTAL_DRIVES_FILE);
    fileReader >> initialDriveCount;
    fileReader.close();

    int totalDriveCount;
    while(true) {
        totalDriveCount = detect_total_drives();

        if(initialDriveCount == totalDriveCount) {
            if(check_card()) {
                break; // Exit loop when change in drive count is detected
            }
            play_animation(1);
        }
        else {
            if(!check_card()) {
                setup_card(); // Sets up the card if it does not have the necessary file needed to operate
            }
            break;
        }
        Sleep(500);
    }

    Sleep(100); // Wait before reading card
    string cardFilePath = DRIVE_PATHS[totalDriveCount - 1] + CARD_FILE;
    ifstream cardDataReader(cardFilePath);
    string accountNumber;
    string pinCode;
    if(cardDataReader.is_open()) {
        cardDataReader >> accountNumber;
        cardDataReader >> pinCode;

        cardDataReader.close();
        if(check_account(accountNumber, pinCode)) {
            return 0; // Return 0 when an account already exists
        }
        else {
            if(isFull()) {
                play_animation(4);
                return -2; // Return -2 when the list is full
            }

            return 1; // Return 1 when there's no account
        }
    }
    cardDataReader.close();

    play_animation(3);
    return -1; // Return -1 when error reading card
}

/// Getter method
bool Registration::get_file_initialization_status() {
    return areFilesInitialized;
}

/// ADT list operations
void Registration::register_account(AccountList newAccount) {
    accList.info[accList.last] = newAccount.info[newAccount.last];
    accList.isBlocked[accList.last] = newAccount.isBlocked[newAccount.last];
    accList.last++;

    return_file();
    return_card();
}

/// Debugging
void Registration::display_registration_list() {
    system("cls");
    cout << "\n[Registration List]\n" << endl;
    for(int i = 0; i < accList.last; i++) {
        cout << "Name: " << accList.info[i].name << endl;
        cout << "Account Number: " << accList.info[i].accountNumber << endl;
        cout << "Pin: " << accList.info[i].pinCode << endl;
        cout << "Birthday: " << accList.info[i].dateOfBirth << endl;
        cout << "Contact: " << accList.info[i].contactNumber << endl;
        cout << "Balance: " << accList.info[i].initialDeposit << endl;
        cout << "Blocked: " << (accList.isBlocked[i] == 1 ? "true" : "false") << endl;
        cout << "Last: " << accList.last << endl;
        cout << endl;
    }
    cout << "[End of Registration List]\n" << endl;
    system("pause");
}
