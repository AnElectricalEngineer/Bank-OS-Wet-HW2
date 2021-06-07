#include "atm.h"
#include "bank.h"

#include <fstream>
#include <map>
#include <unistd.h>

#define ATM_OP_TIME 1 // amount of time in SECONDS that each operation takes
#define ATM_SLEEP_TIME 100000 // amount of time in MICROSECONDS that ATM sleeps

using namespace std;

extern accounts Accounts;
extern Logfile logfile;

int openAccount(string accountNumber, string password, int
initialAmount)
{
    // Check if account with accountNumber already exists in accounts
    Accounts.enterWriter();
    sleep(ATM_OP_TIME);
    // Check if account with same account number already exists
    if(Accounts._accounts.find(accountNumber) != Accounts._accounts.end())
    {
        Accounts.exitWriter();
        return -1; // fail
    }

    auto newAccount = new account(password, initialAmount);
    Accounts._accounts.insert({accountNumber, newAccount});
    Accounts.exitWriter();
    return 0; // success
}

int deposit(string accountNumber, string password, int amount)
{
    Accounts.enterReader();
    auto account = Accounts._accounts.find(accountNumber);

    // If account with id accountNumber doesn't exist in accounts
    if(account == Accounts._accounts.end())
    {
        sleep(ATM_OP_TIME);
        Accounts.exitReader();
        return -3;
    }

    else if(account->second->checkPassword(password)) // Check if pass is correct
    {
        account->second->enterWriter();
        sleep(ATM_OP_TIME);
        account->second->deposit(amount);
        int balance = account->second->getBalance();
        account->second->exitWriter();
        Accounts.exitReader();
        return balance; // successful
    }

    else
    {
        sleep(ATM_OP_TIME);
        Accounts.exitReader();
        return -1; // failed
    }
}

int withdraw(string accountNumber, string password, int amount)
{
    Accounts.enterReader();
    auto account = Accounts._accounts.find(accountNumber);

    // If account with id accountNumber doesn't exist in accounts
    if(account == Accounts._accounts.end())
    {
        sleep(ATM_OP_TIME);
        Accounts.exitReader();
        return -3;
    }

    // Check if pass is correct
    else if(account->second->checkPassword(password))
    {
        account->second->enterWriter();
        sleep(ATM_OP_TIME);
        int currentBalance = account->second->getBalance();
        if(currentBalance >= amount) // Check that balance >= amount
        {
            account->second->withdraw(amount);
            int balance = account->second->getBalance();
            account->second->exitWriter();
            Accounts.exitReader();
            return balance; // successful
        }
        else
        {
            account->second->exitWriter();
            Accounts.exitReader();
            return -2; // password correct but balance too low
        }
    }

    else
    {
        sleep(ATM_OP_TIME);
        Accounts.exitReader();
        return -1; // password incorrect
    }
}

int checkBalance(string accountNumber, string password)
{
    Accounts.enterReader();
    auto account = Accounts._accounts.find(accountNumber);

    // If account with id accountNumber doesn't exist in accounts
    if(account == Accounts._accounts.end())
    {
        sleep(ATM_OP_TIME);
        Accounts.exitReader();
        return -3;
    }

    // Check if pass is correct
    else if(account->second->checkPassword(password))
    {
        account->second->enterReader();
        sleep(ATM_OP_TIME);
        int balance = account->second->getBalance();
        account->second->exitReader();
        Accounts.exitReader();
        return balance;
    }
    else
    {
        sleep(ATM_OP_TIME);
        Accounts.exitReader();
        return -1; // wrong password
    }
}

int closeAccount(string accountNumber, string password)
{
    Accounts.enterWriter();
    sleep(ATM_OP_TIME);
    auto account = Accounts._accounts.find(accountNumber);

    // If account with id accountNumber doesn't exist in accounts
    if(account == Accounts._accounts.end())
    {
        Accounts.exitWriter();
        return -3;
    }

    // Check if pass is correct
    if(account->second->checkPassword(password))
    {
        int balance = account->second->getBalance();
        delete account->second;
        Accounts._accounts.erase(accountNumber);
        Accounts.exitWriter();
        return balance;
    }

    // Wrong password
    else
    {
        Accounts.exitWriter();
        return -1;
    }
}

TransferResult transfer(string sourceAccNumber, string sourcePass, string
targetAccNumber, int amount)
{
    TransferResult transferResult{};
    Accounts.enterReader();
    auto sourceAccount = Accounts._accounts.find(sourceAccNumber);

    // If account with id sourceAccNumber doesn't exist in accounts
    if(sourceAccount == Accounts._accounts.end())
    {
        sleep(ATM_OP_TIME);
        transferResult.result = -3; // wrong source account number
        Accounts.exitReader();
        return transferResult;
    }

    // Wrong password
    else if(!sourceAccount->second->checkPassword(sourcePass))
    {
        sleep(ATM_OP_TIME);
        transferResult.result = -1; // Wrong password for target account
        Accounts.exitReader();
        return transferResult;
    }

    auto targetAccount = Accounts._accounts.find(targetAccNumber);

    // If account with id targetAccNumber doesn't exist in accounts
    if(targetAccount == Accounts._accounts.end())
    {
        sleep(ATM_OP_TIME);
        transferResult.result = -4; // wrong target account number
        Accounts.exitReader();
        return transferResult;
    }

    string minAccNumber = sourceAccNumber < targetAccNumber ? sourceAccNumber :
            targetAccNumber;

    if(minAccNumber == sourceAccNumber)
    {
        sourceAccount->second->enterWriter();
        targetAccount->second->enterWriter();
    }
    else
    {
        targetAccount->second->enterWriter();
        sourceAccount->second->enterWriter();
    }

    sleep(ATM_OP_TIME);

    int sourceAccountBalance = sourceAccount->second->getBalance();
    if(sourceAccountBalance < amount)
    {
        transferResult.result = -2; // Not enough money in source account
        sourceAccount->second->exitWriter();
        targetAccount->second->exitWriter();
        Accounts.exitReader();
        return transferResult;
    }

    sourceAccount->second->withdraw(amount);
    targetAccount->second->deposit(amount);
    transferResult.result = amount;
    transferResult.sourceAccBalance = sourceAccount->second->getBalance();
    transferResult.targetAccBalance = targetAccount->second->getBalance();

    sourceAccount->second->exitWriter();
    targetAccount->second->exitWriter();
    Accounts.exitReader();
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
        usleep(ATM_SLEEP_TIME);

        char command = newLine.at(0); // The command

        // TODO check if need to support blank lines in input ATM files.
        //  currently fails.
        vector<string> arguments{}; // holds string representation of arguments
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
            arguments.push_back(token);
            //strArguments.push_back(token); //TODO maybe use
            newLine.erase(0, pos + delimiter.length());
        }

        // Open account
        if(command == 'O')
        {
            int initialAmount = std::stoi(arguments.at(2));
            int openAccountTest = openAccount(
                    arguments.at(0),
                    arguments.at(1),
                    initialAmount);

            logfile.enterWriter();
            if (openAccountTest == 0) // Successfully opened account
            {
                logfile._logfile << ATM_id << ": New account id is " <<
                arguments.at(0) << " with password " << arguments.at(1) <<
                        " and initial balance " << initialAmount << endl;
            }
            else // Failed to open account
            {
                logfile._logfile << "Error " << ATM_id << ": Your transaction "
                                                 "failed - account with "
                                                 "the same id exists" << endl;
            }
            logfile.exitWriter();
        }

        // Deposit money
        else if(command == 'D')
        {
            int amount = std::stoi(arguments.at(2));
            int newBalance = deposit(arguments.at(0),
                                     arguments.at(1),
                                     amount);

            logfile.enterWriter();
            // If account doesn't exist
            if(newBalance == -3)
            {
                logfile._logfile << "Error " << ATM_id << ": Your transaction "
                           "failed - account id " << arguments.at(0) <<
                                                 " does not exist" << endl;
            }

            // Wrong password
            else if(newBalance == -1)
            {
                logfile._logfile << "Error " << ATM_id << ": Your transaction " <<
                        "failed - password for account id "
                        << arguments.at(0) << " is incorrect" << endl;
            }

            // Successfully deposited money
            else
            {
                logfile._logfile << ATM_id << ": Account " << arguments.at(0) <<
                        " new balance is " << newBalance << " after " <<
                        amount << " $ was deposited" << endl;
            }
            logfile.exitWriter();
        }

        // Withdraw money
        else if(command == 'W')
        {
            int amount = std::stoi(arguments.at(2));
            int newBalance = withdraw(arguments.at(0),
                                     arguments.at(1),
                                     amount);

            logfile.enterWriter();
            // Wrong password
            if(newBalance == -1)
            {
                logfile._logfile << "Error " << ATM_id << ": Your transaction " <<
                        "failed - password for account id "
                        << arguments.at(0) << " is incorrect" << endl;
            }

            // Balance is lower than amount to withdraw
            else if(newBalance == -2)
            {
                logfile._logfile << "Error " << ATM_id << ": Your transaction " <<
                        "failed -  account id " << arguments.at(0) <<
                        " balance is lower than " << amount << endl;
            }

            // If account doesn't exist
            else if(newBalance == -3)
            {
                logfile._logfile << "Error " << ATM_id << ": Your transaction "
                                                 "failed - account id " << arguments.at(0) <<
                                                " does not exist" << endl;
            }

            // Successfully deposited money
            else
            {
                logfile._logfile << ATM_id << ": Account " << arguments.at(0) <<
                        " new balance is " << newBalance << " after " <<
                        amount << " $ was withdrew" << endl;
            }
            logfile.exitWriter();
        }

        else if(command == 'B')
        {
            int currentBalance = checkBalance(arguments.at(0), arguments.at(1));

            logfile.enterWriter();
            // Wrong password
            if(currentBalance == -1)
            {
                logfile._logfile << "Error " << ATM_id << ": Your transaction " <<
                        "failed - password for account id "
                        << arguments.at(0) << " is incorrect" << endl;
            }

            // If account doesn't exist
            else if(currentBalance == -3)
            {
                logfile._logfile << "Error " << ATM_id << ": Your transaction "
                           "failed - account id " << arguments.at(0) <<
                           " does not exist" << endl;
            }

            // Print balance
            else
            {
                logfile._logfile << ATM_id << ": Account " << arguments.at(0) <<
                        " balance is " << currentBalance << endl;
            }
            logfile.exitWriter();
        }

        // Close account
        else if(command == 'Q')
        {
            int endingBalance = closeAccount(arguments.at(0), arguments.at(1));

            logfile.enterWriter();
            // Wrong password
            if(endingBalance == -1)
            {
                logfile._logfile << "Error " << ATM_id << ": Your transaction " <<
                        "failed - password for account id "
                        << arguments.at(0) << " is incorrect" << endl;
            }

            // If account doesn't exist
            else if(endingBalance == -3)
            {
                logfile._logfile << "Error " << ATM_id << ": Your transaction "
                           "failed - account id " << arguments.at(0) <<
                           " does not exist" << endl;
            }

            // Account successfully closed
            else
            {
                logfile._logfile << ATM_id << ": Account " << arguments.at(0) <<
                        " is now closed. Balance was " << endingBalance << endl;
            }
            logfile.exitWriter();
        }

        // Transfer money
        else if(command == 'T')
        {
            int amountToTransfer = std::stoi(arguments.at(3));
            TransferResult result = transfer(arguments.at(0), arguments.at(1),
                                  arguments.at(2), amountToTransfer);

            logfile.enterWriter();
            // Wrong password for source account
            if(result.result == -1)
            {
                logfile._logfile << "Error " << ATM_id << ": Your transaction " <<
                        "failed - password for account id "
                        << arguments.at(0) << " is incorrect" << endl;
            }

            // Amount to transfer is more than balance in source account
            else if(result.result == -2)
            {
                logfile._logfile << "Error " << ATM_id << ": Your transaction " <<
                        "failed -  account id " << arguments.at(0) <<
                        " balance is lower than " << amountToTransfer << endl;
            }

            // If source account doesn't exist
            else if(result.result == -3)
            {
                logfile._logfile << "Error " << ATM_id << ": Your transaction "
                                                 "failed - account id " <<
                                                 arguments.at(0) <<
                                                 " does not exist" << endl;
            }

            // If source account doesn't exist
            else if(result.result == -4)
            {
                logfile._logfile << "Error " << ATM_id << ": Your transaction "
                                                 "failed - account id " <<
                                                arguments.at(2) <<
                                                " does not exist" << endl;
            }

            // If transfer succeeded
            else
            {
                int sourceAccBalance = result.sourceAccBalance;
                int targetAccBalance = result.targetAccBalance;
                logfile._logfile << ATM_id << ": Transfer " <<
                                        amountToTransfer << " from"
                                      " account " << arguments.at(0) << " to "
                                      "account " << arguments.at(2) << " new"
                                      " account balance is " <<
                                      sourceAccBalance << " new target account "
                                      << "balance is " << targetAccBalance <<
                                      endl;
            }
            logfile.exitWriter();
        }
    }

    ATMfile.close(); //TODO check if need to check if succeeded

    pthread_exit(nullptr); //TODO check
}
