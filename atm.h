#ifndef _ATM_H
#define _ATM_H

#include <string>
#include <vector>

using namespace std;

typedef struct
{
    const int serialNumber; //TODO check if int or something else, check if
    // starts from 1, here we assumed starts from 1
    const string ATMfile;
} ATMinfo, *pATMinfo;

// Object that holds information about transfer
typedef struct
{
    int result;
    int sourceAccBalance;
    int targetAccBalance;
} TransferResult, *pTransferResult;

//TODO for open account/close account: I think need to lock ENTIRE ACCOUNT
// data structure or maybe each individual lock, because while we are
// modifying data structure...critical section.

//******************************************************************************
// Creates a new account and adds it to the collection of accounts.
// Returns: 0 if succeeded
//          -1 if failed
//******************************************************************************
int openAccount(int accountNumber, int password, int initialAmount);

//******************************************************************************
// Deposits amount into the account with the number accountNumber
// Returns: new account balance if succeeded
//          -1 if failed
//          -3 if account doesn't exist
//******************************************************************************
int deposit(int accountNumber, int password, int amount);

// TODO either this func or account::withdraw() needs to ensure
//  that cannot withdraw more money than balance I think!
//******************************************************************************
// Withdraws amount from the account with the number accountNumber
// Returns: new account balance if succeeded
//          -1 if wrong password entered
//          -2 if balance < amount to withdraw
//          -3 if account doesn't exist
//******************************************************************************
int withdraw(int accountNumber, int password, int amount);

//******************************************************************************
// Checks the balance of the account with the number accountNumber
// Returns:  account balance if password is correct
//          -1 if wrong password entered
//          -3 if account doesn't exist
//******************************************************************************
int checkBalance(int accountNumber, int password);

//******************************************************************************
// Closes the account with the number accountNumber
// Returns:  ending account balance if password is correct
//          -1 if wrong password entered
//          -3 if account doesn't exist
//******************************************************************************
int closeAccount(int accountNumber, int password);

//TODO for transfer, look at tirgul 5 slide 33 + extra questions at the end,
// page 4, make sure that locks are always locked in same order...use min()
// on account numbers maybe.

//******************************************************************************
// Transfers amount from the account with the number sourceAccNumber to the
// account with the number targetAccNumber
// Returns: struct containing new account balances if succeeded and result:
//           non-negative number if succeeded
//          -1 if wrong source account password entered
//          -2 if source account balance < amount to transfer
//          -3 if source account doesn't exist
//          -4 if target account doesn't exist
//******************************************************************************
TransferResult transfer(int sourceAccNumber, int sourcePass, int
targetAccNumber, int
amount);

void* atmFunc(void* arg);


#endif //_ATM_H
