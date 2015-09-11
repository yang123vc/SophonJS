/******************************************************************************
 *                SophonJS: An embedded javascript engine                     *
 *                                                                            *
 * Copyright (c) 2015 Gong Ke                                                 *
 * All rights reserved.                                                       *
 *                                                                            *
 * Redistribution and use in source and binary forms, with or without         *
 * modification, are permitted provided that the following conditions are     *
 * met:                                                                       *
 * 1.Redistributions of source code must retain the above copyright notice,   *
 * this list of conditions and the following disclaimer.                      *
 * 2.Redistributions in binary form must reproduce the above copyright        *
 * notice, this list of conditions and the following disclaimer in the        *
 * documentation and/or other materials provided with the distribution.       *
 * 3.Neither the name of the Gong Ke; nor the names of its contributors may   *
 * be used to endorse or promote products derived from this software without  *
 * specific prior written permission.                                         *
 *                                                                            *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS    *
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  *
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR           *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,      *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,        *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR         *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF     *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING       *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               *
 *****************************************************************************/

#include <sophon_types.h>
#include <sophon_mm.h>
#include <sophon_util.h>
#include <sophon_debug.h>

#define MAX_EXP 511

const static Sophon_Double pow_table[] = {
    10.,
    100.,
    1.0e4,
    1.0e8,
    1.0e16,
    1.0e32,
    1.0e64,
    1.0e128,
    1.0e256
};

Sophon_Result
sophon_ucs_to_double (const Sophon_Char *str, Sophon_Char **end,
			Sophon_Double *pd)
{
	const Sophon_Char *c;
	Sophon_Bool has_ipart = SOPHON_FALSE;
	Sophon_Bool sign = SOPHON_TRUE, esign = SOPHON_TRUE;
	Sophon_Int ipart = 0, dpart1 = 0, dpart2 = 10, epart = 0;
	Sophon_Double d, exp;
	const Sophon_Double *ppow;

	SOPHON_ASSERT(str && pd);

	c = str;
	while (sophon_isspace(*c)) {
		c++;
	}

	if (*c == '+') {
		c++;
	} else if (*c == '-') {
		c++;
		sign = SOPHON_FALSE;
	}

	if (sophon_isdigit(*c)) {
		has_ipart = SOPHON_TRUE;
		ipart = *c - '0';
		c++;
	}

	while (sophon_isdigit(*c)) {
		ipart *= 10;
		ipart += *c - '0';
		c++;
	}

	if (*c == '.') {
		c++;

		if (sophon_isdigit(*c)) {
			dpart1 = *c - '0';
			c++;
		} else if (!has_ipart) {
			return SOPHON_ERR_LEX;
		}

		while (isdigit(*c)) {
			dpart1 *= 10;
			dpart1 += *c - '0';
			dpart2 *= 10;
			c++;
		}
	} else if (!has_ipart) {
		return SOPHON_ERR_LEX;
	}

	if ((*c == 'e') || (*c == 'E')) {
		c++;

		if (*c == '+') {
			c++;
		} else if (*c == '-') {
			c++;
			esign = SOPHON_FALSE;
		}

		if (!isdigit(*c)) {
			return SOPHON_ERR_LEX;
		}

		epart = *c - '0';
		c++;

		while (sophon_isdigit(*c)) {
			epart *= 10;
			epart += *c - '0';
			c++;
		}

		if (epart > MAX_EXP)
			return SOPHON_ERR_2BIG;
	}

	if (end)
		*end = (Sophon_Char*)c;

	exp = 1.;
	for (ppow = pow_table; epart != 0; epart >>= 1, ppow++) {
		if (epart & 1)
			exp *= *ppow;
	}

	d = (Sophon_Double)ipart + (Sophon_Double)dpart1 / (Sophon_Double)dpart2;

	if (esign) {
		d *= exp;
	} else {
		d /= exp;
	}

	if (!sign)
		d *= -1.0;

	*pd = d;

	return SOPHON_OK;
}

