#include <iostream>
#include <conio.h>

#include <Registration.h>
#include <Transaction.h>

using namespace std;

int main()
{
    Registration registration;
    Transaction transaction;

    // If files were initialized, relaunch program
    if(registration.get_file_initialization_status()) {
        return 1;
    }

    int status = registration.find_atm_card();
    if(status == 1) {
        registration.register_account(registration.menu());
        transaction.read_file();
    }
    else if(status == -1 || status == -2) {
        return 1; // If status is either -1 or -2
    }

    while(!transaction.get_is_transaction_complete()) {
        switch(transaction.menu() - '0') {
        case 1:
            transaction.balance_inquiry();
            break;

        case 2:
            transaction.change_pin();
            break;

        case 3:
            transaction.fund_transfer();
            break;

        case 4:
            transaction.withdraw();
            break;

        case 5:
            transaction.deposit();
            break;

            default:
            break;
        }
    }

    cout << endl;
    cout << "Press any key to continue...";
    getch();
    return 0;
}
