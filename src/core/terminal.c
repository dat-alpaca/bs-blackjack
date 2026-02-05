#include <stdio.h>
#include <stdlib.h>

#include "terminal.h"
#include "keys.h"

void terminal_initialize()
{
	write(STDOUT_FILENO, CURSOR_HIDE, 6);
	setvbuf(stdout, NULL, _IONBF, 0);
}

void terminal_disable_raw_mode()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTerminalOS);
	printf(CURSOR_SHOW);
	fflush(stdout);
}
void terminal_enable_raw_mode()
{
	tcgetattr(STDIN_FILENO, &originalTerminalOS);
	atexit(terminal_disable_raw_mode);

	struct termios raw = originalTerminalOS;

	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	raw.c_oflag |= OPOST | ONLCR;
	raw.c_cflag |= CS8;
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void terminal_exit(void)
{
	terminal_disable_raw_mode();
}

int terminal_read_key()
{
	fflush(stdout);

	char c;
	if (read(STDIN_FILENO, &c, 1) != 1)
		return -1;

	if (c == '\r' || c == '\n')
		return KEY_ENTER;

	enum { ESC_KEY = 27 };
	if (c == ESC_KEY)
	{
		char seq[2];
		if (read(STDIN_FILENO, &seq[0], 1) != 1)
			return ESC_KEY;

		if (read(STDIN_FILENO, &seq[1], 1) != 1)
			return ESC_KEY;

		if (seq[0] == '[')
		{
			if (seq[1] == 'A')
				return KEY_UP;

			if (seq[1] == 'B')
				return KEY_DOWN;
		}

		return ESC_KEY;
	}

	return c;
}