#include "scarab.h"
#include "time.h"

#define SEED time(NULL)

void fhe_keygen(fhe_pk_t pk, fhe_sk_t sk) {
	int i, j;
	mpz_t temp;
	mpz_init(temp);

	fmpz_poly_t F;
	fmpz_poly_init(F);

	// set f = x^n + 1
	fmpz_poly_set_coeff_ui(F, SCARAB_N, 1);
	fmpz_poly_set_coeff_ui(F, 0, 1);
	
	fmpz_poly_t G;
	fmpz_poly_init(G);

	gmp_randstate_t randstate;
	gmp_randinit_default(randstate);
	gmp_randseed_ui(randstate, SEED);

	do {
		fmpz_poly_rand_coeff_even(G, SCARAB_N, LOG_NU, &randstate);

		//fmpz_poly_set_coeff_ui(G, 0, 1);
		fmpz_t *G_0 = (fmpz_t *) fmpz_poly_get_coeff_ptr(G, 0);
        fmpz_add_ui(*G_0, *G_0, 1);

		// set p = resultant(G(x), F(x))
		fmpz_t fmpz_p;
		fmpz_init2(fmpz_p, fmpz_poly_resultant_bound(G, F)/FLINT_BITS + 2);
		fmpz_poly_resultant(fmpz_p, G, F);

		fmpz_get_mpz(temp, fmpz_p);
		fmpz_clear(fmpz_p);
	} while (!mpz_probab_prime_p(temp, 25));

	fmpz_t P;
	fmpz_init(P);
	fmpz_set_mpz(P, temp);

	fmpz_mod_poly_t F_mod_p, G_mod_p, D_mod_p;
	fmpz_mod_poly_init(F_mod_p, P);
	fmpz_mod_poly_init(G_mod_p, P);
	fmpz_mod_poly_init(D_mod_p, P);
	fmpz_mod_poly_set_fmpz_poly(F_mod_p, F);
	fmpz_mod_poly_set_fmpz_poly(G_mod_p, G);
	
	fmpz_mod_poly_gcd_euclidean(D_mod_p, F_mod_p, G_mod_p);

	fmpz_t *root = (fmpz_t *) fmpz_poly_get_coeff_ptr(D_mod_p, 0);
	fmpz_neg(*root, *root);	// root of D = -coeff[0]
	fmpz_mod(*root, *root, P);

	fmpz_t r;
    fmpz_init2(r, fmpz_poly_resultant_bound(G, F)/FLINT_BITS + 2);

	fmpz_poly_t Z, t;
	fmpz_poly_init(Z);
	fmpz_poly_init(t);

	fmpz_poly_xgcd(r, Z, t, G, F);

	fmpz_t fmpz_p;
	fmpz_init2(fmpz_p, mpz_size(temp));
	fmpz_set_mpz(fmpz_p, temp);

	// build pk
	mpz_set(pk->p, temp);
	fmpz_get_mpz(pk->alpha, *root);

	// build sk
	mpz_set(sk->p, temp);
	fmpz_poly_get_coeff_mpz(sk->B, Z, 0);
	mpz_mul_ui(temp, temp, 2);
	mpz_mod(sk->B, sk->B, temp);
	
	// Build hint
	mpz_t B_i, r_plus, r_minus;
	mpz_init(B_i);
	mpz_init(r_plus);
	mpz_init(r_minus);
	
	mpz_fdiv_q_ui(B_i, sk->B, S2);
	for (i = 0; i < S2; i++) {
		mpz_set(pk->B[i], B_i);
		fhe_encrypt(pk->c[i], pk, 1);
	}

	mpz_add(pk->B[0], pk->B[0], sk->B);
	mpz_submul_ui(pk->B[0], B_i, S2);
	for (i = S2; i < S1; i++) {
		mpz_urandomm(B_i, randstate, temp); // p is already 2*pk->p !
		mpz_sub(B_i, B_i, pk->p);
		mpz_set(pk->B[i], B_i);
		fhe_encrypt(pk->c[i], pk, 0);
	}
	
	// add/sub values randomly
	for (i = 0; i < S2; i++) {
		mpz_urandomm(r_plus, randstate, pk->p);
		mpz_neg(r_minus, r_plus);

		while (1) {
			j = rand() % S2;
			mpz_add(pk->B[j], pk->B[j], r_plus);
			if (mpz_cmp(pk->B[j], pk->p) > 0) {
				mpz_sub(r_plus, pk->B[j], pk->p);
				mpz_set(pk->B[j], pk->p);
			} else {
				break;
			}
		}

		while (1) {
			j = rand() % S2;
			mpz_add(pk->B[j], pk->B[j], r_minus);
			mpz_neg(temp, pk->p);
			if (mpz_cmp(pk->B[j], temp) < 0) {
				mpz_add(r_minus, pk->B[j], pk->p);
				mpz_neg(pk->B[j], pk->p);
			} else {
				break;
			}
		}
	}

	// shuffle
	for (i = 0; i < S1; i++) {
		j = rand() % S1;
		mpz_swap(pk->B[i], pk->B[j]);
		mpz_swap(pk->c[i], pk->c[j]);
	}
	
	// cleanup
	mpz_clear(B_i);
	mpz_clear(r_plus);
	mpz_clear(r_minus);
	mpz_clear(temp);
	fmpz_clear(fmpz_p);
	fmpz_clear(r);
	fmpz_poly_clear(Z);
	fmpz_poly_clear(t);
	fmpz_poly_clear(F);
	fmpz_poly_clear(G);
	fmpz_mod_poly_clear(F_mod_p);
	fmpz_mod_poly_clear(G_mod_p);
	fmpz_mod_poly_clear(D_mod_p);
	fmpz_clear(P);
	gmp_randclear(randstate);
}

void fhe_encrypt(mpz_t c, fhe_pk_t pk, int m) {
	if ((m != 0) && (m != 1)) {
		fprintf(stderr, "\nm must be either 0 or 1\n");
		abort();
	}

	fmpz_poly_t C;
	fmpz_t fmpz_c, alpha;

	fmpz_poly_init(C);
	fmpz_init2(fmpz_c, 1000000);

	fmpz_init2(alpha, mpz_size(pk->alpha));
	fmpz_set_mpz(alpha, pk->alpha);

	gmp_randstate_t randstate;
	gmp_randinit_default(randstate);
	gmp_randseed_ui(randstate, SEED + 1000 * rand());

	fmpz_poly_rand_coeff_even(C, SCARAB_N, 2, &randstate);
	fmpz_t *C_0 = fmpz_poly_get_coeff_ptr(C, 0);
    if (*C_0) {
	    fmpz_add_ui(*C_0, *C_0, m);
	} else {
	    fmpz_poly_set_coeff_ui(C, 0, m);
	}

	fmpz_poly_evaluate_fmpz(fmpz_c, C, alpha);

	fmpz_get_mpz(c, fmpz_c);
	fmpz_clear(fmpz_c);

	mpz_mod(c, c, pk->p);

	// cleanup
	fmpz_poly_clear(C);
	fmpz_clear(alpha);
	fmpz_clear(fmpz_c);
	gmp_randclear(randstate);
}

int fhe_decrypt(mpz_t c, fhe_sk_t sk) {
	int m;
	
	mpz_t temp, q, r;
	mpz_init(temp);
	mpz_init(q);
	mpz_init(r);
	
	mpq_t n, d, one_half;
	mpq_init(n);
	mpq_init(d);
	mpq_init(one_half);
	
	mpz_mul(temp, c, sk->B);
	mpz_fdiv_qr(q, r, temp, sk->p);
	
	// round: add 1 to q if r/p > 1/2
	mpq_set_ui(one_half, 1, 2);
	mpq_set_z(n, r);
	mpq_set_z(d, sk->p);
	mpq_div(n, n, d);
	
	if (mpq_cmp(n, one_half) > 0) { // n > 1/2
		mpz_add_ui(q, q, 1);
	}

	mpz_add(temp, c, q);	
	mpz_mod_ui(temp, temp, 2);
	
	m = mpz_get_si(temp);
		
	// cleanup
	mpz_clear(temp);
	mpz_clear(q);
	mpz_clear(r);
	mpq_clear(n);
	mpq_clear(d);
	mpq_clear(one_half);
	
	return m;
}

void fhe_add(mpz_t res, mpz_t a, mpz_t b, fhe_pk_t pk) {
	mpz_add(res, a, b);
	mpz_mod(res, res, pk->p);
}


void fhe_mul(mpz_t res, mpz_t a, mpz_t b, fhe_pk_t pk)  {
	mpz_mul(res, a, b);
	mpz_mod(res, res, pk->p);
}


void fhe_fulladd(mpz_t sum, mpz_t c_out, mpz_t a, mpz_t b, mpz_t c_in, fhe_pk_t pk) {
	mpz_t temp;
	mpz_init(temp);

	mpz_add(temp, a, b);
	mpz_add(temp, temp, c_in);
	mpz_mod(sum, temp, pk->p);
	
	mpz_mul(temp, a, b);
	mpz_addmul(temp, c_in, a);
	mpz_addmul(temp, c_in, b);
	mpz_mod(c_out, temp, pk->p);
	
	mpz_clear(temp);
}


void fhe_halfadd(mpz_t sum, mpz_t c_out, mpz_t a, mpz_t b, fhe_pk_t pk) {
	fhe_add(sum, a, b, pk);
	fhe_mul(c_out, a, b, pk);
}


void fhe_recrypt(mpz_t c, fhe_pk_t pk) {
	int i, j, k;

	mpz_t C[S1][SCARAB_T], H[SCARAB_T][SCARAB_T], temp, p;
	mpq_t q;
	
	mpz_init(temp);
	mpz_init(p);
	mpq_init(q);

	for (i = 0; i < S1; i++) {
		for (j = 0; j < SCARAB_T; j++) {
			mpz_init(C[i][j]);
		}
	}

	for (i = 0; i < SCARAB_T; i++) {
		for (j = 0; j < SCARAB_T; j++) {
			mpz_init_set_ui(H[i][j], 0);
		}
	}
	
	// Fill C-matrix
	mpz_mul_ui(p, pk->p, 2);
	for (i = 0; i < S1; i++) {
		mpz_mul(temp, c, pk->B[i]);
		mpz_mod(temp, temp, p);
		mpq_set_num(q, temp);
		mpq_set_den(q, pk->p);
		mpq_canonicalize(q);
		double d = mpq_get_d(q);

		// base convert and encrypt d
		for (j = 0; j < SCARAB_T; j++) {
			fhe_encrypt(C[i][j], pk, (int) d);
			mpz_mul(C[i][j], C[i][j], pk->c[i]);
			mpz_mod(C[i][j], C[i][j], pk->p);
			d -= (int) d;
			d *= 2;
		}
	}
	
	// Construct Hammingweight in H-matrix
	for (j = 0; j < SCARAB_T; j++) {
		for (i = 1; i <= S1; i++) {
			for (k = (i < (2<<(SCARAB_S-2))) ? i : (2<<(SCARAB_S-2)); k >= 2; k--) {
				mpz_addmul(H[k-1][j], H[k-2][j], C[i-1][j]);
				mpz_mod(H[k-1][j], H[k-1][j], pk->p);
			}
			mpz_add(H[0][j], H[0][j], C[i-1][j]);
			mpz_mod(H[0][j], H[0][j], p);
		}
	}

	for (j = 0; j < SCARAB_T; j++) {
		mpz_set(H[2][j], H[3][j]);
	}

	for (j = 1; j < SCARAB_T; j++) {
		for (i = min(SCARAB_S, j+1)-1; i >= 0; i--) {
			mpz_swap(H[i][j], H[j][j-i]);
		}
	}

	// merge rows 0 and 3; 1 and 4
	for (i = 0; i < 2; i++) {
		for (j = 0; j < SCARAB_S; j++) {
			mpz_set(H[i][i+j+1], H[i+SCARAB_S][i+j+1]);
		}
	}
	
	// carry save adder of rows 0,1,2 --> 0,1 (columnwise)
	for (j = 0; j < SCARAB_T; j++) {
		fhe_fulladd(H[3][j], H[4][j], H[0][j], H[1][j], H[2][j], pk);
	}

	// leftshift the row with the carry bits
	mpz_swap(H[0][SCARAB_T-1], H[3][SCARAB_T-1]);
	fhe_encrypt(H[1][SCARAB_T-1], pk, 0);
	for (j = 0; j < SCARAB_T-1; j++) {
		mpz_swap(H[0][j], H[3][j]);
		mpz_swap(H[1][j], H[4][j+1]);
	}

	// ripple-carry-add rows 0 and 1 --> 0 (LSB at SCARAB_T-1)
	// special cases: nothing to do for col SCARAB_T-1, halfadder for SCARAB_T-2
	// note: carry is in temp, result in last row (4)
	fhe_halfadd(H[4][SCARAB_T-2], temp, H[0][SCARAB_T-2], H[1][SCARAB_T-2], pk);
	for (j = SCARAB_T-3; j >= 0; j--) {
		fhe_fulladd(H[4][j], temp, H[0][j], H[1][j], temp, pk);
	}

	// round to nearest integer
	mpz_add(temp, H[4][0], H[4][1]);
	mpz_mod_ui(c, c, 2);
	mpz_add(c, c, temp);
	mpz_mod(c, c, pk->p);
	
	// cleanup
	for (i = 0; i < S1; i++) {
		for (j = 0; j < SCARAB_T; j++) {
			mpz_clear(C[i][j]);
		}
	}

	for (i = 0; i < SCARAB_T; i++) {
		for (j = 0; j < SCARAB_T; j++) {
			mpz_clear(H[i][j]);
		}
	}
	
	mpz_clear(temp);
	mpz_clear(p);
	mpq_clear(q);
}
