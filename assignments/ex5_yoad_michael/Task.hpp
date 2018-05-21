#ifndef Task_hpp
#define Task_hpp

#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;


class Task {

public:
    Task();
    Task(string sentName);
    virtual ~Task();
    virtual string getName() = 0;
    string name;
};

#endif /* Task_hpp */
