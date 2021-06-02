#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <pthread.h>

class account{
private:
    const int _accountNumber;
    const int _password;
    int _balance;
    //TODO I think (lecture 5 slide 17), give each account a read and write
    // mutex and read count. Also add methods to get locks and get and set
    // read count. Then in atms functions, do just like slide 17. Dont forget
    // to add initialization to mutexes in constructor, initialize to unlocked.

    pthread_mutex_t _readLock, _writeLock;
    int _readerCnt;

public:
    account(int accountNumber, int password, int balance);
    void deposit(int amount);
    void withdraw(int amount);
    int getBalance() const;
    bool checkPassword(int password) const;
    void enterReader();
    void exitReader();
    void enterWriter();
    void exitWriter();
};


#endif //ACCOUNT_H
