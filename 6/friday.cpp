//Author:Analy Velazquez
//friday.cpp
//Date: March 3, 2017

#include <stdio.h>
#include <string.h>

int main()
{
    char *text = new char[1000];
    strcpy(text, "bubblybubbles");
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
            printf("(0, 0 , %c)\n", text[curr]);
            curr += 1;
        }
    }
    delete [] text;
    return 0;
}
