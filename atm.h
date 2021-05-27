#ifndef _ATM_H
#define _ATM_H

#include <string>
#include <vector>

using namespace std;

typedef struct
{
    int serialNumber; //TODO check if int or something else, check if starts
    // from 1, here we assumed starts from 1
    string ATMfile;
} ATMinfo, *pATMinfo;

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
//******************************************************************************
int deposit(int accountNumber, int password, int amount);

// TODO either this func or account::withdraw() needs to ensure
//  that cannot withdraw more money than balance I think!
int withdraw(int accountNumber, int password, int amount);
int checkBalance(int accountNumber, int password);
int closeAccount(int accountNumber, int password);
//TODO for transfer, look at tirgul 5 slide 33 + extra questions at the end,
// page 4, make sure that locks are always locked in same order...use min()
// on account numbers maybe.
int transfer(int sourceAccNumber, int sourcePass, int targetAccNumber, int
amount);
void* atmFunc(void* arg);


#endif //_ATM_H
