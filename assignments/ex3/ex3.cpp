#include <iostream>
#include <vector>
#include <sstream>
#include <string> 
#include <iomanip> 
#include <ctime>
#include <chrono>
#include <thread>
#include <cstdlib>

#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <semaphore.h>


#include "Customer.cpp"
#include "Order.cpp"
//#include "Item.cpp"


#define SEMPERM 0600
#define MAX_ORDERS 256

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

int segmentId_Items;
int segmentId_OrdersBoard;
int segmentId_Customers;
int segmentId_Waiters;
int segmentId_ordersCount;
int segmentId_customerCounter;
int segmentId_waiterCounter;


/* function p: lower semaphore value
    copy from lab
*/
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

/* function V: increase semaphore value
    copy from lab
*/
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
              << "\t-t\tChoose simulation time (non-negative)"
              << "\n";
}

/* function setup: parse arguments into variables
    init how many dishes, participants and time
    also returns error message if more than allowed maximum
*/
int setup (int argc, char* argv[], int *nItems, int *nCustomers, int *nWaiters, double *simTime ){
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
                cerr << "--A maximum of 3 waiters is allowed.\n";
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

/* function initsem: inits a semaphore with value 1
    creates a semaphore array with a single element with a value of 1
    input: recieves a hashed key (key_t: semkey) and returns a semaphore id
*/
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
                return -1;
            }
        } else {
            cerr << "Error creating semaphores\n";
            return -1;
        }

    }
    else
    {
        ctl_arg.val = 1;
        status = semctl(semid, 0, SETVAL, ctl_arg);
        cout <<" successfully put 1 in semphore\n";
    }
    
    if( semid == -1 || status == -1 )
    {
        cerr << "Error initsem\n";
        return -1;
    }
    
    cout << "value of some semaphore isr is " << semctl(semid, 0, GETVAL, ctl_arg) <<"\n"; 
    return semid;
}

/* function initSemaphores: init semaphores for items, customers, orders, screen
    returns -1 if any semaphore fails and reports its name

*/
int initSemaphores(){
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

    cout <<" tryint sem for semkey_ResourceAccessItems\n";
    semid_ResourceAccessItems = initsem(semkey_ResourceAccessItems);
    if (semid_ResourceAccessItems == -1){
        cerr << "semaphore semid_ResourceAccessItems failed\n";
        return -1;
    }
    cout <<" tryint sem for semkey_ReadCountAccessItems\n";
    semid_ReadCountAccessItems = initsem(semkey_ReadCountAccessItems);
    if (semid_ReadCountAccessItems == -1){
        cerr << "semaphore semid_ReadCountAccessItems failed\n";
        return -1;
    }
    cout <<" tryint sem for semkey_ServiceQueueItems\n";
    semid_ServiceQueueItems = initsem(semkey_ServiceQueueItems);
    if (semid_ServiceQueueItems == -1){
        cerr << "semaphore semid_ServiceQueueItems failed\n";
        return -1;
    }
    cout <<" trying sem for semkey_ResourceAccessCustomer\n";
    semid_ResourceAccessCustomer = initsem(semkey_ResourceAccessCustomer);
    if (semid_ResourceAccessCustomer == -1){
        cerr << "semaphore semid_ResourceAccessCustomer failed\n";
        return -1;
    }
    cout <<" trying sem for semkey_ReadCountAccessCustomer\n";
    semid_ReadCountAccessCustomer = initsem(semkey_ReadCountAccessCustomer);
    if (semid_ReadCountAccessCustomer == -1){
        cerr << "semaphore semid_ReadCountAccessCustomer failed\n";
        return -1;
    }
    cout <<" trying sem for semkey_ServiceQueueCustomer\n";
    semid_ServiceQueueCustomer = initsem(semkey_ServiceQueueCustomer);
    if (semid_ServiceQueueCustomer == -1){
        cerr << "semaphore semid_ServiceQueueCustomer failed\n";
        return -1;
    }
    cout <<" trying sem for semkey_ResourceAccessOrder\n";
    semid_ResourceAccessOrder = initsem(semkey_ResourceAccessOrder);
    if (semid_ResourceAccessOrder == -1){
        cerr << "semaphore semid_ResourceAccessOrder failed\n";
        return -1;
    }
    cout <<" trying sem for semkey_ReadCountAccessOrder\n";
    semid_ReadCountAccessOrder = initsem(semkey_ReadCountAccessOrder);
    if (semid_ReadCountAccessOrder == -1){
        cerr << "semaphore semid_ReadCountAccessOrder failed\n";
        return -1;
    }
    cout <<" trying sem for semkey_ServiceQueueOrder\n";
    semid_ServiceQueueOrder = initsem(semkey_ServiceQueueOrder);
    if (semid_ServiceQueueOrder == -1){
        cerr << "semaphore semid_ServiceQueueOrder failed\n";
        return -1;
    }
    cout <<" trying sem for semkey_OutputSemaphore\n";
    semid_outputSemaphore = initsem(semkey_OutputSemaphore);
    if (semid_outputSemaphore == -1){
        cerr << "semaphore semid_outputSemaphore failed\n";
        return -1;
    }
    return 0;
}

void deleteSemaphore()
{
	semctl(semid_ResourceAccessItems, 0, IPC_RMID, NULL);
	semctl(semid_ReadCountAccessItems, 0, IPC_RMID, NULL);
	semctl(semid_ServiceQueueItems, 0, IPC_RMID, NULL);
	semctl(semid_ResourceAccessCustomer, 0, IPC_RMID, NULL);
	semctl(semid_ReadCountAccessCustomer, 0, IPC_RMID, NULL);
	semctl(semid_ServiceQueueCustomer, 0, IPC_RMID, NULL);
	semctl(semid_ResourceAccessOrder, 0, IPC_RMID, NULL);
	semctl(semid_ReadCountAccessOrder, 0, IPC_RMID, NULL);
	semctl(semid_ServiceQueueOrder, 0, IPC_RMID, NULL);
	semctl(semid_outputSemaphore, 0, IPC_RMID, NULL);
}

/* function createItems: creates shared memory for current items
    using shmget & shmat. randomizes out of hard coded dishes collection.
    returns address of shared memory or null if fails
*/
Item* createItems(int nItems, int* segmentId)
{
    vector <string> dishes = {"pizza onions", "pizza tomatoes", "pizza mozzarella",
        "salad ceaser", "salad green","hamburger bigmac", "hamburger rancho",
        "milkshake strawberry", "milkshake banana" ,"pie apple"  };
    Item* items;
    int nDishes = dishes.size();
    
    if((*segmentId = shmget(IPC_PRIVATE, sizeof(Item) * nItems, 0644 | IPC_CREAT))==-1)
    {
        cerr << "Shared memory segment exists" << endl;
        return NULL;
    }
    else
    {
        //menu = new (shmat (*segmentId,0, 0)) Dish;
        if ((items = (Item*)shmat (*segmentId,0,0)) == (void*)-1){
            cerr << "Shared memory attach failed\n";
            return NULL;
        }

        for(int i=0; i < nItems; i++)
        {
            items[i] = Item(i, rand() % 100 + 1, dishes[rand() % nDishes] );
        }
    }

    return items;
}

/* function createOrders: creates shared memory for future orders
    using shmget & shmat. memory is unused, each time add one order to it
    returns address of shared memory or null if fails
*/
Order* createOrders(int* segmentId)
{
    Order *orders;
    
    if((*segmentId = shmget(IPC_PRIVATE, sizeof(Order) * MAX_ORDERS, 0644 | IPC_CREAT))==-1)
    {
        cerr << "Shared memory segment exists" << endl;
        return NULL;
    }
    else
    {
        //menu = new (shmat (*segmentId,0, 0)) Dish;
        if ((orders = (Order*)shmat (*segmentId,0,0)) == (void*)-1){
            cerr << "Shared memory attach failed\n";
            return NULL;
        }
    }
    return orders;
}

/* function createWaitersReadCounter: creates shared memory for waiters reader semaphore
    using shmget & shmat. 
    returns address of shared memory or null if fails
*/
int* createWaitersReadCounter(int* segmentId)
{
    int* waitersReadCounter;
    
    if((*segmentId = shmget(IPC_PRIVATE, sizeof(int), 0644 | IPC_CREAT))==-1)
    {
        cerr << "Shared memory segment exists" << endl;
        return NULL;
    }
    else
    {
        //menu = new (shmat (*segmentId,0, 0)) Dish;
        if ((waitersReadCounter = (int*)shmat (*segmentId,0,0)) == (void*)-1){
            cerr << "Shared memory attach failed\n";
            return NULL;
        }
    }
    return waitersReadCounter;
}

/* function createCustomersReadCounter: creates shared memory customers reader semaphore
    using shmget & shmat. 
    returns address of shared memory or null if fails
*/
int* createCustomersReadCounter(int* segmentId)
{
    int *customersReadCounter;
    
    if((*segmentId = shmget(IPC_PRIVATE, sizeof(int) , 0644 | IPC_CREAT))==-1)
    {
        cerr << "Shared memory segment exists" << endl;
        return NULL;
    }
    else
    {
        //menu = new (shmat (*segmentId,0, 0)) Dish;
        if ((customersReadCounter = (int*)shmat (*segmentId,0,0)) == (void*)-1){
            cerr << "Shared memory attach failed\n";
            return NULL;
        }
    }
    return customersReadCounter;
}

/* function createOrdersCounter: creates shared memory for orders array index
    using shmget & shmat. must be shared memory for different processes to coordinate
    returns address of shared memory or null if fails
*/
int* createOrdersCounter(int* segmentId)
{
    int *ordersCounter;
    
    if((*segmentId = shmget(IPC_PRIVATE, sizeof(int), 0644 | IPC_CREAT))==-1)
    {
        cerr << "Shared memory segment exists" << endl;
        return NULL;
    }
    else
    {
        //menu = new (shmat (*segmentId,0, 0)) Dish;
        if ((ordersCounter = (int*)shmat (*segmentId,0,0)) == (void*)-1){
            cerr << "Shared memory attach failed\n";
            return NULL;
        }
    }
    return ordersCounter;
}


/* function printPrompt: display program arguments summary
    display simulation time, num customers, num waiters, what items were chosen
*/
void printPrompt(double simTime, int nItems, int nCustomers, int nWaiters, Item* items){
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

    for (int i =0; i < nItems; i++){
        items[i].print();
    }
    cout << "\n=================================\n";
}

/* function customerActions: simulate customer actions
    customer reads from menu and orders
*/
void customerActions(int i,Item* items, int nItems, Order* orders,int* ordersCounter, int *customersReadCounter,
chrono::time_point<std::chrono::_V2::system_clock, std::chrono::nanoseconds> start)
{
    int customerSleep_MIN=3,customerSleep_MAX=6;
    
    sleep(rand()%(customerSleep_MAX-customerSleep_MIN+1)+customerSleep_MIN);
    Item chosenItem = items[rand()%(nItems)];
    
    

    cout <<"customer " << i << "enter counter = " << *customersReadCounter <<"\n";
    
    // p(semid_outputSemaphore);
    // cout << fixed << showpoint << setprecision(3);
    // cout
    // << chrono::duration<double, milli>(chrono::high_resolution_clock::now()-start).count()/1000 
    // << " Customer ID " << i 
    // << " reads a menu about: " << chosenItem.getName();
    // v(semid_outputSemaphore);
    
    p(semid_ServiceQueueOrder);
    p(semid_ReadCountAccessOrder);
    if ((*customersReadCounter) == 0)
        p(semid_ResourceAccessOrder);
    (*customersReadCounter)++;
    v(semid_ServiceQueueOrder);
    v(semid_ReadCountAccessOrder);

        
    if(*ordersCounter==0){
        // from 0 to 10, not inclusive. if above 50%, order
       
        if (( rand()%11) > 5 ){
            p(semid_ServiceQueueOrder);
            p(semid_ResourceAccessOrder);
            v(semid_ServiceQueueOrder);
            
            
            orders[0]= Order(i,chosenItem.getId(),rand()%11 + 1);
            orders[0].clearDone();
            (*ordersCounter)++;

            v(semid_ResourceAccessOrder);

            p(semid_outputSemaphore);
            sleep(1);
            cout << fixed << showpoint << setprecision(3);
            cout
            << chrono::duration<double, milli>(chrono::high_resolution_clock::now()-start).count()/1000 
            << " Customer ID " << i 
            << " reads a menu about: " << chosenItem.getName() << "(ordered, " << orders[0].getAmount() <<")\n";
            v(semid_outputSemaphore);
            

        }
        else{
            p(semid_outputSemaphore);
             sleep(1);
            cout << fixed << showpoint << setprecision(3);
            cout
            << chrono::duration<double, milli>(chrono::high_resolution_clock::now()-start).count()/1000 
            << " Customer ID " << i 
            << " reads a menu about: " << chosenItem.getName()
            << "(doesn't want to order)\n";
            v(semid_outputSemaphore);
        }
    }
    else if(orders[(*ordersCounter)-1].isDone()){
        // from 0 to 10, not inclusive. if above 50%, order
        if (( rand()%11) > 5){
            p(semid_ServiceQueueOrder);
            p(semid_ResourceAccessOrder);
            v(semid_ServiceQueueOrder);
            //enter write CS
            orders[*ordersCounter]= Order(i,chosenItem.getId(),rand()% 11 + 1);
            orders[*ordersCounter].clearDone();
            (*ordersCounter)++;
            //exit write CS
            v(semid_ResourceAccessOrder);

            p(semid_outputSemaphore);
            sleep(1);
            cout << fixed << showpoint << setprecision(3);
            cout
            << chrono::duration<double, milli>(chrono::high_resolution_clock::now()-start).count()/1000 
            << " Customer ID " << i 
            << " reads a menu about: " << chosenItem.getName();
            cout << " (ordered, " << orders[(*ordersCounter) -1].getAmount() <<")\n";
            v(semid_outputSemaphore);
            
            
          }
        else{
            p(semid_outputSemaphore);
            sleep(1);
            cout << fixed << showpoint << setprecision(3);
            cout
            << chrono::duration<double, milli>(chrono::high_resolution_clock::now()-start).count()/1000 
            << " Customer ID " << i 
            <<" (doesn't want to order)\n";
            v(semid_outputSemaphore);
        }
    }
    
    p(semid_ReadCountAccessOrder);
    (*customersReadCounter)--;
    if ((*customersReadCounter) == 0)
        v(semid_ResourceAccessOrder);
    
    v(semid_ReadCountAccessOrder);

}

void waiterActions(int i,Item* items, int nItems, Order* orders,int* ordersCounter, int *waitersReadCounter,
std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::nanoseconds>  start)
{   
    
    int waiterSleep_MIN=1,waiterSleep_MAX=2,orderAmount;

    sleep(rand()%(waiterSleep_MAX-waiterSleep_MIN+1)+waiterSleep_MIN);
    if(*ordersCounter>0)
    {
        if(!(orders[(*ordersCounter)-1].isDone()))
        {
            Order o=orders[(*ordersCounter)-1];    
            orderAmount=orders[(*ordersCounter)-1].getAmount();
            p(semid_outputSemaphore);
            cout << fixed << showpoint << setprecision(3);
            cout
            << chrono::duration<double, milli>(chrono::high_resolution_clock::now()-start).count()/1000 
            << " waiter ID " << i <<" performs the order of Customer ID "<< o.getCustomerId()<<"("<<orderAmount<<" "<<items[orders[(*ordersCounter)-1].getItemId()].getName()<<")\n";
            v(semid_outputSemaphore);
            items[orders[(*ordersCounter)-1].getItemId()]._totalOrdered+=orderAmount;
            orders[(*ordersCounter)-1]._done=true;
        }

    }


}

/* function ManagerProcess: begin simulation of customers and waiters 
    while sim time not ended, customers() and waiters() read\write from memory
*/
void ManagerProcess(double simTime, Item* items, int nItems, Order* orders, 
    int *ordersCounter, int nCustomers, int nWaiters, int *waitersReadCounter, int *customersReadCounter){
    
    auto start = chrono::high_resolution_clock::now();
    pid_t childpid;
    int i;

    
    for (i = 0; i < nCustomers + nWaiters ;  ++i){
        childpid = fork();
        
        if ( childpid == 0 )
        {
        break;
        }
    }
    if (childpid ==0 ){
        srand (time(NULL) * getpid());
        //parent forks children
        // first children with lower i values will be customers
        // later children with higher i values will be waiters

        //customers
        if (0 <= i && i < nCustomers){
            p(semid_outputSemaphore);
            cout << fixed << showpoint << setprecision(3);
            cout
            << chrono::duration<double, milli>(chrono::high_resolution_clock::now()-start).count()/1000 
            << " Customer " << i 
            << ": Created PID "  << getpid() 
            << " PPID " << getppid() << "\n";
            v(semid_outputSemaphore);
            while(simTime >= chrono::duration<double, milli>(chrono::high_resolution_clock::now()-start).count()/1000){
                customerActions(i,items,nItems,orders,ordersCounter, customersReadCounter , start);
            
            }
            p(semid_outputSemaphore);
            cout
            << chrono::duration<double, milli>(chrono::high_resolution_clock::now()-start).count()/1000 
            << " Customer " << i 
            << ": PID "  << getpid() 
            << " end work PPID " << getppid() << "\n";
            v(semid_outputSemaphore);
        
        } 
        //waiter       
        else if (nCustomers <= i && i < nCustomers + nWaiters) { 
            p(semid_outputSemaphore);
            cout << fixed << showpoint << setprecision(3);
            cout
            << chrono::duration<double, milli>(chrono::high_resolution_clock::now()-start).count()/1000 
            << " Waiter " << i 
            << ": Created PID "  << getpid() 
            << " PPID " << getppid() << "\n";
            v(semid_outputSemaphore);
            while(simTime >= chrono::duration<double, milli>(chrono::high_resolution_clock::now()-start).count()/1000){
                waiterActions(i,items,nItems,orders,ordersCounter, waitersReadCounter , start);
            }
            p(semid_outputSemaphore);
            cout
            << chrono::duration<double, milli>(chrono::high_resolution_clock::now()-start).count()/1000 
            << " Waiter " << i 
            << ": PID "  << getpid() 
            << " end work PPID " << getppid() << "\n";
            v(semid_outputSemaphore);
        }
    exit(0);
    }
    while ((wait(0)) > 0);
}


int main(int argc, char* argv[]){
    //init menu

    
    int nItems, nCustomers, nWaiters, status, *ordersCounter,
         *waitersReadCounter, *customersReadCounter;
    double simTime;
    Item* items;
    Order* orders;
    vector <Customer> customers;
    //init from command line
    status = setup(argc, argv, &nItems, &nCustomers, &nWaiters, &simTime);
    if (status == 1)
        return 1;
    deleteSemaphore();

    if (initSemaphores() == -1){
        cout << "init semaphores fail\n";
        return 1;
    }
    
    //init items on menu from dishes
    items = createItems(nItems, &segmentId_Items);
    if (items == NULL){
        cerr << "error creating items menu\n";
        return 1;
    }
    orders = createOrders(&segmentId_OrdersBoard);
    if (orders == NULL){
        cerr << "error creating orders menu\n";
        return 1;
    }
    ordersCounter = createOrdersCounter(&segmentId_ordersCount);
    if (ordersCounter == NULL){
        cerr << "error creating orders counter\n";
        return 1;
    }
    waitersReadCounter = createWaitersReadCounter(&segmentId_waiterCounter);
    if (waitersReadCounter == NULL){
        cerr << "error creating waiters read counter\n";
        return 1;
    }
    (*waitersReadCounter) = 0;
    customersReadCounter = createCustomersReadCounter(&segmentId_customerCounter);
    if (customersReadCounter == NULL){
        cerr << "error creating customers read counter\n";
        return 1;
    }
    (*customersReadCounter) = 0;

    
    printPrompt(simTime, nItems, nCustomers, nWaiters, items);
    
    ManagerProcess(simTime, items, nItems, orders, ordersCounter, nCustomers, nWaiters, 
        waitersReadCounter, customersReadCounter );

    deleteSemaphore();
    cout << "\nPress any key to continue...\n";
    getchar();
    return 0;
}