#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <AccountManager.h>
#include <iostream>

class Transaction : protected AccountManager
{
    private:
        AccountList loggedInAccount;
        bool isPinVerified;
        bool isTransactionComplete;

        /// File handling
        void update_account_info(); // Updates any changes made with the logged in account
        void update_card();

        /// Preparation methods
        void initialize_logged_in_account();
        bool verify_pin();          // Return false when account is blocked. Return true when verified

        /// Menus
        int another_transaction_menu();
        int withdraw_menu();
        int deposit_menu();
        int fund_transfer_menu();
        int balance_inquiry_menu();
        std::string change_pin_menu();

        /// Animation display
        void remove_card_animation(int timeout);
        void transaction_complete_animation(int timeout, bool moneyOut = false);
        void animate_loading_bar(int iterations, int xPoistion, int yPosition, std::string message);
        void animate_processed_transaction(int timeout);

    public:
        Transaction();

        /// Public methods
        int menu();
        void read_file(); // Reads the file again after registration;

        /// Transaction methods
        void withdraw();
        void deposit();
        void fund_transfer();
        void balance_inquiry();
        void change_pin();

        /// Getter method
        bool get_is_transaction_complete();

        /// Debugging
        void display_transaction_list();
};

#endif // TRANSACTION_H
