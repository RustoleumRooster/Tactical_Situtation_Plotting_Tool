#include "asserts.h"
#include <iostream>
#include <stdlib.h>
//#include "wrapper.h"
#include <string>
using namespace std;

void ___ASSERT(const char* error, const char* file, int line)
    {
    //goxy(1,1);
    //cout<<"Assertion failed: ("<<error<<") at line "<<line<<", "<<file<<endl;
    std::string msg = "Assertion Failed: (";
    std::string fi = file;
    while(fi.length() > 15)
        {
        fi.erase(0,1);
        }
    std::string er = error;
    while(er.length() > 15)
        {
        er.erase(0,1);
        }
    msg+=er;
    msg+=") at line ";
    msg+=line;
    msg+=", ";
    msg+=fi;
    //goxy(1,1);
    //color(WHITE);
    //cout<<file<<endl;
    //xwrite(1,1,msg,YELLOW);
    cout<<msg<<"\n";
    cout<<"line: "<<line<<"\n";
    //xsleep(600);
    system("pause");
//    get();
    exit(1);
    }
