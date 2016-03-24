/*
 * Copyright (C) 2014 Andreas Steffen
 * HSR Hochschule fuer Technik Rapperswil
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include "test_suite.h"

#include <tests/utils/test_rng.h>
#include <utils/test.h>
#include <crypto/mgf1/mgf1.h>
#include <crypto/mgf1/mgf1_bitspender.h>

typedef struct {
	hash_algorithm_t alg;
	size_t hash_size;
	size_t ml1, ml2, ml3, seed_len;
	chunk_t seed;
	chunk_t hashed_seed;
	chunk_t mask;
	uint32_t bits[22];
} mgf1_test_t;

/**
 * MGF1 Mask Generation Function Test Vectors
 */
mgf1_test_t mgf1_tests[] = {
	{	HASH_SHA1, 20, 60, 20, 15, 24,
		chunk_from_chars(
						0xED, 0xA5, 0xC3, 0xBC, 0xAF, 0xB3, 0x20, 0x7D,
						0x14, 0xA1, 0x54, 0xF7, 0x8B, 0x37, 0xF2, 0x8D,
						0x8C, 0x9B, 0xD5, 0x63, 0x57, 0x38, 0x11, 0xC2,
						0xB5, 0xCA, 0xBF, 0x06, 0x43, 0x45, 0x19, 0xD5,
						0xE7, 0x36, 0xD0, 0x29, 0x21, 0xDA, 0x02, 0x20,
						0x45, 0xF6, 0x5F, 0x0F, 0x10, 0x04, 0x2A, 0xE3,
						0x6A, 0x1D, 0xD5, 0x9F, 0x1D, 0x66, 0x44, 0x8F,
						0xFA, 0xC6, 0xCA, 0xA4, 0x6E, 0x3B, 0x00, 0x66,
						0xA6, 0xC9, 0x80, 0x5C, 0xF5, 0x2D, 0xD7, 0x72,
						0xC6, 0xD4, 0x4F, 0x30, 0x72, 0xA2, 0xAD, 0xE0,
						0x33, 0xE8, 0x55, 0xD5, 0xE6, 0xD6, 0x00, 0x1D,
						0xA8, 0x68, 0xFF, 0x97, 0x36, 0x8A, 0xF4, 0xD6,
						0xF1, 0xB6, 0x7E, 0x1F, 0x06, 0xCB, 0x57, 0xCB,
						0x35, 0x38, 0xF2, 0x2D, 0xF6, 0x20),
		chunk_from_chars(
						0xF3, 0x9B, 0x0B, 0xB4, 0x97, 0x50, 0xB5, 0xA7,
						0xE6, 0xBD, 0xDA, 0xD0, 0x9A, 0x52, 0xBE, 0xA0,
						0x21, 0xC4, 0x90, 0xB6),
		chunk_from_chars(
						0x10, 0x43, 0x76, 0x72, 0x6C, 0xDE, 0xA0, 0x0E,
						0x77, 0x51, 0xFB, 0x58, 0x39, 0x8A, 0x36, 0xE1,
						0x63, 0x2B, 0xC9, 0x17, 0x56, 0x0C, 0x4B, 0x46,
						0xA4, 0x07, 0xA4, 0x3B, 0x8E, 0x33, 0x4D, 0xD1,
						0x65, 0xF1, 0xAC, 0xC8, 0x59, 0x21, 0x32, 0x16,
						0x44, 0x2B, 0x7F, 0xB2, 0xA8, 0xA7, 0x26, 0x5D,
						0xE8, 0x02, 0xBE, 0x8E, 0xDC, 0x34, 0xEB, 0x10,
						0x76, 0x16, 0x8C, 0xDD, 0x90, 0x92, 0x3D, 0x29,
						0x90, 0x98, 0x46, 0x11, 0x73, 0x53, 0x47, 0xB1,
						0x2C, 0xD4, 0x83, 0x78, 0x9B, 0x93, 0x2F, 0x5B,
						0xFC, 0x26, 0xFF, 0x42, 0x08, 0x1F, 0x70, 0x66,
						0x40, 0x4B, 0xE7, 0x22, 0x3A, 0x56, 0x10, 0x6D,
						0x4D, 0x29, 0x0B, 0xCE, 0xA6, 0x21, 0xB5, 0x5C,
						0x71, 0x66, 0x2F, 0x70, 0x35, 0xD8, 0x8A, 0x92,
						0x33, 0xF0, 0x16, 0xD4, 0x0E, 0x43, 0x8A, 0x14),
		{ 0, 0, 0, 4, 1, 1, 46, 103, 38, 411, 848, 57, 3540, 4058, 12403,
		  0x63, 0x2B, 0xC9, 0x17, 0x56, 669409, 0xA407A43B },
	},
	{	HASH_SHA256, 32, 64, 32, 33, 40,
		chunk_from_chars(
						0x52, 0xC5, 0xDD, 0x1E, 0xEF, 0x76, 0x1B, 0x53,
						0x08, 0xE4, 0x86, 0x3F, 0x91, 0x12, 0x98, 0x69,
						0xC5, 0x9D, 0xDE, 0xF6, 0xFC, 0xFA, 0x93, 0xCE,
						0x32, 0x52, 0x66, 0xF9, 0xC9, 0x97, 0xF6, 0x42,
						0x00, 0x2C, 0x64, 0xED, 0x1A, 0x6B, 0x14, 0x0A,
						0x4B, 0x04, 0xCF, 0x6D, 0x2D, 0x82, 0x0A, 0x07,
						0xA2, 0x3B, 0xDE, 0xCE, 0x19, 0x8A, 0x39, 0x43,
						0x16, 0x61, 0x29, 0x98, 0x68, 0xEA, 0xE5, 0xCC,
						0x0A, 0xF8, 0xE9, 0x71, 0x26, 0xF1, 0x07, 0x36,
						0x2C, 0x07, 0x1E, 0xEB, 0xE4, 0x28, 0xA2, 0xF4,
						0xA8, 0x12, 0xC0, 0xC8, 0x20, 0x37, 0xF8, 0xF2,
						0x6C, 0xAF, 0xDC, 0x6F, 0x2E, 0xD0, 0x62, 0x58,
						0xD2, 0x37, 0x03, 0x6D, 0xFA, 0x6E, 0x1A, 0xAC,
						0x9F, 0xCA, 0x56, 0xC6, 0xA4, 0x52, 0x41, 0xE8,
						0x0F, 0x1B, 0x0C, 0xB9, 0xE6, 0xBA, 0xDE, 0xE1,
						0x03, 0x5E, 0xC2, 0xE5, 0xF8, 0xF4, 0xF3, 0x46,
						0x3A, 0x12, 0xC0, 0x1F, 0x3A, 0x00, 0xD0, 0x91,
						0x18, 0xDD, 0x53, 0xE4, 0x22, 0xF5, 0x26, 0xA4,
						0x54, 0xEE, 0x20, 0xF0, 0x80),
		chunk_from_chars(
						0x76, 0x89, 0x8B, 0x1B, 0x60, 0xEC, 0x10, 0x9D,
						0x8F, 0x13, 0xF2, 0xFE, 0xD9, 0x85, 0xC1, 0xAB,
						0x7E, 0xEE, 0xB1, 0x31, 0xDD, 0xF7, 0x7F, 0x0C,
						0x7D, 0xF9, 0x6B, 0x7B, 0x19, 0x80, 0xBD, 0x28),
		chunk_from_chars(
						0xF1, 0x19, 0x02, 0x4F, 0xDA, 0x58, 0x05, 0x9A,
						0x07, 0xDF, 0x61, 0x81, 0x22, 0x0E, 0x15, 0x46,
						0xCB, 0x35, 0x3C, 0xDC, 0xAD, 0x20, 0xD9, 0x3F,
						0x0D, 0xD1, 0xAA, 0x64, 0x66, 0x5C, 0xFA, 0x4A,
						0xFE, 0xD6, 0x8F, 0x55, 0x57, 0x15, 0xB2, 0xA6,
						0xA0, 0xE6, 0xA8, 0xC6, 0xBD, 0x28, 0xB4, 0xD5,
						0x6E, 0x5B, 0x4B, 0xB0, 0x97, 0x09, 0xF5, 0xAC,
						0x57, 0x65, 0x13, 0x97, 0x71, 0x2C, 0x45, 0x13,
						0x3D, 0xEE, 0xFB, 0xBF, 0xFE, 0xAF, 0xBB, 0x4B,
						0x0D, 0x5C, 0x45, 0xD4, 0x2F, 0x17, 0x92, 0x07,
						0x66, 0x11, 0xF5, 0x46, 0xF8, 0x0C, 0x03, 0x92,
						0xF5, 0xF5, 0xFF, 0xA4, 0xF3, 0x52, 0xF4, 0x08,
						0x2C, 0x49, 0x32, 0x1A, 0x93, 0x51, 0x98, 0xB6,
						0x94, 0x83, 0x39, 0xCF, 0x6B, 0x1F, 0x2F, 0xFC,
						0x2B, 0xFF, 0x10, 0x71, 0x7D, 0x35, 0x6C, 0xEA,
						0xC5, 0x66, 0xC7, 0x26, 0x7D, 0x9E, 0xAC, 0xDD,
						0x35, 0xD7, 0x06, 0x3F, 0x40, 0x82, 0xDA, 0xC3,
						0x2B, 0x3C, 0x91, 0x3A, 0x32, 0xF8, 0xB2, 0xC6,
						0x44, 0x4D, 0xCD, 0xB6, 0x54, 0x5F, 0x81, 0x95,
						0x59, 0xA1, 0xE5, 0x4E, 0xA5, 0x0A, 0x4A, 0x42),
		 { 0, 1, 3, 4, 4, 12, 32, 36, 253, 331, 2, 1640, 503, 6924, 580,
		   0xCB, 0x35, 0x3C, 0xDC, 0xAD, 922950, 0x0DD1AA64 }
	}
};

START_TEST(mgf1_test_mgf1)
{
	mgf1_t *mgf1;
	chunk_t mask, mask1, mask2, mask3;

	mask1 = mgf1_tests[_i].mask;
	mask2 = chunk_skip(mask1, mgf1_tests[_i].ml1);
	mask3 = chunk_skip(mask2, mgf1_tests[_i].ml2);
	mask1.len = mgf1_tests[_i].ml1;
	mask2.len = mgf1_tests[_i].ml2;
	mask3.len = mgf1_tests[_i].ml3;

	mgf1 = mgf1_create(HASH_UNKNOWN, mgf1_tests[_i].seed, TRUE);
	ck_assert(mgf1 == NULL);

	mgf1 = mgf1_create(mgf1_tests[_i].alg, chunk_empty, TRUE);
	ck_assert(mgf1 == NULL);

	/* return mask in allocated chunk */
	mgf1 = mgf1_create(mgf1_tests[_i].alg, mgf1_tests[_i].seed, TRUE);
	ck_assert(mgf1);

	/* check hash size */
	ck_assert(mgf1->get_hash_size(mgf1) == mgf1_tests[_i].hash_size);

	/* get zero number of octets */
	ck_assert(mgf1->allocate_mask(mgf1, 0, &mask));
	ck_assert(mask.len == 0 && mask.ptr == NULL);

	/* get non-zero number of octets */
	ck_assert(mgf1->allocate_mask(mgf1, mgf1_tests[_i].mask.len, &mask));
	ck_assert(chunk_equals(mask, mgf1_tests[_i].mask));
	mgf1->destroy(mgf1);

	/* copy mask to pre-allocated buffer */
	mgf1 = mgf1_create(mgf1_tests[_i].alg, mgf1_tests[_i].seed, TRUE);
	ck_assert(mgf1);
	ck_assert(mgf1->get_mask(mgf1, mgf1_tests[_i].mask.len, mask.ptr));
	ck_assert(chunk_equals(mask, mgf1_tests[_i].mask));
	mgf1->destroy(mgf1);

	/* get mask in batches without hashing the seed */
	mgf1 = mgf1_create(mgf1_tests[_i].alg, mgf1_tests[_i].hashed_seed, FALSE);
	ck_assert(mgf1);

	/* first batch */
	ck_assert(mgf1->get_mask(mgf1, mask1.len, mask.ptr));
	mask.len = mask1.len;
	ck_assert(chunk_equals(mask, mask1));

	/* second batch */
	ck_assert(mgf1->get_mask(mgf1, mask2.len, mask.ptr));
	mask.len = mask2.len;
	ck_assert(chunk_equals(mask, mask2));

	/* third batch */
	ck_assert(mgf1->get_mask(mgf1, mask3.len, mask.ptr));
	mask.len = mask3.len;
	ck_assert(chunk_equals(mask, mask3));

	mgf1->destroy(mgf1);
	chunk_free(&mask);
}
END_TEST

START_TEST(mgf1_test_bitspender)
{
	mgf1_bitspender_t *bitspender;
	uint32_t bits;
	uint8_t byte;
	int j;

	bitspender = mgf1_bitspender_create(HASH_UNKNOWN,
										mgf1_tests[_i].hashed_seed, FALSE);
	ck_assert(bitspender == NULL);

	bitspender = mgf1_bitspender_create(mgf1_tests[_i].alg,
										mgf1_tests[_i].hashed_seed, FALSE);
	ck_assert(bitspender);

	for (j = 0; j < 15; j++)
	{
		ck_assert(bitspender->get_bits(bitspender, j, &bits));
		DBG1(DBG_LIB, "bits[%d] = %u, bits = %u", j, mgf1_tests[_i].bits[j],
					   bits);
		ck_assert(bits == mgf1_tests[_i].bits[j]);
	}
	ck_assert(!bitspender->get_bits(bitspender, 33, &bits));

	for (j = 15; j < 20; j++)
	{
		ck_assert(bitspender->get_byte(bitspender, &byte));
		DBG1(DBG_LIB, "bits[%d] = 0x%02x, byte = 0x%02x", j,
				   mgf1_tests[_i].bits[j], byte);
		ck_assert(byte == mgf1_tests[_i].bits[j]);
	}

	j = 20; /* 23 remaining bits */
	ck_assert(bitspender->get_bits(bitspender, 23, &bits));
	DBG1(DBG_LIB, "bits[%d] = %u, bits = %u", j,
				   mgf1_tests[_i].bits[j], bits);
	ck_assert(bits == mgf1_tests[_i].bits[j]);

	j = 21; /* 32 aligned bits */
	ck_assert(bitspender->get_bits(bitspender, 32, &bits));
	DBG1(DBG_LIB, "bits[%d] = 0x%08x, bits = 0x%08x", j,
				   mgf1_tests[_i].bits[j], bits);
	ck_assert(bits == mgf1_tests[_i].bits[j]);

	bitspender->destroy(bitspender);
}
END_TEST


Suite *mgf1_suite_create(char *name, int n)
{
	Suite *s;
	TCase *tc;

	s = suite_create(name);

	tc = tcase_create("mgf1");
	tcase_add_loop_test(tc, mgf1_test_mgf1, n, n + 1);
	suite_add_tcase(s, tc);

	tc = tcase_create("bitspender");
	tcase_add_loop_test(tc, mgf1_test_bitspender, n, n + 1);
	suite_add_tcase(s, tc);

	return s;
}

Suite *mgf1_sha1_suite_create()
{
	return mgf1_suite_create("mgf1-sha1", 0);
}

Suite *mgf1_sha256_suite_create()
{
	return mgf1_suite_create("mgf1-sha256", 1);
}
