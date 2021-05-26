#ifndef _ATM_H
#define _ATM_H

typedef struct _ATMinfo
{
    int serialNumber; //TODO check if int or something else, check if starts
    // from 1, here we assumed starts from 1
    int fileDescriptor;
} ATMinfo, *pATMinfo;

int openAccount(int accountNumber, int password, int initialAmount);
int deposit(int accountNumber, int password, int amount);
int withdraw(int accountNumber, int password, int amount);
int checkBalance(int accountNumber, int password);
int closeAccount(int accountNumber, int password);
int transfer(int sourceAccNumber, int sourcePass, int targetAccNumber, int
amount);
void* atmFunc(void* arg);

#endif //_ATM_H
