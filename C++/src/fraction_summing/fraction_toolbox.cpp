#include <iostream>

#include "fraction_toolbox.hpp"

void print_fraction(fraction frac)
{
    std::cout << frac.num << '/' << frac.denom;
}

void print_fraction_array(fraction frac_array[], int n)
{
    std::cout << "[ " << frac_array[0].num << '/' << frac_array[0].denom;

    for (int i = 1; i < n-1; i++)
    {
        std::cout << "  ";
        print_fraction(frac_array[i]);
    }
    std::cout << "  " << frac_array[n-1].num << '/' << frac_array[n-1].denom << " ]" << std::endl;
}

fraction square_fraction(fraction frac)
{
    fraction newfrac;
    newfrac.num = frac.num * frac.num;
    newfrac.denom = frac.denom * frac.denom;

    return newfrac;
}

//TODO: implement function 3
void square_fraction_inplace(fraction& frac) {
    frac.num = frac.num * frac.num;
    frac.denom = frac.denom * frac.denom;
}

double fraction2double(fraction frac)
{
    return frac.num / (double) frac.denom;
}

int gcd(int a, int b)
{
    if(a == 0) {
        return b;
    }

    return gcd(b % a, a);
}

//TODO: implement function 6
int gcd(fraction frac) {
    int a = frac.num;
    int b = frac.denom;
    while(a != 0) {
        int t = a;
        a = b % a;
        b = t;
    }

    return b;
}

void reduce_fraction_inplace(fraction & frac)
{
    //TODO: implement function 7
    int GCD = gcd(frac);
    frac.num /= GCD;
    frac.denom /= GCD;

    //TODO: add short comment to explain which of the gcd() functions your code is calling
    //Because the input is a fraction type the compiler uses the iterative euclid function via overloading
}

fraction add_fractions(fraction frac1, fraction frac2)
{
    //TODO: implement function 8
    fraction sum;
    sum.num = (frac1.num * frac2.denom) + (frac1.denom * frac2.num);
    sum.denom = frac1.denom * frac2.denom; 

    //Add fracitons and reduce to new fraction
    reduce_fraction_inplace(sum);

    return sum;
}

double sum_fraction_array_approx(fraction frac_array[], int n)
{
    //TODO: implement function 9
    double sum = fraction2double(frac_array[0]);
    for(std::size_t i = 1; i < n; i++) {
        sum += fraction2double(frac_array[i]);
    }

    //TODO: add short comment to explain why this function is approximate
    //output is approximate because the c++ standard uses a iterative numerical algorithm to compute
    //the division. Instead of an exact solution it returns the solution after the iteration reaches
    //a certain level of convergence. A sum of approximations is a less accurate approximation
    //This, however, does require less computation than adding the fractions
    return sum;
}

//TODO: implement function 10
fraction sum_fraction_array(fraction frac_array[], int n) {
    fraction sum = frac_array[0];
    for(std::size_t i = 1; i < n; i++) {
        sum.num = (sum.num * frac_array[i].denom) + (sum.denom * frac_array[i].num);
        sum.denom = sum.denom * frac_array[i].denom; 
    }

    return sum;
}

void fill_fraction_array(fraction frac_array[], int n)
{
    fraction temp_frac;
    temp_frac.num = 1;
    for(int i = 1; i <= n; i++)
    {
        temp_frac.denom = i * (i+1);
        frac_array[i-1] = temp_frac;
    }
}

