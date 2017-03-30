//Written by: Analy Velazquez
//Lab 4, Program 1
//date: February 16, 2017

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <fstream>
#include <cstring>
#include <cctype>
#include <string>
#include <algorithm>
#include <functional>

using namespace std;
string dline;

int main()
{
    int points;
    //string dline;
    ifstream fin;
    char text[10000]; 
    char* charsrem;
    fin.open("/usr/share/dict/cracklib-small");

    cout << "Reading from the file" <<endl;
    getline(fin, dline);
    while(!fin.eof()) {
        getline(fin, dline);        
       // cout << "word: " <<dline <<endl;
        if ((dline.length()) <=6) {
        
                cout  << "word: " << dline  <<endl;//<< "total: " << total <<endl; 

            }
        }
    
        fin.close();   
    
}
