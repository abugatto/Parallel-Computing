#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "fraction_toolbox.hpp"

using namespace std;

// read command line arguments
static void readcmdline(fraction & frac, int argc, char* argv[])
{
    if (argc!=3)
    {
        printf("Usage: n d\n");
        printf("  n        numerator of fraction\n");
        printf("  d        denominator of fraction\n");
        exit(1);
    }

    // read n
    frac.num = atoi(argv[1]);

    // read d
    frac.denom = atoi(argv[2]);
}

static void test23467(int argc, char* argv[])
{
    //TODO: implement function
    fraction frac;
    readcmdline(frac, argc, argv);

    std::cout << "Testing fraction square for ";
    print_fraction(frac);
    std::cout << ":\t";
    fraction frac2 = square_fraction(frac);
    print_fraction(frac2);
    std::cout << std::endl;

    std::cout << "Testing fraction square inplace for ";
    print_fraction(frac);
    std::cout << ":\t";
    fraction frac3 = frac;
    square_fraction_inplace(frac3);
    print_fraction(frac3);
    std::cout << std::endl;

    std::cout << "Testing fraction to double for ";
    print_fraction(frac);
    std::cout << ":\t";
    double sum4 = fraction2double(frac);
    std::cout << sum4 << std::endl;

    std::cout << "Testing reduce fraction inplace for ";
    fraction frac6;
    frac6.num = 9;
    frac6.denom = 27;
    print_fraction(frac6);
    std::cout << ":\t";
    reduce_fraction_inplace(frac6);
    print_fraction(frac6);
    std::cout << std::endl;

    std::cout << "Testing add fractions for [";
    print_fraction(frac);
    std::cout << " + 1/2]:\t";
    fraction frac7b;
    frac7b.num = 1;
    frac7b.denom = 2;
    fraction frac7 = add_fractions(frac, frac7b);
    print_fraction(frac7);
    std::cout << std::endl;
}

static void test5()
{
    //TODO: implement function
    int i1, i2;
    std::cout << "Testing gcd. Please give two integers in order to find the GCD:" << std::endl;
    cin >> i1 >> i2;

    int GCD = gcd(i1, i2);
    std::cout << "\nThe GCD of " << i1 << " and " << i2 << " is:\t" << GCD << std::endl;
}

static void test_array_functions(int n)
{
    //TODO: implement function
    std::cout << "Testing fraction array functions:\t";
    fraction* frac_array = (fraction*) malloc(sizeof(fraction) * n);
    fill_fraction_array(frac_array, n);
    print_fraction_array(frac_array, n);

    fraction fracsum = sum_fraction_array(frac_array, n);
    std::cout << "The fraction sum is:\t";
    print_fraction(fracsum);
    std::cout << std::endl;

    fill_fraction_array(frac_array, n);
    print_fraction_array(frac_array, n);
    double doublesum = sum_fraction_array_approx(frac_array, n);
    std::cout << "The sum is:\t" << doublesum << std::endl;

    //TODO: find n for which sum function breaks. Explain what is happening.
    //At some n the 32 bit int will overflow and loop back to negatives by flipping ths sign bit
    //this requires a long int after n = 8
    std::cout << "Testing for n that overflows int in fraction array sum (n,frac):\t";
    for(std::size_t i = 1; i < 10; i++) {
        fraction* frac_array = (fraction*) malloc(sizeof(fraction) * i);
        fill_fraction_array(frac_array, i);
        fraction fracsum = sum_fraction_array(frac_array, i);
        std::cout << "(" << i << ",";
        print_fraction(fracsum);
        std::cout << ") ";
    }
    std::cout << std::endl;

    delete[] frac_array;
    frac_array = nullptr;
}

static void test_toolbox(int argc, char* argv[])
{
    cout << "\n===============  test23467  =============== " << endl;
    test23467(argc, argv);

    cout << "\n=================  test5  ================= " << endl;
    test5();

    cout << "\n==========  test_array_functions  ========= " << endl;
    int n = 5;
    test_array_functions(n);
}

int main(int argc, char* argv[])
{
    test_toolbox(argc, argv);

    return 0;
}