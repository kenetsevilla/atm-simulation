#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H


#include <iostream>

#define MAX 100

// Ascii code
#define ASCII_ZERO 48
#define ASCII_ONE 49
#define ASCII_TWO 50
#define ASCII_THREE 51
#define ASCII_FOUR 52
#define ASCII_FIVE 53
#define ASCII_SIX 54
#define ASCII_SEVEN 55
#define ASCII_EIGHT 56
#define ASCII_NINE 57
#define ASCII_BACKSPACE 8
#define ASCII_ENTER 13
#define ASCII_ESCAPE 27

// Encryption
#define ENCRYPTION_TABLE "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define ENCRYPTION_TABLE_LENGTH 62
#define ENCRYPTION_KEY 5

// File names
#define CARD_FILE "Card File.txt"                   // ATM Card file
#define TOTAL_DRIVES_FILE "Total Drives.txt"        // File for counting total initial drives
#define ACCOUNTS_DATA_FILE "Accounts Data File.txt" // File for storing the data of every account

// File paths
const std::string DRIVE_PATHS[] = {"C:\\", "D:\\", "E:\\", "F:\\", "G:\\"};

struct AccountInfo {
    std::string name;
    std::string accountNumber;  // 5 digits
    std::string pinCode;        // 6 digits
    std::string dateOfBirth;
    std::string contactNumber;
    int initialDeposit;         // Minimum of 5000 initial deposit
};

struct AccountList {
    AccountInfo info[MAX];
    bool isBlocked[MAX];
    int last;
};

class AccountManager
{
    protected:
        AccountList accList;

        /// Helper methods
        AccountList get_account(std::string accountNumber);                 // Returns the data of an account
        bool check_account(std::string accountNumber);                      // Checks if an account exists on the list
        bool check_account(std::string accountNumber, std::string pinCode); // Checks if an account exists on the list
        std::string format_as_string(int characters[], int expectedStringLength, int addMorphemesInterval = 0, int stopMorphemesAtIndex = 0, std::string affix = "", std::string prefix = "", std::string suffix = "");
        std::string format_number(int number);

        /// File handling
        bool retrieve_file();               // Retrieves the file with all the registered accounts. Returns false when it fails to retrieve the files
        void return_file();                 // Updates the file that holds all the registered accounts
        bool initialize_files();            // Creates all necessary files needed to operate

        AccountList read_card();            // Returns the account information of the card
        void setup_card();                  // Creates the necessary file inside the card
        void return_card(int index = -1);   // Returns the account number and pin code inside the card
        bool check_card();                  // Returns true when a card including its file has been found

        int detect_total_drives();
        void update_total_drives();

        std::string encrypt(std::string data);

    public:
        AccountManager();
};

#endif // ACCOUNTMANAGER_H
