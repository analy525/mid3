//Edited by: Analy Velazquez
//Date: March 8, 2017
//Author: Gordon Griesel
//sample program that needs optimization

#include <iostream>
using namespace std;
const int NLOOPS = 1000;
int summation(int n);
int loop_unrolling(int n);
int pointer_use();
void mult_div();
void toggle(int *x);
int bitwise_operators(int x);
void swap_variables(int *x, int *y);
void conditional_iterating();
int search(int x);

int main()
{
    for (int i=0; i<NLOOPS; i++) {
        summation(1000);
        int n=1000;
        loop_unrolling(n);
        pointer_use();
        mult_div();
        int x=1,y=2;
        toggle(&x);
        bitwise_operators(x);
        swap_variables(&x, &y);
        conditional_iterating();
        search(5);
    }
    return 0;
}

int summation(int n)
{
    //summation of 1 to n
    int sum=0;
    for (int i=0; i<300; i++) {
     //   for (int j=1; j<=n; j++) {
      //      sum += j;
       // }
       sum = n* (n+1)/2;
    }
    return sum;
}

int loop_unrolling(int n)
{
    int sum=0;
    int x=25;
    //Loop unrolling:
    //for (int i=0; i<72000; i++) {
        //for (int j=0; j<4; j++) {
            //sum += x*4;
            //sum += x+x+x+x;
            //sum += x << 2;
            sum =+(x <<2)* 7200;
 //     }
  //  }
    return sum;
    //What are 2 ways to optimize this?
}

int pointer_use()
{
    //Array index to pointer addition:
    const int SIZE = 10;
    int sum=0;                  
    int array[SIZE]={4,5,6,3,2,9,7,6,5,4};
    for (int i=0; i<20000; i++) { 
        for (int j=0; j<SIZE; j++) {
            sum += array[j];
        }
    }
    return sum;
}

void mult_div()
{
    //Multiply and divide by 2:
    //multiply x by 2:
    int x = 12345;
    for (int i=0; i<390000; i++) {
        x = x * 2;
    }
}

void toggle(int *x)
{
    //toggle x between 0 and 1
   // for (int i=0; i<390000; i++) {
   //     if (*x == 0) 
   //         *x = 1;
   //     else
   //             *x = 0;
        //ternary?

        //with XOR?
        *x = *x ^1;
    //}
}

int bitwise_operators(int x)
{
    int result;
    for (int i=0; i<430000; i++) {
        result = x & 0x01;
      //  result = x % 2;
    }
    return result;
}

void swap_variables(int *x, int *y)
{
    //swap x and y
    for (int i=0; i<198000; i++) {
        int temp = *x;
        *x = *y;
        *y = temp;
    
        //*x ^= *y;
        //*y ^= *x;
        //*x ^= *y;
        
        
        //*x += *y;
        //*y = *x-*y;
        //*x = *x-*y;
    }
}

void conditional_iterating()
{
    //At least 2 ways to optimize...
    int a=1, b=2, c=3;
        if (a == 1)
    for (int i=0; i<215000; i++) 
            b = b*3+a/2+c*3;
        if (a == 2)
    for (int i=0; i<215000; i++) 
            b = c*3+a/2+c*3;
        if (a == 3)
    for (int i=0; i<215000; i++) 
            c = a*3+c/2+c*3;
        if (a > 3)
    for (int i=0; i<215000; i++) 
            b = b*3+a/2+c*3;
    
}

int search(int x)
{
    int arr[] = {4,25,63,51,33,20,6,-2,8,16};
    int n = sizeof(arr)/sizeof(int);
    for (int i=0; i<22000; i++) {
        for (int j=0; j<n; j++) {
            if (arr[j] == x)
                return j;
        }
    }
    return -1;
}



