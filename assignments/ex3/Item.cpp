#include <string>
#include <iostream>
#include <iomanip>
#include "Item.h"
using namespace std;
 

Item :: Item(int id, int price, string name) 
  : _id(id), _price (price), _totalOrdered(0)
{
    
    _name = name.substr(0,15);
}
 
int Item :: getId() 
{
    return _id; 
}	
	
int Item :: getPrice(){
    return _price;
}
int Item :: getTotalOrdered(){
    return _totalOrdered;
}
string Item :: getName(){
    return _name;
}
void Item :: print()
{
    cout  
    << left << "[" << setw(2) << _id << "] "
    << left << setw(20) <<_name
    << left << setw(8) << _price 
    << left << setw(4) << _totalOrdered  << "\n";     
 
}
 
bool Item :: equals(const Item &otherItem)
{
    return true;
}