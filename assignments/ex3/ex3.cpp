#include <iostream>
#include <vector>
#include <sstream>
#include <string>  

#include "Item.h"
#include "Order.h"

using namespace std;

/*  function show_usage: print usage instructions
    when enters program
*/
void show_usage(string name){
    cerr << "Usage: " << name << " <option(s)>\n"
              << "Options:\n"
              << "\t-i\tChoose menu size (max 10)\n"
              << "\t-c\tChoose how many customers (max 10)\n"
              << "\t-w\tChoose how many waiters (max 3)\n"
              << "\n";
}

/* function setup: init how many dishes and participants
    also returns error message if more than allowed maximum
*/
int setup (int argc, char* argv[], int *nItems, int *nCustomers, int *nWaiters ){
    int status = 0;
    if (argc < 4) {
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
      for (int i=0; i< nItems; i++)
          (*items)[i].print();
}

int main(int argc, char* argv[]){
    //init menu
    vector <string> dishes = {"pizza onions", "pizza tomatoes", "pizza mozzarella",
        "salad ceaser", "salad green","hamburger bigmac", "hamburger rancho",
        "milkshake strawberry", "milkshake banana" ,"pie apple"  };
    
    int nItems, nCustomers, nWaiters, status;
    vector <Item> items;
    vector <Order> orders;

    //init from command line
    status = setup(argc, argv, &nItems, &nCustomers, &nWaiters);
    if (status == 1)
        return 1;

    //init items on menu
    initDishes(dishes, &items, nItems);
    

    
    cout << "\nPress any key to continue...\n";
    getchar();
    return 0;
}