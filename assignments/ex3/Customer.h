#ifndef CUSTOMER_H
#define CUSTOMER_H



using namespace std;

class Customer
{
private:    
    int _customerId;
    vector <Item> items;
public:

    Customer(int custId);

    void Customer :: chooseItem(vector <Item> items);






};
#endif