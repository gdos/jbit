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

#include <string.h>

#include "nano.h"

#define FRMFPS 17
#define FRMDRAW 18
#define RANDOM 23
#define KEYBUF 24
#define CONVIDEO 40

#define MAXRAND 0xFFFFFFFFFFFFLL

extern uint8_t vm_vsync;

static long long rnd_next(microio_context_t *ctx) {
	ctx->r_seed[0] = (ctx->r_seed[0] * 0x5DEECE66DLL + 0xBLL) & MAXRAND;
	return ctx->r_seed[0];
}

static uint8_t rnd_get(microio_context_t *ctx) {
	long long i;
	while (ctx->r_n_minus_1 + 1 <= (i = rnd_next(ctx) / ctx->r_divisor))
		;
	return (uint8_t)i;
}

static void rnd_put(microio_context_t *ctx, uint8_t max) {
	if (max == 0) {
		long long t = ctx->r_seed[0];
		ctx->r_seed[0] = ctx->r_seed[1];
		ctx->r_seed[1] = t;
	} else {
		ctx->r_n_minus_1 = max;
		ctx->r_divisor = MAXRAND / (max + 1);
	}
}

void microio_init(microio_context_t *ctx, int random_seed) {
	memset(ctx->convideo, ' ', MICROIO_CONVIDEO_SIZE);
	memset(ctx->keybuf, 0, MICROIO_KEYBUF_SIZE);
	ctx->r_seed[0] = random_seed;
	rnd_put(ctx, 255);
}

void microio_put(microio_context_t *ctx, uint8_t addr, uint8_t data) {
	if (addr >= CONVIDEO && addr < CONVIDEO + MICROIO_CONVIDEO_SIZE) {
		ctx->convideo[addr - CONVIDEO] = data;
	} else if (addr == KEYBUF) {
		int i;
		for (i = 0; i < MICROIO_KEYBUF_SIZE - 1; i++)
			ctx->keybuf[i] = ctx->keybuf[i + 1];
		ctx->keybuf[i] = 0;
	} else if (addr == FRMDRAW) {
		vm_vsync = 1;
	} else if (addr == RANDOM) {
		rnd_put(ctx, data);
	}
}

uint8_t microio_get(microio_context_t *ctx, uint8_t addr) {
	if (addr >= CONVIDEO && addr < CONVIDEO + MICROIO_CONVIDEO_SIZE) {
		return ctx->convideo[addr - CONVIDEO];
	} else if (addr >= KEYBUF && addr < KEYBUF + MICROIO_KEYBUF_SIZE) {
		return ctx->keybuf[addr - KEYBUF];
	} else if (addr == RANDOM) {
		return rnd_get(ctx);
	}
	return 0;
}

void microio_lcd(microio_context_t *ctx, uint8_t x, uint8_t y) {
	int i, r, c;

	for (i = 0, r = 0; r < 4; r++) {
		lcd_goto(x, y + r);
		for (c = 0; c < 10; c++, i++)
			lcd_char(ctx->convideo[i]);
	}
}

void microio_keypress(microio_context_t *ctx, uint8_t code) {
	int i;
	for (i = 0; i < MICROIO_KEYBUF_SIZE; i++) {
		if (ctx->keybuf[i] == 0) {
			ctx->keybuf[i] = code;
			return;
		}
	}
}