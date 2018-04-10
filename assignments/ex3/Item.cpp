#include <iostream>
#include <iomanip>
#include "Item.h"
using namespace std;
 
Item :: Item(int id, int price, int totalOrdered = 0) 
  : _id(id), _price (price), _totalOrdered(totalOrdered)
{}
 
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
char* Item :: getName(){
    return _name;
}
void Item :: print()
{
     
 
}
 
bool Item :: equals(const Item &otherItem)
{
    return true;
}