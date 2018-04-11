
#include <string>
#include <iostream>
#include <iomanip>
#include "Order.h"
using namespace std;
 

Order :: Order(int customerId, int itemId, int amount) 
  : _customerId(customerId), _itemId (itemId), _amount(amount), _done(true)
{
}
 
int Order :: getCustomerId() 
{
    return _customerId; 
}	
	
int Order :: getItemId(){
    return _itemId;
}
int Order :: getAmount(){
    return _amount;
}
bool Order :: isDone(){
    return _done;
}
void Order :: print()
{
    cout 
    << "customerId: " << _customerId 
    << ", itemId: " << _itemId
    << ", amount: " << _amount
    << ", Done: " << _done
    << "\n";
}
 
bool Order :: equals(const Order &otherOrder)
{
    return true;
}