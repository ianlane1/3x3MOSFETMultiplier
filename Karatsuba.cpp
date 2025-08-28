/*
* Author: Ian Lane
* Date: 8/20/2024
* 
* The purpose of this code is to create code for Karatsuba's multiplication algorithim.
* This code is intended to be updated to arduino code, where the only change is the single digit multiplication is done by the single digit NMOS multiplier
* This code works as intended.
*/

#include <iostream>
using namespace std;

// Karatsuba multiplication
unsigned long int multiply(unsigned int, unsigned int);

void divideBy10(unsigned int*, unsigned int*, int, int = 0);

int findNumDigits(unsigned int, int = 0);

int main() {
    unsigned int a, b;

    cout << "Please enter 2 numbers to multiply:" << endl;
    cin >> a;
    cin >> b;

    unsigned long int answer = multiply(a, b);
    cout << "The answer is " << answer << endl; 
	return 0;
}

unsigned long int multiply(unsigned int a, unsigned int b) {
    // find out min number of digits
    int n = (a > b) ? findNumDigits(b) : findNumDigits(a);

    int mid = n >> 1; // divides by 2

    // Base cases
    if (n == 0) { return 0; }
    else if (n == 1) { return a * b; } // This is the only actual multiplication in this entire program. This will be replaced in the arduino with the Mosfet Multiplier

    // determine first and second half by dividing by 10 mid times
    unsigned int al, ar, bl, br;
    al = a;
    bl = b;
    ar = 0; // Initalize the rights to 0 since we continuously add the remainders
    br = 0;
    
    // Pass by reference so we can change these constantly through the process
    // We want them to be divided by the min number of digits, divided by 2
    divideBy10(&al, &ar, mid);
    divideBy10(&bl, &br, mid);

    // Do the three multiplications of Karatsubas algorithm
    unsigned long int P1 = multiply(al, bl); 
    unsigned long int P2 = multiply(ar, br);
    unsigned long int P3 = multiply((al + ar), (bl + br));

    // Determine a new P3 as in karatsubas algorithm
    P3 = P3 - P1 - P2;

    // Multiply P1 by (10^n) 
    for (int i = 0; i < n; i++) {
        P1 = (P1 << 3) + (P1 << 1);
    }

    // Multiply the second term by (10^(n/2))
    for (int i = 0; i < mid; i++) {
        P3 = (P3 << 3) + (P3 << 1);
    }

    // Add up all three parts to finish Karatsubas algorithm to get the answer. This term is recursive
    return P1 + P3 + P2;
}

void divideBy10(unsigned int* x, unsigned int* y, int totalDivides, int currDivides) {
    // once the total number of divides have been done, we return.
    if (totalDivides == currDivides) { return; }

    // establish variables for the quotient and remainders
    unsigned int q, r;

    q = (*x >> 1) + (*x >> 2); // q = x * .75
    q = q + (q >> 4);
    q = q + (q >> 8);
    q = q + (q >> 16); // Now q = q * 0.8
    q = q >> 3; // Now Q = q * .8 / 8 == q / 10
    
    r = *x - (((q << 2) + q) << 1); // Remainder algorithm 

    q = q + (r > 9); // Add up error if there is error

    // Set x as the new quotient
    *x = q;

    if (r >= 10) {
        r = r - 10;
    }
    // multiply the current value by 10 for however many divides have been done so far
    for (int i = 0; i < currDivides; i++) {
        r = (r << 3) + (r << 1);
    }
    r = r + *y;
    *y = r; // Set new remainder

    return divideBy10(x, y, totalDivides, ++currDivides);
}

int findNumDigits(unsigned int x, int count) {
    if (x == 0) { return count; }
    if (int(x) - 10 < 0) { return 1; }
    unsigned int q = x;

    q = (x >> 1) + (x >> 2);
    q = q + (q >> 4); 
    q = q + (q >> 8); 
    q = q + (q >> 16); // Now q = al * 0.8 
    q = q >> 3; // Now Q = al / 10 

    count++; // Increment the count
    return findNumDigits(q, count);
}
