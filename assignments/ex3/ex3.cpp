#include <iostream>
#include <vector>
#include <sstream>
#include <string> 
#include <iomanip> 

#include <unistd.h>

#include "Item.h"
#include "Order.h"
#include "Customer.h"
using namespace std;







int semid_ResourceAccessItems;
int semid_ReadCountAccessItems;
int semid_ServiceQueueItems;
int semid_ResourceAccessCustomer;
int semid_ReadCountAccessCustomer;
int semid_ServiceQueueCustomer;
int semid_ResourceAccessOrder;
int semid_ReadCountAccessOrder;
int semid_ServiceQueueOrder;
int semid_outputSemaphore;

/*  function show_usage: print usage instructions
    when enters program
*/
void show_usage(string name){
    cerr << "Usage: " << name << " <option(s)>\n"
              << "Options:\n"
              << "\t-i\tChoose menu size (max 10)\n"
              << "\t-c\tChoose how many customers (max 10)\n"
              << "\t-w\tChoose how many waiters (max 3)\n"
              << "\t-w\tChoose simulation time"
              << "\n";
}

/* function setup: init how many dishes, participants and time
    also returns error message if more than allowed maximum
*/

vector <Customer> initCustomers(nCustomers){
    vector <Customer> c;


    for(i=0;i<=nCustomers;i++){
      c.push_back(Customer(i));
    }  
    return c;

}


int setup (int argc, char* argv[], int *nItems, int *nCustomers, int *nWaiters, int *simTime ){
    int status = 0;
    if (argc < 5) {
        show_usage(argv[0]);
        return 1;
    }
    
    srand (time(NULL));
    
    string arg;

    for (int i = 1; i < argc; i+=2) {
        arg = argv[i];
        if (arg == "-i" ) {
            *nItems = stoi(argv[i+1]);
            if (*nItems > 10){
                cerr << "--A maximum of 10 items is allowed.\n";
                status =  1;
            }
        } else if (arg == "-c") {
            *nCustomers = stoi(argv[i+1]);
            if (*nCustomers > 10){
                cerr << "--A maximum of 10 customers is allowed.\n";
                status =  1;
            }
        } else if (arg == "-w") {
            *nWaiters = stoi(argv[i+1]);
            if (*nWaiters > 3){
                cerr << "--A maximum of 10 waiters is allowed.\n";
                status = 1;
            }
        } else if (arg == "-t") {
            *simTime = stoi(argv[i+1]);
            if (*simTime < 1){
                cerr << "--Simulation time must be a non-negative number.\n";
                status = 1;
            }
        } else {
            show_usage(argv[0]);
            status = 1;
        }
    }

    return status;
}

/*  function initDishes: init menu (what dishes and their price)
    inits a vector if Item-class, randomizes price and dishes
*/
void initDishes(vector <string> dishes, vector <Item> *items, int nItems){
    int nDishes = dishes.size();
    for (int i=0; i< nItems; i++)
        (*items).push_back((Item(i, rand() % 100 + 1, dishes[rand() % nDishes] )));
}

void printPrompt(int simTime, int nItems, int nCustomers, int nWaiters, vector <Item> items){
    cout << "=======Simulation arguments======\n"
    << "Simulation time: " << simTime << "\n"
    << "Menu items count: " << nItems << "\n"
    << "Customers count: " << nCustomers << "\n"
    << "Waiters count: " << nWaiters << "\n";
    cout << "=================================\n";
    cout << "0.00 " << "Main process ID " << getpid() << " start\n";
    cout << "============Menu list============\n";
    cout 
    << left << setw(5) << "Id" 
    << left << setw(20) << "Name" 
    << left << setw(8) << "Price" 
    << left << setw(4) << "Orders\n";

    for (size_t i =0; i < items.size(); i++){
        items[i].print();
    }
}





int main(int argc, char* argv[]){
    //init menu
    vector <string> dishes = {"pizza onions", "pizza tomatoes", "pizza mozzarella",
        "salad ceaser", "salad green","hamburger bigmac", "hamburger rancho",
        "milkshake strawberry", "milkshake banana" ,"pie apple"  };
    
    int nItems, nCustomers, nWaiters, status, simTime;
    vector <Item> items;
    vector <Order> orders;
    vector <Customer> customers;
    //init from command line
    status = setup(argc, argv, &nItems, &nCustomers, &nWaiters, &simTime);
    if (status == 1)
        return 1;
    

    //init items on menu
    initDishes(dishes, &items, nItems);
    printPrompt(simTime, nItems, nCustomers, nWaiters, items);
    customers=initCustomers(nCustomers);
    placeOrder(&customers,&Items);
    //init orders board (<custumer-id> is arbitrary, <items-id> and <amount> are random)
   /* for (int i =0; i< nCustomers; i++)
        orders.push_back(Order(i, items[rand() % items.size()].getId(), rand() % 10));*/

    
    cout << "\nPress any key to continue...\n";
    getchar();
    return 0;
}