#include <string>
#include <iostream>
#include <iomanip>

using namespace std;
 




typedef struct Item{

    int _id, _price, _totalOrdered;
    string _name;

    Item(int id, int price, string name) : _id(id), _price (price), _totalOrdered(0)
    {
    
        _name = name.substr(0,15);
    }
 
    int  getId() 
    {
        return _id; 
    }	
	
    int  getPrice(){
        return _price;
    }
    int  getTotalOrdered(){
        return _totalOrdered;
    }
    string  getName(){
        return _name;
    }
    void  print()
    {
     cout  
     << left << "[" << setw(2) << _id << "] "
     << left << setw(20) <<_name
      << left << setw(8) << _price 
        << left << setw(4) << _totalOrdered  << "\n";     
 
    }
 
    bool equals(const Item &otherItem)
    {
        return true;
    }

}Item;