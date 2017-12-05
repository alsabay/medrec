#pragma once
#ifndef TEST_H
#define TEST_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "scarab.h"

#define RUNS 5
#define KEYRUNS 10

void test_encryt_decrypt();

void test_halfadd();

void test_fulladd();

void test_recrypt();

void test_homomorphic();

void test_fully_homomorphic();

void test_suite();

#endif
