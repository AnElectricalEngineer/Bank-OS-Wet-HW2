#include "atm.h"
#include "account.h"

#include <fstream>
#include <map>

using namespace std;

extern map<int, account*> accounts;
extern ofstream logfile;

int openAccount(int accountNumber, int password, int initialAmount)
{
    // TODO probably lock/unlock here
    // Check if account with accountNumber already exists in accounts
    for(auto & account : accounts)
    {
        if(account.first == accountNumber)
        {
            return -1; // fail
        }
    }

    //TODO delete at end!
    auto newAccount = new account(accountNumber, password, initialAmount);
    accounts.insert({accountNumber, newAccount});
    return 0; // success
}

int deposit(int accountNumber, int password, int amount)
{
    auto account = accounts.find(accountNumber);

    // If account with id accountNumber doesn't exist in accounts
    if(account == accounts.end())
    {
        return -3;
    }

    if(account->second->checkPassword(password)) // Check if pass is correct
    {
        account->second->deposit(amount);
        return account->second->getBalance(); // successful
    }

    else
    {
        return -1; // failed
    }
}

int withdraw(int accountNumber, int password, int amount)
{
    auto account = accounts.find(accountNumber);

    // If account with id accountNumber doesn't exist in accounts
    if(account == accounts.end())
    {
        return -3;
    }

    // Check if pass is correct
    if(account->second->checkPassword(password))
    {
        int currentBalance = account->second->getBalance();
        if(currentBalance >= amount) // Check that balance >= amount
        {
            account->second->withdraw(amount);
            return account->second->getBalance(); // successful
        }
        else
        {
            return -2; // password correct but balance too low
        }
    }

    else
    {
        return -1; // password incorrect
    }
}

int checkBalance(int accountNumber, int password)
{
    auto account = accounts.find(accountNumber);

    // If account with id accountNumber doesn't exist in accounts
    if(account == accounts.end())
    {
        return -3;
    }

    // Check if pass is correct
    if(account->second->checkPassword(password))
    {
        return account->second->getBalance();
    }
    else
    {
        return -1; // wrong password
    }
}

int closeAccount(int accountNumber, int password)
{
    auto account = accounts.find(accountNumber);

    // If account with id accountNumber doesn't exist in accounts
    if(account == accounts.end())
    {
        return -3;
    }

    // Check if pass is correct
    if(account->second->checkPassword(password))
    {
        int balance = account->second->getBalance();
        delete account->second;
        accounts.erase(accountNumber);
        return balance;
    }

    // Wrong password
    else
    {
        return -1;
    }
}

TransferResult transfer(int sourceAccNumber, int sourcePass, int
targetAccNumber, int amount)
{
    TransferResult transferResult{};
    auto sourceAccount = accounts.find(sourceAccNumber);

    // If account with id sourceAccNumber doesn't exist in accounts
    if(sourceAccount == accounts.end())
    {
        transferResult.result = -3; // wrong source account number
        return transferResult;
    }

    // Wrong password
    if(!sourceAccount->second->checkPassword(sourcePass))
    {
        transferResult.result = -1; // Wrong password for target account
        return transferResult;
    }

    auto targetAccount = accounts.find(targetAccNumber);

    // If account with id targetAccNumber doesn't exist in accounts
    if(targetAccount == accounts.end())
    {
        transferResult.result = -4; // wrong target account number
        return transferResult;
    }

    int sourceAccountBalance = sourceAccount->second->getBalance();
    if(sourceAccountBalance < amount)
    {
        transferResult.result = -2; // Not enough money in source account
        return transferResult;
    }

    sourceAccount->second->withdraw(amount);
    targetAccount->second->deposit(amount);
    transferResult.result = amount;
    transferResult.sourceAccBalance = sourceAccount->second->getBalance();
    transferResult.targetAccBalance = targetAccount->second->getBalance();

    return transferResult;
}

// TODO figure out maybe problem with TEST_THIS_CASE.txt test case - extra
//  lines at end.
void* atmFunc(void* arg)
{
    auto atMinfo = (ATMinfo*)arg;
    ifstream ATMfile(atMinfo->ATMfile);
    int ATM_id = atMinfo->serialNumber;
    string newLine;

    if(!ATMfile.is_open())
    {
        //TODO check if works, check if it fails, reason is written to to errno
        perror("Couldn't open file: ");
        exit(-1);
    }

    // opened file
    while(getline(ATMfile, newLine))
    {
        char command = newLine.at(0); // The command

        vector<int> arguments{}; // holds all arguments
        string delimiter = " "; //TODO check if space is enough, or if need
        // other delimiters.
        size_t pos = 0;
        string token;
        newLine.erase(0, 2); // delete the command character and space after it

        // Add delimiter to end so loop doesn't skip last argument
        newLine.push_back(' ');

        // Get arguments from each line of user input file
        while ((pos = newLine.find(delimiter)) != string::npos) {
            token = newLine.substr(0, pos);
            arguments.push_back(std::stoi(token));
            newLine.erase(0, pos + delimiter.length());
        }

        // Open account
        if(command == 'O')
        {
            //TODO lock
            int openAccountTest = openAccount(
                    arguments.at(0),
                    arguments.at(1),
                    arguments.at(2));

            if (openAccountTest == 0) // Successfully opened account
            {
                logfile << ATM_id << ": New account id is " << arguments
                        .at(0) << " with password " << arguments.at(1) <<
                        " and initial balance " << arguments.at(2) << endl;
            }
            else // Failed to open account
            {
                logfile << "Error " << ATM_id << ": Your transaction "
                                                 "failed - account with "
                                                 "the same id exists" << endl;
            }
        }
        //TODO unlock

        // Deposit money
        else if(command == 'D')
        {
            // TODO lock
            int newBalance = deposit(arguments.at(0),
                                     arguments.at(1),
                                     arguments.at(2));

            // If account doesn't exist
            if(newBalance == -3)
            {
                logfile << "Error " << ATM_id << ": Your transaction "
                           "failed - account id " << arguments.at(0) <<
                                                 " does not exist" << endl;
            }

            // Wrong password
            else if(newBalance == -1)
            {
                logfile << "Error " << ATM_id << ": Your transaction " <<
                        "failed - password for account id "
                        << arguments.at(0) << " is incorrect" << endl;
            }

            // Successfully deposited money
            else
            {
                logfile << ATM_id << ": Account " << arguments.at(0) <<
                        " new balance is " << newBalance << " after " <<
                        arguments.at(2) << " $ was deposited" << endl;
            }

        }
        // TODO unlock

        // Withdraw money
        else if(command == 'W')
        {
            // TODO lock
            int newBalance = withdraw(arguments.at(0),
                                     arguments.at(1),
                                     arguments.at(2));

            // Wrong password
            if(newBalance == -1)
            {
                logfile << "Error " << ATM_id << ": Your transaction " <<
                        "failed - password for account id "
                        << arguments.at(0) << " is incorrect" << endl;
            }

            // Balance is lower than amount to withdraw
            else if(newBalance == -2)
            {
                logfile << "Error " << ATM_id << ": Your transaction " <<
                        "failed -  account id " << arguments.at(0) <<
                        " balance is lower than " << arguments.at(2) << endl;
            }

            // If account doesn't exist
            else if(newBalance == -3)
            {
                logfile << "Error " << ATM_id << ": Your transaction "
                                                 "failed - account id " << arguments.at(0) <<
                        " does not exist" << endl;
            }

            // Successfully deposited money
            else
            {
                logfile << ATM_id << ": Account " << arguments.at(0) <<
                        " new balance is " << newBalance << " after " <<
                        arguments.at(2) << " $ was withdrew" << endl;
            }
        }
        // TODO unlock

        else if(command == 'B')
        {
            // TODO lock
            int currentBalance = checkBalance(arguments.at(0), arguments.at(1));

            // Wrong password
            if(currentBalance == -1)
            {
                logfile << "Error " << ATM_id << ": Your transaction " <<
                        "failed - password for account id "
                        << arguments.at(0) << " is incorrect" << endl;
            }

            // If account doesn't exist
            else if(currentBalance == -3)
            {
                logfile << "Error " << ATM_id << ": Your transaction "
                           "failed - account id " << arguments.at(0) <<
                           " does not exist" << endl;
            }

            // Print balance
            else
            {
                logfile << ATM_id << ": Account " << arguments.at(0) <<
                        " balance is " << currentBalance << endl;
            }
        }
        //TODO unlock

        // Close account
        else if(command == 'Q')
        {
            // TODO lock
            int endingBalance = closeAccount(arguments.at(0), arguments.at(1));

            // Wrong password
            if(endingBalance == -1)
            {
                logfile << "Error " << ATM_id << ": Your transaction " <<
                        "failed - password for account id "
                        << arguments.at(0) << " is incorrect" << endl;
            }

            // If account doesn't exist
            else if(endingBalance == -3)
            {
                logfile << "Error " << ATM_id << ": Your transaction "
                           "failed - account id " << arguments.at(0) <<
                           " does not exist" << endl;
            }

            // Account successfully closed
            else
            {
                logfile << ATM_id << ": Account " << arguments.at(0) <<
                        " is now closed. Balance was " << endingBalance << endl;
            }
        }
        // TODO unlock

        // Transfer money
        else if(command == 'T')
        {
            // TODO lock

            TransferResult result = transfer(arguments.at(0), arguments.at(1),
                                  arguments.at(2), arguments.at(3));


            // Wrong password for source account
            if(result.result == -1)
            {
                logfile << "Error " << ATM_id << ": Your transaction " <<
                        "failed - password for account id "
                        << arguments.at(0) << " is incorrect" << endl;
            }

            // Amount to transfer is more than balance in source account
            else if(result.result == -2)
            {
                logfile << "Error " << ATM_id << ": Your transaction " <<
                        "failed -  account id " << arguments.at(0) <<
                        " balance is lower than " << arguments.at(3) << endl;
            }

            // If source account doesn't exist
            else if(result.result == -3)
            {
                logfile << "Error " << ATM_id << ": Your transaction "
                                                 "failed - account id " <<
                                                 arguments.at(0) <<
                                                 " does not exist" << endl;
            }

            // If source account doesn't exist
            else if(result.result == -4)
            {
                logfile << "Error " << ATM_id << ": Your transaction "
                                                 "failed - account id " <<
                                                arguments.at(2) <<
                                                " does not exist" << endl;
            }

            // If transfer succeeded
            else
            {
                int sourceAccBalance = result.sourceAccBalance;
                int targetAccBalance = result.targetAccBalance;
                logfile << ATM_id << ": Transfer " << arguments.at(3) << " from"
                                      " account " << arguments.at(0) << " to "
                                      "account " << arguments.at(2) << " new"
                                      " account balance is " <<
                                      sourceAccBalance << " new target account "
                                      << "balance is " << targetAccBalance <<
                                      endl;
            }
        }
        // TODO unlock
    }

    ATMfile.close(); //TODO check if need to check if succeeded

    return nullptr; //TODO check
}
