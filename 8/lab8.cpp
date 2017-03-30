//Author:Analy Velazquez
//friday.cpp
//Date: March 3, 2017

#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;

int decode(unsigned char *codes, int lenght, char *data);

#ifdef UTEST
int unit_test();
#endif

int main()
{
    #ifdef UTEST
    unit_test();
    return 0;
    #endif

    char *text = new char[1000];
    strcpy(text, "banana");
    strcpy(text, "ban");
    int len = strlen(text);
    printf("len: %i\n", len);
    int curr = 0;
    //some kind of loop
    while (curr < len) {
        int start = 0;
        int end = curr-1;
        bool match = false;
        for (int i=start; i <=end; i++) {
            if (text[i] == text[curr]) {
                //found a match. generate a code.
                char c = (curr < (len-1)) ? text[curr+1] : '$';
                printf("(%i, %i, %c)\n", curr-i, 1, c);
                curr += 2;
                match = true;
                break;
            }
        }
        if (!match) {
           // char c = text[curr];
            printf("(0, 0 , %c)\n", text[curr]);
            curr += 1;
        }
    } 
    delete [] text;
    return 0;
}

int decode(unsigned char *codes, int length, char *data){
    //codes:  an unsigned char array of codes, in sets of 3.
    //length: the known length of the decoded data.
    //data:   starting address of where the decoded data will go.//char data[];
    int idx = 0; //for lenght
    int ptr = 0; //for pointer into data
    //want to loop until we run out of data
    while(ptr < length) {
        unsigned char a = codes[idx+0];
        unsigned char b = codes[idx+1];
        unsigned char c = codes[idx+2];
        idx += 3;
        if  (a == 0 && b == 0) {
            data[ptr++] = c;
        } else {
            for (int i=0; i<b; i++) {
                data[ptr] = data[ptr - a];
                ++ptr;
            }
            data[ptr++]=c;
        }
    }

return 0;
}

#ifdef UTEST
int unit_test()
{
#ifdef T1
    int data_length = 82;
    unsigned char codes[] = {
        0,  0,  'i',
        0,  0,  ' ',
        0,  0,  'l',
        3,  1,  'k',
        0,  0,  'e',
        5,  1,  'i',
        7,  13, 'b',
        0,  0,  'a',
        0,  0,  'n',
        2,  3,  's',
        8,  24, '.',
        40, 16, 'a',
        5,  1,  'd',
        12, 4,  'd',
        29, 5,  '$' };
#endif

#ifdef T2   
    int data_length = 6;
    unsigned char codes[] = {
        0, 0, 'b',
        0, 0, 'a',
        0, 0, 'n',
        2, 3, '$' };
#endif

    char *data = new char[1000];
    int ret = decode(codes, data_length, data);
    for (int i = 0; i < data_length; i++) 
        cout << data[i];
    cout << endl;
    delete [] data;
        return ret;
}
#endif
