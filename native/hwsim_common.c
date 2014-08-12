/*
 * Copyright (C) 2014  Emanuele Fornara
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdint.h>

#include "hwsim.h"

static struct metric_t {
	char key;
	hwsim_rect_t value;
} metrics[] = {
	{ HWSIM_M_WINDOW,       {   0,   0, 200, 300 } },
	{ HWSIM_M_DISPLAY,      {  10,  10, 168,  96 } },
};

#define N_OF_METRICS (sizeof(metrics) / sizeof(struct metric_t))

static struct color_t {
	char key;
	hwsim_color_t value;
} colors[] = {
	{ HWSIM_C_BODY,         { 0x30,0x30,0x30 } },
	{ HWSIM_C_DISPLAY_BG,   { 0x96,0xbb,0xa4 } },
	{ HWSIM_C_DISPLAY_FG,   { 0x00,0x00,0x00 } },
};

#define N_OF_COLORS (sizeof(colors) / sizeof(struct color_t))

int hwsim_get_metrics(int element, hwsim_rect_t *m) {
	int i;

	for (i = 0; i < N_OF_METRICS; i++) {
		if (metrics[i].key == element) {
			*m = metrics[i].value;
			return 1;
		}
	}
	return 0;
}

int hwsim_get_color(int element, hwsim_color_t *c) {
	int i;

	for (i = 0; i < N_OF_COLORS; i++) {
		if (colors[i].key == element) {
			*c = colors[i].value;
			return 1;
		}
	}
	return 0;
}
