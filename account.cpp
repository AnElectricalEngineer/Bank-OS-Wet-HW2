#include "account.h"

account::account(int accountNumber, int password, int balance):
_accountNumber(accountNumber), _password(password), _balance(balance),
_readerCnt(0)
{
    // TODO check if need to check if succeeded like syscall
    // TODO check if need to destroy in destructor
    pthread_mutex_init(&_readLock, nullptr);
    pthread_mutex_init(&_writeLock, nullptr);
}

void account::deposit(int amount)
{
    _balance += amount;
}

void account::withdraw(int amount)
{
    _balance -= amount;
}

int account::getBalance() const
{
    return _balance;
}

bool account::checkPassword(int password) const
{
    return (password == this->_password);
}

int account::getPassword() const
{
    return _password;
}

void account::enterReader()
{
    pthread_mutex_lock(&_readLock); // TODO check if sys call
    _readerCnt++;
    if(_readerCnt == 1)
    {
        pthread_mutex_lock(&_writeLock); // TODO check if sys call
    }
    pthread_mutex_unlock(&_readLock); // TODO check if sys call
}

void account::exitReader()
{
    pthread_mutex_lock(&_readLock); // TODO check if sys call
    _readerCnt--;
    if(_readerCnt == 0)
    {
        pthread_mutex_unlock(&_writeLock); // TODO check if sys call
    }
    pthread_mutex_unlock(&_readLock); // TODO check if sys call
}

void account::enterWriter()
{
    pthread_mutex_lock(&_writeLock); // TODO check if sys call
}

void account::exitWriter()
{
    pthread_mutex_unlock(&_writeLock); // TODO check if sys call
}