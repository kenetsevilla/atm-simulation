#ifndef REGISTRATION_H
#define REGISTRATION_H

#include <AccountManager.h>
#include <iostream>

class Registration : protected AccountManager
{
    private:
        bool areFilesInitialized;

        /// Helper methods
        bool isFull();      // Returns true if the list is full. Return false otherwise
        void date_formatter(int& i, int birthday[]);
        void clear_field(int xPosition, int yPosition, int eraseLength, int newXPos, int newYPos);

        /// Display animations
        void missing_card_animation(int timeout);
        void booting_atm_animation(int timeout);
        void reinsert_card_animation(int timeout);
        void atm_is_full_animation(int timeout);
        void play_animation(int playAnimation);
        void animate_loading_bar(int iterations, int xPoistion, int yPosition);

    public:
        Registration();

        /// Public methods
        AccountList menu();  // Returns the newly registered account. Defines a nullObject.last = -1 to return when list is full
        int find_atm_card(); // Detect whether the atm card(flashdrive) is inserted or not

        /// Getter method
        bool get_file_initialization_status();

        /// ADT list operations
        void register_account(AccountList newAccount);

        /// Debugging
        void display_registration_list();
};

#endif // REGISTRATION_H
