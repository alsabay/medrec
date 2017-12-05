#include "util.h"

int
min(int a, int b)
{
	return (a<b) ? a : b;
}

void
fmpz_poly_rand_coeff_even(fmpz_poly_t poly, int n, ulong length, gmp_randstate_t* state)
{
	int i;
	mpz_t c, exp_length_half;
	mpz_init(c);
	mpz_init(exp_length_half);
	
	mpz_ui_pow_ui(exp_length_half, 2, length - 1);
	
	for (i = 1; i < n; i++) {
		mpz_urandomb(c, *state, length);
		mpz_sub(c, c, exp_length_half);
		mpz_mul_ui(c, c, 2);
		fmpz_poly_set_coeff_mpz(poly, i, c);
	}
	
	mpz_clear(c);
	mpz_clear(exp_length_half);
}


int
fmpz_probab_prime_p(fmpz_t n, int reps)
{
	int result;
	mpz_t mpz;
	
	mpz_init(mpz);
	
	//fmpz_to_mpz(mpz, n);
	fmpz_get_mpz(mpz, n);
	result = mpz_probab_prime_p(mpz, reps);
	
	mpz_clear(mpz);

	printf("%d\n", result);
	
	return result;
}

unsigned long
fmpz_poly_resultant_bound(fmpz_poly_t a, fmpz_poly_t b)
{
   if (b->length == 0) return 0;
   if (a->length == 0) return 0;
   fmpz_t t1, t2, tt;
   fmpz_init2(t1, b->length*2);
   fmpz_init2(t2, a->length*2);
   fmpz_poly_2norm(t1, a);
   fmpz_poly_2norm(t2, b);
   fmpz_pow_ui(t1, t1, b->length - 1);
   fmpz_pow_ui(t2, t2, a->length - 1);
   fmpz_init2(tt, fmpz_size(t1)+fmpz_size(t2));
   fmpz_mul(tt, t1, t2);
   fmpz_clear(t1);
   fmpz_clear(t2);

   unsigned long bound = fmpz_bits(tt);
   fmpz_clear(tt);

   return bound;
}
