#include "AccountManager.h"

#include <iostream>
///#include <cctype>
#include <string>
#include <conio.h>
#include <thread>
#include <fstream>
#include <windows.h>
#include <sstream>

using namespace std;

AccountManager::AccountManager()
{
    accList.last = 0;
}

/// Helper methods
AccountList AccountManager::get_account(string accountNumber) {
    AccountList account;
    account.last = -1; // Return last as -1 when account is not found
    for(int i = 0; i < accList.last; i++) {
        if(accList.info[i].accountNumber == accountNumber) {
            account.info[i] = accList.info[i];
            account.isBlocked[i] = accList.isBlocked[i];
            account.last = i;
            return account;
        }
    }
    return account;
}

bool AccountManager::check_account(string accountNumber) {
    for(int i = 0; i < accList.last; i++) {
        if(accList.info[i].accountNumber == accountNumber) {
            return true; // Return true if an account with such account number exists
        }
    }
    return false; // Return false otherwise
}

bool AccountManager::check_account(string accountNumber, string pinCode) {
    for(int i = 0; i < accList.last; i++) {
        if(accList.info[i].accountNumber == accountNumber && accList.info[i].pinCode == pinCode) {
            return true; // Return true if an account with such account number and pin code exists
        }
    }
    return false; // Return false otherwise
}

string AccountManager::format_as_string(int characters[], int expectedStringLength, int addMorphemesInterval, int stopMorphemesAtIndex, string affix, string prefix, string suffix) {
    string concatenatedString = "";
    for(int i = 0; i < expectedStringLength; i++) {
        int j = i + 1;
        concatenatedString += to_string(characters[i] - '0'); // -'0' to display value and not ascii code

        if(affix != "") {
            if(j % addMorphemesInterval == 0 && i != 0 && j < stopMorphemesAtIndex) {
                concatenatedString += affix;
            }
        }
        else if(prefix != "") {
            concatenatedString = prefix + concatenatedString;
        }
    }

    if(suffix != "") {
        concatenatedString += suffix;
    }

    return concatenatedString;
}

string AccountManager::format_number(int number) {
    string formattedNumber;
    string numberString = to_string(number);
    int stringLength = numberString.length();
    int interval = 0;

    if(stringLength <= 3) {
        return numberString;
    }

    int firstPartLength = stringLength % 3;

    if(firstPartLength > 0) {
        for (int i = 0; i < firstPartLength; i++) {
            formattedNumber += numberString[i];
        }

        if (stringLength > 3) {
            formattedNumber += ",";
        }
    }

    for(int j = firstPartLength; j < stringLength; j++) {
        interval++;
        formattedNumber += numberString[j];
        if (interval == 3 && j != stringLength - 1) {
            interval = 0;
            formattedNumber += ",";
        }
    }

    return formattedNumber;
}

/// File handling
bool AccountManager::retrieve_file() {
    ifstream fileReader(ACCOUNTS_DATA_FILE);

    if(!fileReader.is_open()) {
        fileReader.close();

        ofstream createFile(ACCOUNTS_DATA_FILE);
        createFile.close();
        return false; // Return false when file was not retrieved
    }

    accList.last = 0;
    string firstLine;
    string secondLine;
    string thirdLine;
    string lastLine;
    while(getline(fileReader, firstLine)) {
        accList.info[accList.last].name = firstLine;

        getline(fileReader, secondLine);
        stringstream sl(secondLine);
        sl >> accList.info[accList.last].accountNumber >> accList.info[accList.last].pinCode;

        getline(fileReader, thirdLine);
        stringstream tl(thirdLine);
        tl >> accList.info[accList.last].dateOfBirth >> accList.info[accList.last].contactNumber >> accList.info[accList.last].initialDeposit;
        getline(fileReader,lastLine);
        accList.isBlocked[accList.last] = (lastLine == "0" ? false : true);

        accList.last++;
    }
    fileReader.close();

    return true; // Return true otherwise
}

void AccountManager::return_file() {
    ofstream fileWriter(ACCOUNTS_DATA_FILE);

    for(int i = 0; i < accList.last; i++) {
        fileWriter << accList.info[i].name << endl;
        fileWriter << accList.info[i].accountNumber << " " << accList.info[i].pinCode << endl;
        fileWriter << accList.info[i].dateOfBirth << " " << accList.info[i].contactNumber << " " << to_string(accList.info[i].initialDeposit) << endl;
        fileWriter << accList.isBlocked[i] << endl;
    }
    fileWriter.close();
}

bool AccountManager::initialize_files() {
    ifstream fileReader(TOTAL_DRIVES_FILE);

    if(!fileReader.is_open()) {
        // Initialize 'Total Drives'
        ofstream createTotalDrives(TOTAL_DRIVES_FILE);
        createTotalDrives << detect_total_drives() << endl;
        createTotalDrives.close();

        return true; // Return true when files were initialized
    }
    fileReader.close();

    return false; // Return false when files were already initialized
}

AccountList AccountManager::read_card() {
    int cardIndex = detect_total_drives() - 1;
    string filePath = DRIVE_PATHS[cardIndex] + CARD_FILE;

    string accountNumber;
    string pin;
    ifstream readCardFile(filePath);
    readCardFile >> accountNumber >> pin;
    readCardFile.close();

    AccountList account;
    account.info[0].accountNumber = "-1"; // Initialize as '-1' to indicate null
    account.info[0].pinCode = "-1";
    for(int i = 0; i < accList.last; i++) {
        if(accList.info[i].accountNumber == accountNumber && accList.info[i].pinCode == pin) {
            account.last = i;
            account.isBlocked[account.last] = accList.isBlocked[i];
            account.info[account.last] = accList.info[i];
            break;
        }
    }
    return account;
}

void AccountManager::setup_card() {
    int cardIndex = detect_total_drives() - 1;
    int precedingCardIndex = cardIndex - 1;
    string currentFilePath = DRIVE_PATHS[cardIndex] + CARD_FILE;

    if(!check_card()) {
        ofstream createFile(currentFilePath);
        createFile.close();
    }

    for(int i = precedingCardIndex; i != 0; i--) {
        string precedingFilePath = DRIVE_PATHS[i] + CARD_FILE;
        remove(precedingFilePath.c_str());
    }
}

void AccountManager::return_card(int index) {
    int cardIndex = detect_total_drives() - 1;
    string currentFilePath = DRIVE_PATHS[cardIndex] + CARD_FILE;

    ofstream fileWriter(currentFilePath);
    if(index == -1) {
        fileWriter << accList.info[accList.last - 1].accountNumber + " " + accList.info[accList.last - 1].pinCode;
    }
    else {
        fileWriter << accList.info[index].accountNumber + " " + accList.info[index].pinCode;
    }
    fileWriter.close();
}

bool AccountManager::check_card() {
    int cardIndex = detect_total_drives() - 1;
    string filePath = DRIVE_PATHS[cardIndex] + CARD_FILE;

    ifstream fileReader(filePath);
    if(!fileReader.is_open()) {
        fileReader.close();
        return false; // Return false if file does not exist
    }
    fileReader.close();
    return true; // Return true otherwise
}

int AccountManager::detect_total_drives() {
    int totalDrives = 0;
    string drivePaths[5] = {"C:\\", "D:\\", "E:\\", "F:\\", "G:\\"};

    for(int i = 5; i > 0; i--) {
        string drivePath = drivePaths[i - 1];

        if(GetDriveType(drivePath.c_str()) != DRIVE_NO_ROOT_DIR) {
            totalDrives++;
        }
    }
    return totalDrives;
}

void AccountManager::update_total_drives() {
    // Perform necessary changes
    int initialDriveCount;
    int totalDriveCount = detect_total_drives();

    ifstream readTotalDrives(TOTAL_DRIVES_FILE);
    readTotalDrives >> initialDriveCount;

    if(initialDriveCount > totalDriveCount) {
        readTotalDrives.close();
        // If total drives was reduced, initialDriveCount must be equal to totalDriveCount initially without the flash drive
        initialDriveCount = totalDriveCount;

        ofstream fileWriter(TOTAL_DRIVES_FILE);
        fileWriter << totalDriveCount << endl;
        fileWriter.close();
    }
    readTotalDrives.close();
}

string AccountManager::encrypt(string data) {
    string encryptedData;
    for(char c : data) {
        if(c == ' ') {
            encryptedData += '-';
        }
        else {
            int charPosition = -1; // Initialize to -1 for error handling
            for(int i = 0; i < ENCRYPTION_TABLE_LENGTH; i++) {
                if(c == ENCRYPTION_TABLE[i]) {
                    charPosition = i + ENCRYPTION_KEY;
                    break;
                }
            }

            if (charPosition != -1) {
                charPosition %= ENCRYPTION_TABLE_LENGTH;
                encryptedData += ENCRYPTION_TABLE[charPosition];
            }
            else {
                encryptedData += c;
            }
        }
    }
    return encryptedData;
}
