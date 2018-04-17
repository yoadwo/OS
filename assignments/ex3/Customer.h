#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <vector>
#include "Item.h"


using namespace std;

class Customer
{
private:    
    int _customerId;
    vector <Item> items;
public:

    Customer(int custId);

     chooseItem(vector <Item> items);






};
#endif