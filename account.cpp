#include "account.h"

account::account(int accountNumber, int password, int balance):
_accountNumber(accountNumber), _password(password), _balance(balance){}

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
