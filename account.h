#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <pthread.h>

class account{
private:
    const int _accountNumber;
    const int _password;
    int _balance;
    pthread_mutex_t _readLock, _writeLock;
    int _readerCnt;

public:
    account(int accountNumber, int password, int balance);
    void deposit(int amount);
    void withdraw(int amount);
    int getBalance() const;
    bool checkPassword(int password) const;
    int getPassword() const;
    void enterReader();
    void exitReader();
    void enterWriter();
    void exitWriter();
};


#endif //ACCOUNT_H
