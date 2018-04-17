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


int p(int semid)
{
    struct sembuf p_buf;
    p_buf.sem_num = 0;
    p_buf.sem_op = -1;
    p_buf.sem_flg = SEM_UNDO;
    
    if( semop(semid, &p_buf, 1) == -1 )
    {
        cerr << ("Error operation p(semid)") << endl;
        exit(1);
    }
    return 0;
}

int v(int semid)
{
    struct sembuf v_buf;
    v_buf.sem_num = 0;
    v_buf.sem_op = 1;
    v_buf.sem_flg = SEM_UNDO;
    
    if( semop(semid, &v_buf, 1) == -1 )
    {
        cerr << "Error operation v(semid)" << endl;
        exit(1);
    }
    return 0;
}










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


int initsem(key_t semkey)
{
    int status = 0, semid;
    union semun{
        int val;
        struct semid_ds *stat;
        ushort *array;
    } ctl_arg;
    
    // semget (key, nsems, semflag)
    if(( semid = semget(semkey, 1, SEMPERM | IPC_CREAT | IPC_EXCL )) == -1 )
    {
        if( errno == EEXIST )
        {
            semid = semget( semkey, 1, 0 );
            if (semid == -1){
                cerr << "Error creating semaphores\n";
                status = -1;
            }
        } else {
            cerr << "Error creating semaphores\n";
            status = -1;
        }

    }
    else
    {
        ctl_arg.val = 1;
        status = semctl(semid, 0, SETVAL, ctl_arg);
    }
    
    if( semid == -1 || status == -1 )
    {
        cerr << "Error initsem\n";
        exit(-1);
    }
    return semid;
}

void initSemaphores(){
    key_t semkey_ResourceAccessItems = ftok(".",'a');
    key_t semkey_ReadCountAccessItems = ftok(".",'b');
    key_t semkey_ServiceQueueItems = ftok(".",'c');
    key_t semkey_ResourceAccessCustomer = ftok(".",'d');
    key_t semkey_ReadCountAccessCustomer = ftok(".",'e');
    key_t semkey_ServiceQueueCustomer = ftok(".",'f');
    key_t semkey_ResourceAccessOrder = ftok(".",'g');
    key_t semkey_ReadCountAccessOrder = ftok(".",'h');
    key_t semkey_ServiceQueueOrder = ftok(".",'i');
    key_t semkey_OutputSemaphore = ftok(".",'j');

    semid_ResourceAccessMenu = initsem(semkey_ResourceAccessItems);
    semid_ReadCountAccessMenu = initsem(semkey_ReadCountAccessItems);
    semid_ServiceQueueMenu = initsem(semkey_ServiceQueueItems);
    semid_ResourceAccessCustomer = initsem(semkey_ResourceAccessCustomer);
    semid_ReadCountAccessCustomer = initsem(semkey_ReadCountAccessCustomer);
    semid_ServiceQueueCustomer = initsem(semkey_ServiceQueueCustomer);
    semid_ResourceAccessOrder = initsem(semkey_ResourceAccessOrder);
    semid_ReadCountAccessOrder = initsem(semkey_ReadCountAccessOrder);
    semid_ServiceQueueOrder = initsem(semkey_ServiceQueueOrder);
    semid_outputSemaphore = initsem(semkey_OutputSemaphore);
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