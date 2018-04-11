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
    cout << "[" << _id << "] " << _name << ", $" << _price 
    << ", total Ordered " << _totalOrdered << "\n";     
 
}
 
bool Item :: equals(const Item &otherItem)
{
    return true;
}