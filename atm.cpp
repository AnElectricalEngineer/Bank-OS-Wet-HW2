#include "atm.h"
#include "account.h"

#include <fstream>
#include <map>

using namespace std;

extern map<int, account*> accounts;
extern ofstream logfile;

int openAccount(int accountNumber, int password, int initialAmount)
{
    // Check if account with accountNumber already exists in accounts
    for(auto it = accounts.begin(); it != accounts.end(); it++)
    {
        if(it->first == accountNumber)
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
    //TODO check if possible that accountNumber is of account that doesn't exist
    auto account = accounts.find(accountNumber);
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

        switch(command)
        {
            // Open account
            case 'O':
                //TODO lock
                int openAccountTest = openAccount(
                        arguments.at(0),
                        arguments.at(1),
                        arguments.at(2));

                if(openAccountTest == 0) // Successfully opened account
                {
                    logfile << ATM_id << ": New account id is " << arguments
                    .at(0) << " with password " << arguments.at(1) <<
                    " and initial balance " << arguments.at(2) << endl;
                }
                else // Failed to open account
                {
                    logfile << "Error " << ATM_id << ": Your transaction "
                                                     "failed - account with "
                                                     "the same id exists"
                                                     << endl;
                }
                break;
                //TODO unlock

            // Deposit money
            case 'D':
                // TODO lock
                int newBalance = deposit(arguments.at(0),
                                         arguments.at(1),
                                         arguments.at(2));

                if(newBalance != -1) // Successfully deposited money
                {
                    logfile << ATM_id << ": Account " << arguments.at(0) <<
                    " new balance is " << newBalance << " after " <<
                    arguments.at(2) << " $ was deposited" << endl;
                }

                else // Failed depositing money
                {
                    logfile << "Error " << ATM_id << ": Your transaction " <<
                    "failed - password for account id " << arguments.at(0) <<
                    " is incorrect" << endl;
                }
                break;
                // TODO unlock

            default:
                break;
        }
    }

    ATMfile.close(); //TODO check if need to check if succeeded

    return nullptr; //TODO check
}
