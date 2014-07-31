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

microio_context_t microio;

void test_keypad() {
	int i, mask;

	lcd_goto(4, 5);
	keypad_scan();
	for (i = 0, mask = 1; i < 12; i++, mask <<= 1)
		lcd_char((keypad_state & mask) ? keypad_labels[i] : ' ');
}

uint8_t vm_vsync;

const uint8_t *vm_code;
uint8_t vm_size;

static const uint8_t irqvec[] PROGMEM = {
	// 255:240
	141,18,2,76,240,255,0,0,0,0,
	// 255:250
	240, 255, // NMI
	0, 3, // RESET
	240, 255, // BRK
};

// mpages; linear search for now

#define PAGE_SIZE 256

#define MAX_N_MPAGES 8

#define MPAGE_SIZE      0x0020
#define MPAGE_ADDR_MASK 0xffe0
#define MPAGE_DATA_MASK 0x001f

typedef struct {
	uint8_t page0[PAGE_SIZE];
	struct {
		uint16_t addr;
		uint8_t data[MPAGE_SIZE];
	} mpage[MAX_N_MPAGES];
	uint16_t n_mpages;
} vm_context_t;

static vm_context_t ctx_;
static vm_context_t *ctx = &ctx_;

uint8_t read6502(uint16_t address) {
	uint8_t page = address >> 8;
	uint8_t offset = address & 0xff;
	int i;
	switch (page) {
	case 0:
		return ctx->page0[offset];
	case 2:
		return microio_get(&microio, offset);
	case 255:
		if (offset >= 240)
			return pgm_read_byte(&(irqvec[offset - 240]));
		return 0;
	}
	for (i = 0; i < ctx->n_mpages; i++)
		if ((address & MPAGE_ADDR_MASK) == ctx->mpage[i].addr)
			return ctx->mpage[i].data[offset & MPAGE_DATA_MASK];
	if (page != 1 && address < 0x300 + vm_size)
		return pgm_read_byte(&(vm_code[address - 0x300]));
	return 0;
}

void write6502(uint16_t address, uint8_t value) {
	uint8_t page = address >> 8;
	uint8_t offset = address & 0xff;
	int i;
	switch (page) {
	case 0:
		ctx->page0[offset] = value;
		return;
	case 2:
		microio_put(&microio, offset, value);
		return;
	case 255:
		return;
	}
	for (i = 0; i < ctx->n_mpages; i++)
		if ((address & MPAGE_ADDR_MASK) == ctx->mpage[i].addr)
			goto write_value;
	if (ctx->n_mpages == MAX_N_MPAGES) {
		// TODO: vm state: out of memory
#ifdef PLATFORM_PC
		printf("out of memory\n");
		abort();
#endif
		return;
	}
	i = ctx->n_mpages++;
	ctx->mpage[i].addr = (address & MPAGE_ADDR_MASK);
	if (page == 1 || ctx->mpage[i].addr > 0x300 + vm_size) {
		memset(ctx->mpage[i].data, 0, MPAGE_SIZE);
	} else {
		int j, a = (address & MPAGE_ADDR_MASK);
		for (j = 0; j < MPAGE_SIZE; j++)
			if (a + j < 0x300 + vm_size)
				ctx->mpage[i].data[j] = pgm_read_byte(&(vm_code[a + j - 0x300]));
			else
				ctx->mpage[i].data[j] = 0;
	}
write_value:
	ctx->mpage[i].data[offset & MPAGE_DATA_MASK] = value;
}

static void process_events(uint8_t event, char c) {
	microio_keypress(&microio, c);
}

void vm_init() {
	memset(&ctx_, 0, sizeof(ctx_));
	trace6502(0);
	reset6502();
	lcd_clear();
	microio_init(&microio, sys_get_random_seed());
	keypad_handler = process_events;
}

void vm_step() {
	int i = 0;

	vm_vsync = 0;
	for (i = 0; i < 1000 && !vm_vsync; i++)
		step6502();
	microio_lcd(&microio, 12, 1);
}