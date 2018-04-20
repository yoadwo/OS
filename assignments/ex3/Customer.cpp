#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include "Item.cpp"

using namespace std;

typedef struct Customer{
    int _customerId;
    vector <Item> items;

     Customer(int custId) 
    : _customerId(custId)
    {
    }

}Customer;

