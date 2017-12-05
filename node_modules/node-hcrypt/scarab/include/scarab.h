#pragma once
#ifndef SCARAB_H
#define SCARAB_H

#include <gmp.h>
#include <assert.h>
#include <flint/fmpz_poly.h>
#include <flint/fmpz_mod_poly.h>
#include "types.h"
#include "parameters.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

/** main function **/

void fhe_keygen(fhe_pk_t pk, fhe_sk_t sk);

void fhe_encrypt(mpz_t c, fhe_pk_t pk, int m);

int  fhe_decrypt(mpz_t c, fhe_sk_t sk);

void fhe_recrypt(mpz_t c, fhe_pk_t pk);

void fhe_add(mpz_t res, mpz_t a, mpz_t b, fhe_pk_t pk);

void fhe_mul(mpz_t res, mpz_t a, mpz_t b, fhe_pk_t pk);

void fhe_fulladd(mpz_t sum, mpz_t c_out, mpz_t a, mpz_t b, mpz_t c_in, fhe_pk_t pk);

void fhe_halfadd(mpz_t sum, mpz_t c_out, mpz_t a, mpz_t b, fhe_pk_t pk);

#ifdef __cplusplus
}
#endif

#endif
