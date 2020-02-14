#include <iostream>
#include "gitint.h"
// Add any necessary headers
using namespace std;

void print_exception_message(const std::string& what_msg);
bool run_command(GitInt& g,const std::string& cmd_line);

int main()
{

    GitInt g;
    string cmd_line;
    bool quit = false;
    const string PROMPT_STARTER = "$ ";

    g.print_menu();
    cout << PROMPT_STARTER;
    getline(cin, cmd_line);
    // Add your code here
    quit=run_command(g,cmd_line);
    while(!quit){
        cout<<PROMPT_STARTER;
        getline(cin, cmd_line);
        quit=run_command(g,cmd_line);
        cin.clear();
    }
    return 0;

}


bool run_command(GitInt& g,const std::string& cmd_line){
    bool quit;
    try{
        quit=g.process_command(cmd_line); 
    }catch(const std::runtime_error& a){
        print_exception_message(a.what());
    }catch(const std::invalid_argument& a){
        print_exception_message(a.what());
    }
    return quit;
}

void print_exception_message(const std::string& what_msg)
{
  cout << "Error - " << what_msg << endl;
}
