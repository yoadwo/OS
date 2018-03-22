#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
#include <vector>
#include <sstream>
#include <string>    


using namespace std;

std::vector<std::string> parseLine(std::string line)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(line);
   while (std::getline(tokenStream, token, ' ')){
      tokens.push_back(token);
   }
   return tokens;
}

int main()
{

    std::cout <<"Welcome to OS SHell\n";
    char cwd[1024];
    std::string line;
    int linelen;
    // char *line = NULL;
    // size_t len = 0;
    // ssize_t nread;
    std::vector<std::string> res;

    while (1)
    {
        std::cout << "OS SHell: "; 
        if (getcwd(cwd, sizeof(cwd)) == NULL)
            perror("??");
        else
            std:: cout << cwd << " > ";

        std::getline (std::cin,line);
        linelen = line.length();
        //if only "return" was pressed
        if (linelen == 1)
            continue;

        line[linelen] = 0;
        res = parseLine(line);

        if (!res[0].compare("cd")){
            chdir(res[1].c_str());
        }
        else if (!res[0].compare("exit"))
        {
            std::cout << "C ya!\n";
            break;
        }
        else
            std::cout << res[0] <<": command not found\n";
        
        
        
    }

    std::cout << "Press any key to continue...\n";
    getchar();
    return 0;
}