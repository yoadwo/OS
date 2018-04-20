
#include <string>
#include <iostream>
#include <iomanip>
#include "Order.h"
using namespace std;
 
struct foo {
  int bar;
  foo() : bar(3) {}   //look, a constructor
  int getBar() 
  { 
    return bar; 
  }
};

typedef struct Order{
    int _customerId, _itemId, _amount;
    bool _done;

    Order(int customerId, int itemId, int amount) 
    : _customerId(customerId), _itemId (itemId), _amount(amount), _done(true)
    {}

    int getCustomerId() {
    return _customerId; 
    }

    int getItemId(){
        return _itemId;
    }
    int getAmount(){
        return _amount;
    }
    bool isDone(){
        return _done;
    }
    void  print(){
        cout 
        << "customerId: " << _customerId 
        << ", itemId: " << _itemId
        << ", amount: " << _amount
        << ", Done: " << _done
        << "\n";
    }

}  Order;

// Order :: Order(int customerId, int itemId, int amount) 
//   : _customerId(customerId), _itemId (itemId), _amount(amount), _done(true)
// {
// }
 
// int Order :: getCustomerId() 
// {
//     return _customerId; 
// }	
	
// int Order :: getItemId(){
//     return _itemId;
// }
// int Order :: getAmount(){
//     return _amount;
// }
// bool Order :: isDone(){
//     return _done;
// }
// void Order :: print()
// {
//     cout 
//     << "customerId: " << _customerId 
//     << ", itemId: " << _itemId
//     << ", amount: " << _amount
//     << ", Done: " << _done
//     << "\n";
// }
 
// bool Order :: equals(const Order &otherOrder)
// {
//     return true;
// }