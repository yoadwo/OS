#ifndef ITEM_H
#define ITEM_H

using namespace std;

class Item
{
     private :
          int _id, _price, _totalOrdered;
          string _name;
     public :
       //with default value
       Item(int id, int price, string name);
       //getter functions
       int getId();
       int getPrice();
       int getTotalOrdered();
       string getName();
       // Print a description of object
       void print();
       //compare two time object
       bool equals(const Item &);
};
#endif