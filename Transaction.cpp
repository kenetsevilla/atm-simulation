#include "Transaction.h"

#include <AccountManager.h>

#include <iostream>
///#include <cctype>
//#include <string>
#include <conio.h>
#include <thread>
#include <fstream>
#include <windows.h>
//#include <sstream>

using namespace std;

Transaction::Transaction()
{
    retrieve_file();
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    // Hide the cursor
    CONSOLE_CURSOR_INFO cursorInfo = {1, false};
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    isPinVerified = false;
    isTransactionComplete = false;
}

/// File handling
void Transaction::update_account_info() {
    accList.info[loggedInAccount.last] = loggedInAccount.info[loggedInAccount.last];
    accList.isBlocked[loggedInAccount.last] = loggedInAccount.isBlocked[loggedInAccount.last];
}

void Transaction::update_card() {
    int cardIndex = detect_total_drives() - 1;
    string currentFilePath = DRIVE_PATHS[cardIndex] + CARD_FILE;

    ofstream fileWriter(currentFilePath);
    fileWriter << loggedInAccount.info[loggedInAccount.last].accountNumber + " " + loggedInAccount.info[loggedInAccount.last].pinCode;
    fileWriter.close();
}

/// Preparation methods
void Transaction::initialize_logged_in_account() {
    loggedInAccount = read_card();
}

bool Transaction::verify_pin() {
    if(loggedInAccount.isBlocked[loggedInAccount.last]) {
        return false;
    }

    int pinLength = 7;
    int pinInput[pinLength]; // 6 digit pin. Last index dedicated for 'Enter' key to validate completion
    string pinCode = "-1";

    system("cls");
    cout << "\n" << endl;
    cout << "\t================================" << endl;
    cout << "\t||   Please enter your PIN    ||" << endl;
    cout << "\t================================" << endl;
    cout << "\n" << endl;
    cout << "\t     .--------------------.     " << endl;
    cout << "\t     |  _  _  _  _  _  _  |     " << endl;
    cout << "\t     '--------------------'     " << endl;
    cout << "\n" << endl;
    cout << "\t     Press enter to confirm     " << endl;
    cout << "\to==============================o" << endl;
    cout << "\t|                              |" << endl;
    cout << "\to==============================o" << endl;

    int x = 16;
    int y = 8;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD currentCursorPosition;
    currentCursorPosition.X = x;
    currentCursorPosition.Y = y;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    bool isPinCorrect;
    int attempts = 3;
    int i = 0;
    do {
        pinInput[i] = getch();
        pinCode = format_as_string(pinInput, pinLength - 1);
        isPinCorrect = encrypt(pinCode) == loggedInAccount.info[loggedInAccount.last].pinCode ? true : false;

        // Erase incorrect pin message
        if(i == pinLength - 1 && pinInput[i] == ASCII_BACKSPACE) {
            currentCursorPosition.X = x + 1;
            currentCursorPosition.Y = y + 6;
            SetConsoleCursorPosition(hConsole, currentCursorPosition);
            cout << "             " << endl;
            currentCursorPosition.X = x + 18;
            currentCursorPosition.Y = y;
            SetConsoleCursorPosition(hConsole, currentCursorPosition);
        }

        if(i != 0 && pinInput[i] == ASCII_BACKSPACE) {
            cout << "\b\b\b";
            putch('_');
            cout << "\b";
            i--;
        }
        else if(i == pinLength - 1) {
            if(pinInput[i] == ASCII_ENTER && isPinCorrect) { // For entering (to finalize input)
                i++;
            }
            else if(pinInput[i] == ASCII_ENTER) {
                attempts--;

                if(attempts < 3) {
                    currentCursorPosition.X = x + 1;
                    currentCursorPosition.Y = y + 6;
                    SetConsoleCursorPosition(hConsole, currentCursorPosition);
                    cout << "Incorrect PIN" << endl;
                }

                if(attempts <= 0) {
                    currentCursorPosition.X = 6;
                    currentCursorPosition.Y = 13;
                    SetConsoleCursorPosition(hConsole, currentCursorPosition);
                    cout << "\to==============================o" << endl;
                    cout << "\t|  Consecutive incorrect PINs  |" << endl;
                    cout << "\to==============================o" << endl;

                    loggedInAccount.isBlocked[loggedInAccount.last] = true;
                    update_account_info();

                    this_thread::sleep_for(chrono::milliseconds(1000));
                    return false; // Block account
                }
            }
        }
        else if(i < pinLength - 1 && pinInput[i] >= ASCII_ZERO && pinInput[i] <= ASCII_NINE) {
            putch('*');
            cout << "  ";
            i++;
        }
    }while(i < pinLength);

    return true; // If account has been verified
}

/// Menus
int Transaction::another_transaction_menu() {
    system("cls");
    cout << "\n" << endl;
    cout << "\t\t  ============o==============o============" << endl;
    cout << "\t\t          Bank of the Walking Debt             " << endl;
    cout << "\t\t     ==================================   " << endl;
    cout << "\n\n" << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t        Would you like to make another transaction?            " << endl;
    cout << endl;
    cout << "\t.---.  .-----------------.        .----------------.  .---.    " << endl;
    cout << "\t| 1 |--|       Yes       |        |       No       |--| 2 |    " << endl;
    cout << "\t'---'  '-----------------'        '----------------'  '---'    " << endl;
    cout << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                   =====================                       " << endl;
    cout << "\t                    o========O========o                        " << endl;
    cout << endl;
    cout << "\t                                                               " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\t|                                                         |    " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\n" << endl;

    int value;
    do {
        value = getch();
    }while(value != ASCII_ONE && value != ASCII_TWO);
    return value - '0';
}

int Transaction::withdraw_menu() {
    if(loggedInAccount.info[loggedInAccount.last].initialDeposit < 100) {
        system("cls");
        cout << "\n" << endl;
        cout << "\t\t  ============o==============o============" << endl;
        cout << "\t\t          Bank of the Walking Debt        " << endl;
        cout << "\t\t     ==================================   " << endl;
        cout << "\n\n" << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t       o===========================================o           " << endl;
        cout << "\t       |   You currently have insufficient funds   |           " << endl;
        cout << "\t       o===========================================o           " << endl;
        cout << "\t                                                               " << endl;
        cout << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                    p================q                         " << endl;
        cout << endl;
        cout << "\t                                                               " << endl;
        cout << "\to---------------------------------------------------------o    " << endl;
        cout << "\t|                                                         |    " << endl;
        cout << "\to---------------------------------------------------------o    " << endl;
        cout << endl;
        this_thread::sleep_for(chrono::milliseconds(1500));
        return -1;
    }

    system("cls");
    cout << "\n" << endl;
    cout << "\t\t  ============o==============o============" << endl;
    cout << "\t\t                 Withdrawal               " << endl;
    cout << "\t\t     ==================================   " << endl;
    cout << "\n\n" << endl;
    cout << "\t       .-------------------------------------------.           " << endl;
    cout << "\t       |     Enter an amount divisible by 100      |           " << endl;
    cout << "\t       '-------------------------------------------'           " << endl;
    cout << endl;
    cout << "\t       [ Amount ]----------------------------------.           " << endl;
    cout << "\t       | $ :                                       |           " << endl;
    cout << "\t       '-------------------------------------------'           " << endl;
    cout << "\t                   Max 20,000 withdrawal                       " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                       Esc to cancel                           " << endl;
    cout << "\t                   =====================                       " << endl;
    cout << "\t                    o========O========o                        " << endl;
    cout << "\t                                                               " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\t|                                                         |    " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\n" << endl;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo = {1, true};
    SetConsoleCursorInfo(hConsole, &cursorInfo);
    COORD currentCursorPosition;
    currentCursorPosition.X = 21;
    currentCursorPosition.Y = 13;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    int maxWithdrawal = 20000;
    int balance = loggedInAccount.info[loggedInAccount.last].initialDeposit;
    int inputLength = 6;
    int input[inputLength]; // 5 digit initial deposit. Last index dedicated to the 'Enter' key
    int amount;             // Amount to be withdrawn
    bool inputIsValid = false;

    int i = 0;
    while(i < inputLength) {
        input[i] = getch();

        if(input[i] == ASCII_ESCAPE) {
            return -1;
        }

        if(i > 0 && input[i] == ASCII_BACKSPACE) {
            cout << "\b \b";
            i--;
        }
        else if((input[i] == ASCII_ENTER && inputIsValid) || i == inputLength - 1) {  // For entering (to finalize input)
            if((input[i] == ASCII_ENTER && inputIsValid) && (amount <= balance && amount <= maxWithdrawal)) {
                cursorInfo = {1, false};
                SetConsoleCursorInfo(hConsole, &cursorInfo);
                putch(' ');
                break;
            }
        }
        else if(input[i] >= ASCII_ZERO && input[i] <= ASCII_NINE) {
            if(input[0] == ASCII_ZERO && i == 0) {
                continue;
            }
            else {
                putch(input[i]);
                i++;
            }
        }

        // Check for input validity
        if(i > 0)
            amount = stoi(format_as_string(input, i));
        if(amount % 100 == 0) {
            inputIsValid = true;
        }
        else {
            inputIsValid = false;
        }
    }
    return amount;
}

int Transaction::deposit_menu() {
    system("cls");
    cout << "\n" << endl;
    cout << "\t\t  ============o==============o============" << endl;
    cout << "\t\t                   Deposit                " << endl;
    cout << "\t\t     ==================================   " << endl;
    cout << "\n\n" << endl;
    cout << "\t       .-------------------------------------------.           " << endl;
    cout << "\t       |     Enter an amount divisible by 100      |           " << endl;
    cout << "\t       '-------------------------------------------'           " << endl;
    cout << endl;
    cout << "\t       [ Amount ]----------------------------------.           " << endl;
    cout << "\t       | $ :                                       |           " << endl;
    cout << "\t       '-------------------------------------------'           " << endl;
    cout << "\t                   Max 100,000 deposit                         " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                      Esc to cancel                            " << endl;
    cout << "\t                   ===================                         " << endl;
    cout << "\t                    o=======O=======o                          " << endl;
    cout << "\t                                                               " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\t|                                                         |    " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\n" << endl;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo = {1, true};
    SetConsoleCursorInfo(hConsole, &cursorInfo);
    COORD currentCursorPosition;
    currentCursorPosition.X = 21;
    currentCursorPosition.Y = 13;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    int maxDeposit = 100000;
    int inputLength = 7;
    int input[inputLength]; // 6 digit initial deposit. Last index dedicated to the 'Enter' key
    int amount;             // Amount to be deposited
    bool inputIsValid = false;

    int i = 0;
    while(i < inputLength) {
        input[i] = getch();

        if(input[i] == ASCII_ESCAPE) {
            return -1;
        }

        if(i > 0 && input[i] == ASCII_BACKSPACE) {
            cout << "\b \b";
            i--;
        }
        else if((input[i] == ASCII_ENTER && inputIsValid) || i == inputLength - 1) {  // For entering (to finalize input)
            if((input[i] == ASCII_ENTER && inputIsValid) && amount <= maxDeposit) {
                cursorInfo = {1, false};
                SetConsoleCursorInfo(hConsole, &cursorInfo);
                putch(' ');
                break;
            }
        }
        else if(input[i] >= ASCII_ZERO && input[i] <= ASCII_NINE) {
            if(input[0] == ASCII_ZERO && i == 0) {
                continue;
            }
            else {
                putch(input[i]);
                i++;
            }
        }

        // Check for input validity
        if(i > 0)
            amount = stoi(format_as_string(input, i));
        if(amount % 100 == 0) {
            inputIsValid = true;
        }
        else {
            inputIsValid = false;
        }
    }
    return amount;
}

int Transaction::fund_transfer_menu() {
    if(accList.last <= 1) {
        system("cls");
        cout << "\n" << endl;
        cout << "\t\t  ============o==============o============" << endl;
        cout << "\t\t          Bank of the Walking Debt        " << endl;
        cout << "\t\t     ==================================   " << endl;
        cout << "\n\n" << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t       o===========================================o           " << endl;
        cout << "\t       |    Currently, there are no other          |           " << endl;
        cout << "\t       |          clients but you in our bank      |           " << endl;
        cout << "\t       o===========================================o           " << endl;
        cout << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                    p================q                         " << endl;
        cout << endl;
        cout << "\t                                                               " << endl;
        cout << "\to---------------------------------------------------------o    " << endl;
        cout << "\t|                                                         |    " << endl;
        cout << "\to---------------------------------------------------------o    " << endl;
        cout << endl;
        this_thread::sleep_for(chrono::milliseconds(1500));
        return -1;
    }

    if(loggedInAccount.info[loggedInAccount.last].initialDeposit < 100) {
        system("cls");
        cout << "\n" << endl;
        cout << "\t\t  ============o==============o============" << endl;
        cout << "\t\t          Bank of the Walking Debt        " << endl;
        cout << "\t\t     ==================================   " << endl;
        cout << "\n\n" << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t       o===========================================o           " << endl;
        cout << "\t       |   You currently have insufficient funds   |           " << endl;
        cout << "\t       o===========================================o           " << endl;
        cout << "\t                                                               " << endl;
        cout << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                    p================q                         " << endl;
        cout << endl;
        cout << "\t                                                               " << endl;
        cout << "\to---------------------------------------------------------o    " << endl;
        cout << "\t|                                                         |    " << endl;
        cout << "\to---------------------------------------------------------o    " << endl;
        cout << endl;
        this_thread::sleep_for(chrono::milliseconds(1500));
        return -1;
    }

    system("cls");
    cout << "\n" << endl;
    cout << "\t\t  ============o==============o============" << endl;
    cout << "\t\t               Funds Transfer               " << endl;
    cout << "\t\t     ==================================   " << endl;
    cout << "\n\n" << endl;
    cout << "\t       [ Receiver ]--------------------------------.           " << endl;
    cout << "\t       | Account Number :                          |           " << endl;
    cout << "\t       '-------------------------------------------'           " << endl;
    cout << endl;
    cout << "\t         Max 50,000 transfer. Must be at least 100             " << endl;
    cout << "\t       [ Amount ]----------------------------------.           " << endl;
    cout << "\t       | $ :                                       |           " << endl;
    cout << "\t       '-------------------------------------------'           " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                       Esc to cancel                           " << endl;
    cout << "\t                   =====================                       " << endl;
    cout << "\t                    o========O========o                        " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\t|                                                         |    " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\n" << endl;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo = {1, true};
    SetConsoleCursorInfo(hConsole, &cursorInfo);
    COORD currentCursorPosition;
    currentCursorPosition.X = 34;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    int accountNumLength = 6;
    int accNumInput[accountNumLength]; // 5 digit account number. Last index dedicated to the 'Enter' key
    bool doesAccountExist = false;
    bool inputIsOwnAccNum = false;
    AccountList receiverAccount;

    int i = 0;
    while(i < accountNumLength) {
        accNumInput[i] = getch();

        if(accNumInput[i] == ASCII_ESCAPE) {
            return -1;
        }

        if(i > 0 && accNumInput[i] == ASCII_BACKSPACE) {
            if((!doesAccountExist || inputIsOwnAccNum) && i == accountNumLength - 1) {
                inputIsOwnAccNum = false;
                cursorInfo = {1, false};
                SetConsoleCursorInfo(hConsole, &cursorInfo);
                currentCursorPosition.X = 17;
                SetConsoleCursorPosition(hConsole, currentCursorPosition);

                cout << "Account Number" << endl;

                currentCursorPosition.X = 39;
                SetConsoleCursorPosition(hConsole, currentCursorPosition);
                cursorInfo = {1, true};
                SetConsoleCursorInfo(hConsole, &cursorInfo);
            }

            cout << "\b \b";
            i--;
        }
        else if(i == accountNumLength - 1) {  // For entering (to finalize input)
            doesAccountExist = check_account(format_as_string(accNumInput, accountNumLength - 1));

            if(accNumInput[i] == ASCII_ENTER && doesAccountExist) {
                string accountNumInput = format_as_string(accNumInput, accountNumLength - 1);
                string currentAccountNum = loggedInAccount.info[loggedInAccount.last].accountNumber;
                if((accNumInput[i] == ASCII_ENTER) && (accountNumInput == currentAccountNum)) {
                    inputIsOwnAccNum = true;
                    cursorInfo = {1, false};
                    SetConsoleCursorInfo(hConsole, &cursorInfo);

                    accNumInput[i] = -1;

                    currentCursorPosition.X = 17;
                    SetConsoleCursorPosition(hConsole, currentCursorPosition);
                    cout << " Your Account " << endl;

                    currentCursorPosition.X = 39;
                    SetConsoleCursorPosition(hConsole, currentCursorPosition);
                    cursorInfo = {1, true};
                    SetConsoleCursorInfo(hConsole, &cursorInfo);
                }
                else {
                    receiverAccount = get_account(format_as_string(accNumInput, accountNumLength - 1));
                    putch(' ');
                    i++;
                }
            }
            else if(accNumInput[i] == ASCII_ENTER && !doesAccountExist) {
                cursorInfo = {1, false};
                SetConsoleCursorInfo(hConsole, &cursorInfo);

                accNumInput[i] = -1;

                currentCursorPosition.X = 17;
                SetConsoleCursorPosition(hConsole, currentCursorPosition);

                cout << "Does Not Exist" << endl;

                currentCursorPosition.X = 39;
                SetConsoleCursorPosition(hConsole, currentCursorPosition);
                cursorInfo = {1, true};
                SetConsoleCursorInfo(hConsole, &cursorInfo);
            }
        }
        else if(accNumInput[i] >= ASCII_ZERO && accNumInput[i] <= ASCII_NINE) {
            putch(accNumInput[i]);
            i++;
        }
    }

    currentCursorPosition.X = 21;
    currentCursorPosition.Y = 14;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    int maxTransfer = 50000;
    int transferLength = 6;
    int input[transferLength];  // 5 digit fund transfer. Last index dedicated to the 'Enter' key
    int amount;                 // Amount to be deposited
    bool amountIsValid = false;

    i = 0;
    while(i < transferLength) {
        input[i] = getch();

        if(input[i] == ASCII_ESCAPE) {
            return -1;
        }

        if(i > 0 && input[i] == ASCII_BACKSPACE) {
            cout << "\b \b";
            i--;
        }
        else if(input[i] == ASCII_ENTER || i == transferLength - 1) {  // For entering (to finalize input)
            if(input[i] == ASCII_ENTER && amount <= maxTransfer && amountIsValid) {
                cursorInfo = {1, false};
                SetConsoleCursorInfo(hConsole, &cursorInfo);
                accList.info[receiverAccount.last].initialDeposit += amount;
                accList.info[loggedInAccount.last].initialDeposit -= amount;
                putch(' ');
                break;
            }
        }
        else if(input[i] >= ASCII_ZERO && input[i] <= ASCII_NINE) {
            if(input[0] == ASCII_ZERO && i == 0) {
                continue;
            }
            else {
                putch(input[i]);
                i++;
            }
        }

        // Check for amount validity
        if(i > 0)
            amount = stoi(format_as_string(input, i));
        if(amount >= 100 && amount <= loggedInAccount.info[loggedInAccount.last].initialDeposit) {
            amountIsValid = true;
        }
        else {
            amountIsValid = false;
        }
    }

    return 0;
}

int Transaction::balance_inquiry_menu() {
    system("cls");
    cout << "\n" << endl;
    cout << "\t\t  ============o==============o============" << endl;
    cout << "\t\t                   Balance                " << endl;
    cout << "\t\t     ==================================   " << endl;
    cout << "\n\n" << endl;
    cout << "\t       [ Your Balance ]----------------------------.           " << endl;
    cout << "\t       | $                                         |           " << endl;
    cout << "\t       '-------------------------------------------'           " << endl;
    cout << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t        Would you like to make another transaction?            " << endl;
    cout << endl;
    cout << "\t.---.  .-----------------.        .----------------.  .---.    " << endl;
    cout << "\t| 1 |--|       Yes       |        |       No       |--| 2 |    " << endl;
    cout << "\t'---'  '-----------------'        '----------------'  '---'    " << endl;
    cout << endl;
    cout << "\n\t                                                             " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\t|                                                         |    " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\n" << endl;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD currentCursorPosition;
    currentCursorPosition.X = 19;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    // Count character length
    int balance = loggedInAccount.info[loggedInAccount.last].initialDeposit;
    cout << format_number(balance) << ".00" << endl;

    int option;
    do {
        option = getch();
    }while(option != ASCII_ONE && option != ASCII_TWO);

    return option - '0';
}

string Transaction::change_pin_menu() {
    system("cls");
    cout << "\n" << endl;
    cout << "\t\t  ============o==============o============" << endl;
    cout << "\t\t                 Change PIN             " << endl;
    cout << "\t\t     ==================================   " << endl;
    cout << "\n\n" << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t       [ Enter PIN ]-------------------------------.           " << endl;
    cout << "\t       | Current :                                 |           " << endl;
    cout << "\t       '-------------------------------------------'           " << endl;
    cout << "\t                                                               " << endl;
    cout << endl;
    cout << "\t                       Esc to cancel                           " << endl;
    cout << "\t                    ===================                        " << endl;
    cout << "\t                     o=======O=======o                         " << endl;
    cout << endl;
    cout << "\n\t                                                             " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\t|                                                         |    " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\n" << endl;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo = {1, true};
    SetConsoleCursorInfo(hConsole, &cursorInfo);
    COORD currentCursorPosition;
    currentCursorPosition.X = 27;
    currentCursorPosition.Y = 12;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    int pinLength = 7; // 6 digit pin code. Last index dedicated to the 'Enter' key
    int currentPin[pinLength];
    bool isCurrentPinCorrect = false;
    bool wasErrMsgDisplayed = false;

    int attempts = 3;
    int i = 0;
    while(i < pinLength) {
        currentPin[i] = getch();
        isCurrentPinCorrect = encrypt(format_as_string(currentPin, pinLength - 1)) == loggedInAccount.info[loggedInAccount.last].pinCode ? true : false;

        if(currentPin[i] == ASCII_ESCAPE) {
            CONSOLE_CURSOR_INFO cursorInfo = {1, false};
            SetConsoleCursorInfo(hConsole, &cursorInfo);
            return "escaped";
        }

        // Erase incorrect pin message
        if(i == pinLength - 1 && wasErrMsgDisplayed && currentPin[i] == ASCII_BACKSPACE) {
            currentCursorPosition.X = 31;
            SetConsoleCursorPosition(hConsole, currentCursorPosition);
            cout << "ESC to cancel";
            currentCursorPosition.X = 33;
            currentCursorPosition.Y = 12;
            SetConsoleCursorPosition(hConsole, currentCursorPosition);
            wasErrMsgDisplayed = false;
        }

        if(currentPin[i] == ASCII_BACKSPACE && i != 0) {
            cout << "\b \b";
            i--;
            cursorInfo = {1, true};
            SetConsoleCursorInfo(hConsole, &cursorInfo);
        }
        else if(i == pinLength - 1) {
            if(currentPin[i] == ASCII_ENTER && isCurrentPinCorrect) {
                cursorInfo = {1, false};
                SetConsoleCursorInfo(hConsole, &cursorInfo);
                putch(' ');
                i++;
            }
            else if(currentPin[i] == ASCII_ENTER && !isCurrentPinCorrect) {
                cursorInfo = {1, false};
                SetConsoleCursorInfo(hConsole, &cursorInfo);
                currentCursorPosition.X = 31;
                currentCursorPosition.Y = 16;
                SetConsoleCursorPosition(hConsole, currentCursorPosition);
                cout << "Incorrect PIN";
                wasErrMsgDisplayed = true;

                attempts--;
                if(attempts == 0) {
                    return "incorrect pin";
                }
            }
        }
        else if(currentPin[i] >= ASCII_ZERO && currentPin[i] <= ASCII_NINE) {
            putch('*');
            i++;
        }
    }

    int newPin[pinLength];
    int confirmPin[pinLength];
    bool isNewPinConfirmed = false;
    bool isUserRetrying = false;

    while(!isNewPinConfirmed) {
        currentCursorPosition.X = 0;
        currentCursorPosition.Y = 0;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        cout << "\n" << endl;
        cout << "\t\t  ============o==============o============" << endl;
        cout << "\t\t                 Change PIN             " << endl;
        cout << "\t\t     ==================================   " << endl;
        cout << "\n\n" << endl;
        cout << "\t                                                               " << endl;
        cout << "\t       [ Enter PIN ]-------------------------------.           " << endl;
        cout << "\t       | New :                                     |           " << endl;
        cout << "\t       '-------------------------------------------'           " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t       [ Re-enter PIN ]----------------------------.           " << endl;
        cout << "\t       | Confirm :                                 |           " << endl;
        cout << "\t       '-------------------------------------------'           " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                       Esc to retry                            " << endl;
        cout << "\t                   ====================                        " << endl;
        cout << "\t                    o========O=======o                         " << endl;
        cout << "\n\t                                                             " << endl;
        cout << "\to---------------------------------------------------------o    " << endl;
        cout << "\t|                                                         |    " << endl;
        cout << "\to---------------------------------------------------------o    " << endl;
        cout << "\n" << endl;

        currentCursorPosition.X = 23;
        currentCursorPosition.Y = 10;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        cursorInfo = {1, true};
        SetConsoleCursorInfo(hConsole, &cursorInfo);

        // New pin
        i = 0;
        while(i < pinLength) {
            newPin[i] = getch();

            // Erase message
            if(i == pinLength - 1 && wasErrMsgDisplayed && newPin[i] == ASCII_BACKSPACE) {
                currentCursorPosition.X = 21;
                SetConsoleCursorPosition(hConsole, currentCursorPosition);
                cout << "          ESC to Retry          ";
                currentCursorPosition.X = 29;
                currentCursorPosition.Y = 10;
                SetConsoleCursorPosition(hConsole, currentCursorPosition);
            }
            wasErrMsgDisplayed = false;

            if(i != 0 && newPin[i] == ASCII_BACKSPACE) {
                cout << "\b \b";
                i--;
                cursorInfo = {1, true};
                SetConsoleCursorInfo(hConsole, &cursorInfo);
            }
            else if(newPin[i] == ASCII_ESCAPE) {
                cursorInfo = {1, false};
                SetConsoleCursorInfo(hConsole, &cursorInfo);
                isUserRetrying = true;
                break;
            }
            else if(i == pinLength - 1) {
                bool doesNewPinAndOldPinMatch = encrypt(format_as_string(newPin, pinLength - 1)) == loggedInAccount.info[loggedInAccount.last].pinCode ? true : false;
                if(newPin[i] == ASCII_ENTER && !doesNewPinAndOldPinMatch) {
                    putch(' ');
                    i++;
                }
                else if(newPin[i] == ASCII_ENTER && doesNewPinAndOldPinMatch) {
                    cursorInfo = {1, false};
                    SetConsoleCursorInfo(hConsole, &cursorInfo);
                    currentCursorPosition.X = 21;
                    currentCursorPosition.Y = 17;
                    SetConsoleCursorPosition(hConsole, currentCursorPosition);
                    cout << "New PIN should not match Old PIN" << endl;
                    wasErrMsgDisplayed = true;
                }
            }
            else if(newPin[i] >= ASCII_ZERO && newPin[i] <= ASCII_NINE) {
                putch('*');
                i++;
            }
        }
        if(isUserRetrying) {
            isUserRetrying = false;
            continue;
        }

        currentCursorPosition.X = 27;
        currentCursorPosition.Y = 14;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        // Confirm pin
        i = 0;
        while(i < pinLength) {
            confirmPin[i] = getch();

            // Erase message
            if(i == pinLength - 1 && wasErrMsgDisplayed && confirmPin[i] == ASCII_BACKSPACE) {
                currentCursorPosition.X = 21;
                SetConsoleCursorPosition(hConsole, currentCursorPosition);
                cout << "          ESC to Retry          ";
                currentCursorPosition.X = 33;
                currentCursorPosition.Y = 14;
                SetConsoleCursorPosition(hConsole, currentCursorPosition);
            }
            wasErrMsgDisplayed = false;

            if(i != 0 && confirmPin[i] == ASCII_BACKSPACE) {
                cout << "\b \b";
                i--;
                cursorInfo = {1, true};
                SetConsoleCursorInfo(hConsole, &cursorInfo);
            }
            else if(confirmPin[i] == ASCII_ESCAPE) {
                cursorInfo = {1, false};
                SetConsoleCursorInfo(hConsole, &cursorInfo);
                break;
            }
            else if(i == pinLength - 1) {
                bool doesNewAndConfirmMatch = format_as_string(newPin, pinLength - 1) == format_as_string(confirmPin, pinLength - 1) ? true : false;
                if(confirmPin[i] == ASCII_ENTER && doesNewAndConfirmMatch) {
                    isNewPinConfirmed = true;
                    putch(' ');
                    i++;
                }
                else if(confirmPin[i] == ASCII_ENTER && !doesNewAndConfirmMatch) {
                    cursorInfo = {1, false};
                    SetConsoleCursorInfo(hConsole, &cursorInfo);
                    currentCursorPosition.X = 23;
                    currentCursorPosition.Y = 17;
                    SetConsoleCursorPosition(hConsole, currentCursorPosition);
                    cout << "The PINs entered do not match" << endl;
                    wasErrMsgDisplayed = true;
                }
            }
            else if(confirmPin[i] >= ASCII_ZERO && confirmPin[i] <= ASCII_NINE) {
                putch('*');
                i++;
            }
        }
    }

    cursorInfo = {1, false};
    SetConsoleCursorInfo(hConsole, &cursorInfo);
    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 0;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);
    cout << "\n" << endl;
    cout << "\t\t  ============o==============o============" << endl;
    cout << "\t\t                 Change PIN             " << endl;
    cout << "\t\t     ==================================   " << endl;
    cout << "\n\n" << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t       o===========================================o           " << endl;
    cout << "\t       |           Processing PIN change           |           " << endl;
    cout << "\t       o===========================================o           " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t               :[                        ]:                    " << endl;
    cout << "\t                    p================q                         " << endl;
    cout << "\t                                                               " << endl;
    cout << "\n\t                                                             " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\t|                                                         |    " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\n" << endl;

    animate_loading_bar(1, 25, 16, "");

    return format_as_string(confirmPin, pinLength - 1);
}

/// Animation display
void Transaction::remove_card_animation(int timeout) {
    system("cls");
    cout <<  endl;
    cout <<  endl;
    cout << "\t:|~                            ~|:" << endl;
    cout << "\t   ============================" << endl;
    cout << "\t   ||   Please Remove Card   ||" << endl;
    cout << "\t   ============================" << endl; // 28 characters
    cout << "\t:|~                            ~|:" << endl;
    cout << "\n\n\n\n\n\n\n\n\n" << endl;
    cout << "\n\t   :[                        ]:" << endl;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD currentCursorPosition;
    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 9;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

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
    cout << "\t\t   |  == \\  | = |          |" << endl;
    cout << "\t\t  _|  == /  |_=_|          |" << endl;
    cout << "\t\t___|            ------------" << endl;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 17;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);
    string removeCardMessage = "\t    Safely remove your card...      ";
    int removeCardMessageLength = 31;

    for(int i = 0; i < removeCardMessageLength; i++) {
        cout << removeCardMessage[i];
        this_thread::sleep_for(chrono::milliseconds(25));
    }

    int initialDrives = detect_total_drives();
    int totalDrives;
    do {
        totalDrives = detect_total_drives();
        animate_loading_bar(1, 13, 17, "");
    }while(initialDrives == totalDrives);
}

void Transaction::transaction_complete_animation(int timeout, bool moneyOut) {
    system("cls");
    cout << "\n" << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t       [ Bank of the Walking Debt ]================o           " << endl;
    cout << "\t       |          Thank you for using our          |           " << endl;
    cout << "\t       |          Automated Theft Machine          |           " << endl;
    cout << "\t       o===========================================o           " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    if(moneyOut)
        cout << "\t                   Please take your cash                       " << endl;
    else
        cout << "\t                    -Your cash, is ours                        " << endl;
    cout << "\t                     p===============q                         " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << endl;
    cout << "\n\n" << endl;
    cout << "\t                                                               " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\t|                                                         |    " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\n" << endl;

    if(moneyOut) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo = {1, false};
        SetConsoleCursorInfo(hConsole, &cursorInfo);
        COORD currentCursorPosition;

        this_thread::sleep_for(chrono::milliseconds(500));

        currentCursorPosition.X = 34;
        currentCursorPosition.Y = 15;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        cout << "||   ||" << endl;
        this_thread::sleep_for(chrono::milliseconds(50));

        currentCursorPosition.X = 34;
        currentCursorPosition.Y = 16;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        cout << "||   ||" << endl;
        this_thread::sleep_for(chrono::milliseconds(50));

        currentCursorPosition.X = 33;
        currentCursorPosition.Y = 17;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        cout << "==     ==" << endl;
        this_thread::sleep_for(chrono::milliseconds(50));

        currentCursorPosition.X = 33;
        currentCursorPosition.Y = 18;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        cout << "\\\\     //" << endl;
        this_thread::sleep_for(chrono::milliseconds(50));

        currentCursorPosition.X = 34;
        currentCursorPosition.Y = 19;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        cout << "\\\\   //" << endl;
        this_thread::sleep_for(chrono::milliseconds(50));

        currentCursorPosition.X = 35;
        currentCursorPosition.Y = 20;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        cout << "\\\\ //" << endl;
        this_thread::sleep_for(chrono::milliseconds(50));


        currentCursorPosition.X = 0;
        currentCursorPosition.Y = 22;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        cout << "        o---------------------------------------------------------o    " << endl;
        cout << "        |             -------------------------------             |    " << endl;
        cout << "        o---------------------------------------------------------o    " << endl;
        this_thread::sleep_for(chrono::milliseconds(timeout));

        currentCursorPosition.X = 0;
        currentCursorPosition.Y = 22;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        cout << "        o---------------------------------------------------------o    " << endl;
        cout << "        |             |____ Man in the Moon ________|             |    " << endl;
        cout << "        o---------------------------------------------------------o    " << endl;
        this_thread::sleep_for(chrono::milliseconds(timeout));

        currentCursorPosition.X = 0;
        currentCursorPosition.Y = 22;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        cout << "        o---------------------------------------------------------o    " << endl;
        cout << "        |             |    /     Hundred Debts  100 |             |    " << endl;
        cout << "        o-------------|____ Man in the Moon ________|-------------o    " << endl;
        this_thread::sleep_for(chrono::milliseconds(timeout));

        currentCursorPosition.X = 0;
        currentCursorPosition.Y = 22;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        cout << "        o---------------------------------------------------------o    " << endl;
        cout << "        |             |     /      \\                |             |    " << endl;
        cout << "        o-------------|    /     Hundred Debts  100 |-------------o    " << endl;
        cout << "                      |____ Man in the Moon ________|                  " << endl;
        this_thread::sleep_for(chrono::milliseconds(timeout));

        currentCursorPosition.X = 0;
        currentCursorPosition.Y = 22;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        cout << "        o---------------------------------------------------------o    " << endl;
        cout << "        |             |      _|  |_                 |             |    " << endl;
        cout << "        o-------------|     /      \\                |-------------o    " << endl;
        cout << "                      |    /     Hundred Debts  100 |                  " << endl;
        cout << "                      |____ Man in the Moon ________|                  " << endl;
        this_thread::sleep_for(chrono::milliseconds(timeout));
    }

    this_thread::sleep_for(chrono::milliseconds(500));
}

void Transaction::animate_loading_bar(int iterations, int xPosition, int yPosition, string message) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD currentCursorPosition;

    currentCursorPosition.X = xPosition - 2;
    currentCursorPosition.Y = yPosition + 1;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    cout << ":[                        ]:" << endl;

    // Animate loading
    currentCursorPosition.X = xPosition;
    currentCursorPosition.Y = yPosition;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    this_thread::sleep_for(chrono::milliseconds(100));
    int messageLength = message.length();
    for(int i = 0; i < iterations; i++) {
        if(i == 0){
            for(int j = 0; j < messageLength; j++) {
                cout << message[j];
                this_thread::sleep_for(chrono::milliseconds(25));
            }

            currentCursorPosition.Y = yPosition + 1;
            SetConsoleCursorPosition(hConsole, currentCursorPosition);
        }

        currentCursorPosition.X = xPosition;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        for(int k = 0; k < 24; k++) {
            cout << ">";
            this_thread::sleep_for(chrono::milliseconds(10));
        }

        currentCursorPosition.X = xPosition;
        SetConsoleCursorPosition(hConsole, currentCursorPosition);
        for(int l = 0; l < 24; l++) {
            cout << " ";
            this_thread::sleep_for(chrono::milliseconds(10));
        }
        this_thread::sleep_for(chrono::milliseconds(500));
    }
}

void Transaction::animate_processed_transaction(int timeout) {
    system("cls");
    cout << "\n" << endl;
    cout << "\t\t  ============o==============o============" << endl;
    cout << "\t\t          Bank of the Walking Debt        " << endl;
    cout << "\t\t     ==================================   " << endl;
    cout << "\n\n" << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t       o===========================================o           " << endl;
    cout << "\t       |          Processing Transaction           |           " << endl;
    cout << "\t       o===========================================o           " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                    p================q                         " << endl;
    cout << endl;
    cout << "\t                                                               " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\t|                                                         |    " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\n" << endl;

    animate_loading_bar(1, 25, 17, "Loading...");
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD currentCursorPosition;

    currentCursorPosition.X = 0;
    currentCursorPosition.Y = 0;
    SetConsoleCursorPosition(hConsole, currentCursorPosition);

    cout << "\n" << endl;
    cout << "\t\t  ============o==============o============" << endl;
    cout << "\t\t          Bank of the Walking Debt        " << endl;
    cout << "\t\t     ==================================   " << endl;
    cout << "\n\n" << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t       o===========================================o           " << endl;
    cout << "\t       |    Your transaction has been processed    |           " << endl;
    cout << "\t       o===========================================o           " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << endl;
    cout << "\t                                                               " << endl;
    cout << "\t                                                               " << endl;
    cout << "\t               :[                        ]:                    " << endl;
    cout << "\t                    p================q                         " << endl;
    cout << endl;
    cout << "\t                                                               " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\t|                                                         |    " << endl;
    cout << "\to---------------------------------------------------------o    " << endl;
    cout << "\n" << endl;

    this_thread::sleep_for(chrono::milliseconds(1250));
}

/// Public methods
int Transaction::menu() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo = {1, false};
    SetConsoleCursorInfo(hConsole, &cursorInfo);

    // Prepare necessary initializations and authentications
    initialize_logged_in_account();
    if(!isPinVerified) {
        isPinVerified = verify_pin();
    }

    system("cls");
    if(isPinVerified) {
        cout << "\n" << endl;
        cout << "\t\t  o======================================o" << endl;
        cout << "\t\t          Bank of the Walking Debt        " << endl;
        cout << "\t\t     o================================o   " << endl;
        cout << "\n\n" << endl;
        cout << "\t.---.  .-----------------.        .----------------.  .---.    " << endl;
        cout << "\t| 1 |--| Balance Inquiry |        |    Withdraw    |--| 4 |    " << endl;
        cout << "\t'---'  '-----------------'        '----------------'  '---'    " << endl;
        cout << endl;
        cout << "\t.---.  .-----------------.        .----------------.  .---.    " << endl;
        cout << "\t| 2 |--|   Change PIN    |        |    Deposit     |--| 5 |    " << endl;
        cout << "\t'---'  '-----------------'        '----------------'  '---'    " << endl;
        cout << endl;
        cout << "\t.---.  .-----------------.                                     " << endl;
        cout << "\t| 3 |--|  Fund Transfer  |                                     " << endl;
        cout << "\t'---'  '-----------------'                                     " << endl;
        cout << endl;
        cout << "\n\t                   Please Enter A Number                     " << endl;
        cout << "\to---------------------------------------------------------o    " << endl;
        cout << "\t|                                                         |    " << endl;
        cout << "\to---------------------------------------------------------o    " << endl;
        cout << "\n" << endl;
    }
    else {
        system("cls");
        cout << "\n\n" << endl;
        cout << "\t[ Bank of the Walking Debt ]========================================================o" << endl;
        cout << "\t|                                                                                   |" << endl;
        cout << "\t|      Your card has been blocked. You cannot use our Automated Theft Machine.      |" << endl;
        cout << "\t|                                                                                   |" << endl;
        cout << "\to===================================================================================o" << endl;
        cout << endl;
        return_file();
        isTransactionComplete = true;
        return -1;
    }

    int value = getch();
    return value;
}

void Transaction::read_file() {
    retrieve_file();
}

/// Transaction methods
void Transaction::withdraw() {
    int result = withdraw_menu();

    if(result != -1) {
        loggedInAccount.info[loggedInAccount.last].initialDeposit -= result;
        update_account_info();

        return_file();
        animate_processed_transaction(50);
        remove_card_animation(50);
        transaction_complete_animation(50, true);

        isTransactionComplete = true;
    }
}

void Transaction::deposit() {
    int result = deposit_menu();

    if(result != -1) {
        loggedInAccount.info[loggedInAccount.last].initialDeposit += result;
        update_account_info();

        return_file();
        animate_processed_transaction(50);
        remove_card_animation(50);
        transaction_complete_animation(50);

        isTransactionComplete = true;
    }
}

void Transaction::fund_transfer() {
    int result = fund_transfer_menu();

    if(result != -1) {
        return_file();
        animate_processed_transaction(50);

        if(another_transaction_menu() == 2) {
            remove_card_animation(50);
            transaction_complete_animation(50);

            isTransactionComplete = true;
        }
    }
}

void Transaction::balance_inquiry() {
    int result = balance_inquiry_menu();

    if(result == 2) {
        remove_card_animation(50);
        transaction_complete_animation(50);

        isTransactionComplete = true;
    }
}

void Transaction::change_pin() {
    string result = change_pin_menu();

    if(result == "incorrect pin") {
        system("cls");
        cout << "\n" << endl;
        cout << "\t\t  ============o==============o============" << endl;
        cout << "\t\t          Bank of the Walking Debt        " << endl;
        cout << "\t\t     ==================================   " << endl;
        cout << "\n\n" << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t       o===========================================o           " << endl;
        cout << "\t       |        Consecutive Incorrect PINs         |           " << endl;
        cout << "\t       o===========================================o           " << endl;
        cout << "\t                                                               " << endl;
        cout << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                                                               " << endl;
        cout << "\t                    p================q                         " << endl;
        cout << endl;
        cout << "\t                                                               " << endl;
        cout << "\to---------------------------------------------------------o    " << endl;
        cout << "\t|                                                         |    " << endl;
        cout << "\to---------------------------------------------------------o    " << endl;
        cout << endl;
        this_thread::sleep_for(chrono::milliseconds(1500));
    }
    else if(result != "escaped") {
        loggedInAccount.info[loggedInAccount.last].pinCode = encrypt(result);
        update_account_info();
        return_file();
        return_card(loggedInAccount.last);

        if(another_transaction_menu() == 2) {
            remove_card_animation(50);
            transaction_complete_animation(50);

            isTransactionComplete = true;
        }
    }
}

/// Getter method
bool Transaction::get_is_transaction_complete() {
    return isTransactionComplete;
}

/// Debugging
void Transaction::display_transaction_list() {
    system("cls");
    cout << "\n[Transaction List]\n" << endl;
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
    cout << "[End of Transaction List]\n" << endl;
    system("pause");
}
