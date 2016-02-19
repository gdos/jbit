/*
 * Copyright (C) 2012-2013  Emanuele Fornara
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

// miocurs.cc

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include <sys/time.h>
#include <curses.h>

#include "jbit.h"
#include "devimpl.h"

class MicroIO {

	/*
	 * Simple port from Java.
	 */

private:

	typedef signed char jbyte;

	long long currentTimeMillis() {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return ((long long)tv.tv_sec * 1000LL) + tv.tv_usec / 1000;
	}

	static const int REG_FRMFPS = 17;
	static const int REG_FRMDRAW = 18;
	static const int REG_RANDOM = 23;
	static const int REG_KEYBUF = 24;
	static const int REG_CONVIDEO = 40;

	static const int KEYBUF_SIZE = 8;

	Random random;
	MicroIOKeybuf keybuf;
	MicroIODisplay display;

	static const int FRAME_MIN_WAIT = 10;

	int fps;	
	long nextFrame;
	int frameInterval;
	bool waitingForFrame;
	
	void frameReset() {
		doFrmFpsPut(40);
	}

	int doFrmFpsGet() {
		return fps;
	}

	void doFrmFpsPut(int value) {
		fps = value;
		if (value == 0) {
			frameInterval = 0;
			nextFrame = 0;
		} else {
			frameInterval = 4000 / value;
			nextFrame = currentTimeMillis() + frameInterval;
		}
	}

	void doFrmDrawPut() {
		if (frameInterval)
			waitingForFrame = true;
	}

public:

	MicroIO() {
		waitingForFrame = false;
	}

	void reset() {
		random.reset();
		keybuf.reset();
		display.reset();
		frameReset();
	}
	
	const MicroIODisplay *getDisplay() {
		return &display;
	}

	void keyPressed(int keyCode) {
		keybuf.enque(keyCode);
	}

	int doSomeWork() {
		int wait = -1;
		long now = currentTimeMillis();
		if (nextFrame != 0 && now >= nextFrame - FRAME_MIN_WAIT) {
			do
				nextFrame += frameInterval;
			while (nextFrame < now);
			if (waitingForFrame) {
				waitingForFrame = false;
			}
			wait = 0;
		}
		if (waitingForFrame) {
			wait = (int)(nextFrame - now - (FRAME_MIN_WAIT >> 1));
		}
		return wait;
	}

	int get(int address) {
		switch (address) {
		case REG_FRMFPS:
			return doFrmFpsGet();
		case REG_RANDOM:
			return random.get();
		default:
			if (address >= REG_KEYBUF
					&& address < REG_KEYBUF + KEYBUF_SIZE) {
				return keybuf.get(address - REG_KEYBUF);
			} else if (address >= REG_CONVIDEO
					&& address < REG_CONVIDEO + MicroIODisplay::CONVIDEO_SIZE) {
				return display.get(address - REG_CONVIDEO);
			}
		}			
		return 0;
	}

	int put(int address, int value) {
		switch (address) {
		case REG_FRMFPS:
			doFrmFpsPut(value);
			break;
		case REG_FRMDRAW:
			doFrmDrawPut();
			break;
		case REG_RANDOM:
			random.put(value);
			break;
		case REG_KEYBUF:
			keybuf.put(address, value);
			break;
		default:
			if (address >= REG_CONVIDEO
					&& address < REG_CONVIDEO + MicroIODisplay::CONVIDEO_SIZE)
				display.put(address - REG_CONVIDEO, value);
		}			
		return 0;
	}
};

static void curses_init() {
	initscr();
	cbreak();
	noecho();
	nonl();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	timeout(0);
}

static void curses_cleanup() {
	endwin();
}

class CursesDevice : public Device {
private:
	static const int KEYCODE_NONE = 0;
	static const int KEYCODE_BREAK = -1;

	MicroIO io;
	int display_y;
	int display_x;
	int cursor_y;
	int cursor_x;
	const char *status_msg;

	void show_static() {
		clear();
		int y = 0;
		int x = 18;
		mvaddstr(y++, x, "[:] break");
		y++;
		x = 7;
		display_y = y;
		display_x = x;
		const MicroIODisplay *display = io.getDisplay();
		for (int i = 0; i < MicroIODisplay::N_OF_LINES; i++)
			mvaddstr(y++, x, display->get_line(i));
		x = 0;
		y++;
		mvaddstr(y++, x, "[1]      [2] abc  [3] def");
		y++;
		mvaddstr(y++, x, "[4] ghi  [5] jkl  [6] mno");
		y++;
		mvaddstr(y++, x, "[7] pqrs [8] tuv  [9] wxyz");
		y++;
		mvaddstr(y++, x, "[*]      [0]  +   [#]    ");
		y++;
		cursor_y = y;
		cursor_x = 0;
	}
	void flush() {
		const MicroIODisplay *display = io.getDisplay();
		mvaddstr(0, 0, status_msg);
		for (int i = 0; i < MicroIODisplay::N_OF_LINES; i++)
			mvaddstr(display_y + i, display_x, display->get_line(i));
		move(cursor_y, cursor_x);
		refresh();
	}
	int filter_char(int c) {
		if (c == ':')
			return KEYCODE_BREAK;
		c = MicroIOKeybuf::map_keypad(c);
		return c ? c : KEYCODE_NONE;
	}
	void wait(int ms) {
		napms(ms);
	}
public:
	CursesDevice() {
		curses_init();
		atexit(curses_cleanup);
	}
	// VM
	void set_address_space(AddressSpace *dma) {
	}
	void reset() {
		io.reset();
		status_msg = "";
		show_static();
		flush();
	}
	void put(int address, int value) {
		io.put(address, value);
	}
	int get(int address) {
		return io.get(address);
	}
	// Device
	void set_args(int argc_, char **argv_) {
	}
	bool update(int status) {
		switch (status) {
		case 0: // OK
			status_msg = "RUNNING";
			break;
		case 1: // HALTED
			status_msg = "HALTED ";
			break;
		default:
			status_msg = "FAILED ";
			break;
		}
		int c = filter_char(getch());
		if (c == KEYCODE_BREAK)
			exit(0);
		if (c != KEYCODE_NONE)
			io.keyPressed(c);
		int ms = 100;
		if (status == 0) // OK
			ms = io.doSomeWork();
		if (ms) {
			flush();
			wait(ms);
		}
		return true;
	}
};

static Device *new_Device(Tag tag) {
	return new CursesDevice();
}

static DeviceEntry entry("microio", new_Device);