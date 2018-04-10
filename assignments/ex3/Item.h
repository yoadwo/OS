#ifndef ITEM_H
#define ITEM_H

class Item
{
     private :
          int _id, _price, _totalOrdered;
          char _name[15];
     public :
       //with default value
       Item(int id, int price, int totalOrdered = 0);
       //getter functions
       int getId();
       int getPrice();
       int getTotalOrdered();
       char* getName();
       // Print a description of object
       void print();
       //compare two time object
       bool equals(const Item &);
};
#endif