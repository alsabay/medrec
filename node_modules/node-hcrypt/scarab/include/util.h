#pragma once
#ifndef UTIL_H
#define UTIL_H

#include <gmp.h>
#include <flint/fmpz_poly.h>
#include <flint/fmpz_mod_poly.h>

int min(int a, int b);

void fmpz_poly_rand_coeff_even(fmpz_poly_t poly, int n, ulong length, gmp_randstate_t* state);

int fmpz_probab_prime_p(fmpz_t n, int reps);

unsigned long fmpz_poly_resultant_bound(fmpz_poly_t a, fmpz_poly_t b);

#endif
