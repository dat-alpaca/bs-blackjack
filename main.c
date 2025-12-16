#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <termio.h>
#include <time.h>
#include <unistd.h>

#define CLEAR_SCREEN "\033[2J"
#define ERASE_LINE "\33[2K\r"

#define CURSOR_UP(n) "\x1b[" #n "A"
#define CURSOR_DOWN(n) "\x1b[" #n "B"
#define CURSOR_HIDE "\x1b[?25l"
#define CURSOR_SHOW "\x1b[?25h"

#define RESET "\033[0m"
#define RED "\e[0;31m"
#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define WHITE "\e[0;37m"
#define YELLOW "\e[0;33m"
#define PURPLE "\e[0;35m"

#define KEY_UP 1000
#define KEY_DOWN 1001
#define KEY_ENTER 1002

#define DECK_SIZE 52
#define HAND_SIZE 13

// utils.h:
void swap(int* lhs, int* rhs)
{
	int temp = *lhs;
	*lhs = *rhs;
	*rhs = temp;
}

// terminal.h
static struct termios originalTerminalOS;

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

	// ESC:
	if (c == 27)
	{
		char seq[2];
		if (read(STDIN_FILENO, &seq[0], 1) != 1)
			return 27;

		if (read(STDIN_FILENO, &seq[1], 1) != 1)
			return 27;

		if (seq[0] == '[')
		{
			if (seq[1] == 'A')
				return KEY_UP;

			if (seq[1] == 'B')
				return KEY_DOWN;
		}

		return 27;
	}

	return c;
}

// game data.h:
typedef enum game_result
{
	GAME_RESULT_NONE = 0,
	GAME_RESULT_EXIT,
	GAME_RESULT_VICTORY,
	GAME_RESULT_DEFEAT,
	GAME_RESULT_DRAW
} game_result;

typedef struct client_data
{
	int totalVictories;
	int totalDefeats;
	int totalDraws;
} client_data;

typedef struct deck
{
	int deck[DECK_SIZE];
	int index;
} deck;

typedef struct player_data
{
	int hand[HAND_SIZE];
	int index;
	int score;
} player_data;

typedef struct game_data
{
	client_data clientData;
	deck deck;
	game_result result;

	player_data userData;
	player_data dealerData;
} game_data;

static void initialize_player_data(player_data* player)
{
	player->index = 0;
	player->score = 0;
}

static void shuffle_deck(deck* deck)
{
	for (int i = 0; i < DECK_SIZE; ++i)
	{
		int k = rand() % (i + 1);
		swap(&deck->deck[i], &deck->deck[k]);
	}
}

static void initialize_deck(deck* deck)
{
	deck->index = 0;

	int count = 0;
	for (int i = 1; i <= 13; ++i)
	{
		for (int j = 0; j < 4; ++j)
			deck->deck[count++] = i;
	}
}

static void initialize_game(game_data* game)
{
	initialize_deck(&game->deck);
	shuffle_deck(&game->deck);

	game->result = GAME_RESULT_NONE;

	initialize_player_data(&game->dealerData);
	initialize_player_data(&game->userData);
}

// game utils.h
static int game_get_card_value(int card)
{
	if (card >= 10)
		return 10;

	return card;
}

static void game_draw_card(deck* deck, player_data* player)
{
	if (player->index >= 13 || deck->index >= 52)
	{
		fprintf(stderr, "Invalid deck index position");
		exit(1);
	}

	player->hand[player->index] = deck->deck[deck->index];
	player->score += game_get_card_value(player->hand[player->index]);

	++deck->index;
	++player->index;
}

// display:
typedef enum ui_view
{
	UI_VIEW_MAIN_MENU = 0,
	UI_VIEW_PLAYING
} ui_view;

typedef enum ui_option
{
	UI_OPTION_PLAY = 0,
	UI_OPTION_QUIT = 1,
	UI_OPTION_AMOUNT = 2
} ui_option;

typedef struct ui_data
{
	ui_view currentView;
	ui_option option;
	bool isRunning;
} ui_data;

static void ui_cycle_option(ui_data* ui, bool reverse)
{
	if (!reverse)
		ui->option = (ui->option + 1) % UI_OPTION_AMOUNT;
	else
		ui->option = (ui->option + UI_OPTION_AMOUNT - 1) % UI_OPTION_AMOUNT;
}

static void ui_print_header(game_data* game)
{
	int wins = game->clientData.totalVictories;
	int draws = game->clientData.totalDraws;
	int lost = game->clientData.totalDefeats;

	printf("\n");
	printf(GREEN "-=-= >" RED " Blackjack " GREEN "< =-=-\n" RESET);
	printf("%s%d%s%d%s%d", GREEN "Wins: " RESET, wins, RESET " | " YELLOW "Draws: " RESET, draws, RESET " | " RED "Defeats: " RESET, lost);
	printf("\n\n");
}

static void ui_print_options(game_data* game)
{
	printf(CLEAR_SCREEN);
	ui_print_header(game);

	printf("Use the 'UP' and 'DOWN' arrow keys to switch through the options.\n");
	printf("Press 'Enter' to select the option chosen.\n\n");
}

static void ui_print_menu(game_data* game, ui_data* ui)
{
	ui_print_options(game);

	switch (ui->option)
	{
		case UI_OPTION_PLAY:
		{
			printf(RED "Play\n\n" RESET);
			printf(WHITE "Quit\n\n" RESET);
		}
		break;

		case UI_OPTION_QUIT:
		{
			printf(WHITE "Play\n\n" RESET);
			printf(RED "Quit\n\n" RESET);
		}
		break;

		default:
			break;
	}
}

// game display
static void ui_print_card(int card)
{
	switch (card)
	{
		case 1:
			printf(RED "Ace" RESET);
			break;
		case 2:
			printf(GREEN "Two" RESET);
			break;
		case 3:
			printf(GREEN "Three" RESET);
			break;
		case 4:
			printf(GREEN "Four" RESET);
			break;
		case 5:
			printf(GREEN "Five" RESET);
			break;
		case 6:
			printf(GREEN "Six" RESET);
			break;
		case 7:
			printf(GREEN "Seven" RESET);
			break;
		case 8:
			printf(GREEN "Eight" RESET);
			break;
		case 9:
			printf(GREEN "Nine" RESET);
			break;
		case 10:
			printf(GREEN "Ten" RESET);
			break;
		case 11:
			printf(PURPLE "Jack" RESET);
			break;
		case 12:
			printf(BLUE "Queen" RESET);
			break;
		case 13:
			printf(YELLOW "King" RESET);
			break;
	}
}

static void ui_print_cards(player_data* player)
{
	for (int i = 0; i < player->index; ++i)
	{
		printf("* ");
		ui_print_card(player->hand[i]);
		printf("\n");
	}
}

static void ui_print_win(game_data* game)
{
	printf(CLEAR_SCREEN);
	ui_print_header(game);

	printf("%s%d\n", GREEN "You Win!" RESET " You got a score of ", game->userData.score);
	printf("The dealer got a score of: %d\n", game->dealerData.score);
	printf("\n");
}

static void ui_print_lose(game_data* game)
{
	printf(CLEAR_SCREEN);
	ui_print_header(game);

	printf("%s%d\n", RESET "Oh no! " RED "You Lost! " RESET "You got a score of ", game->userData.score);
	printf("The dealer got a score of: %d\n", game->dealerData.score);
	printf("\n");
}

static void ui_print_draw(game_data* game)
{
	printf(CLEAR_SCREEN);
	ui_print_header(game);

	printf("%s%d\n", YELLOW "It's a draw!" RESET " You got a score of ", game->userData.score);
	printf("The dealer also got a score of: %d\n", game->dealerData.score);
	printf("\n");
}

// game logic:
static void handle_main_menu(game_data* game, ui_data* ui)
{
	while (true)
	{
		ui_print_menu(game, ui);
		int ch = terminal_read_key();

		if (ch == 'q')
		{
			ui->isRunning = false;
			break;
		}

		else if (ch == KEY_UP)
			ui_cycle_option(ui, false);

		else if (ch == KEY_DOWN)
			ui_cycle_option(ui, true);

		else if (ch == KEY_ENTER)
		{
			if (ui->option == UI_OPTION_QUIT)
			{
				ui->isRunning = false;
				break;
			}
			else
			{
				ui->currentView = UI_VIEW_PLAYING;
				break;
			}
		}
	}
}

static void handle_playing(game_data* game, ui_data* ui)
{
	// Initialization:
	initialize_game(game);

	// Starter hands:
	game_draw_card(&game->deck, &game->userData);
	game_draw_card(&game->deck, &game->userData);

	game_draw_card(&game->deck, &game->dealerData);
	game_draw_card(&game->deck, &game->dealerData);

	// Player's turn:
	while (true)
	{
		printf(CLEAR_SCREEN);
		ui_print_header(game);
		printf("It's " RED "your " RESET "turn now\n\n");

		printf("Your cards are:\n");
		ui_print_cards(&game->userData);
		printf("%s%d" RESET "\n", "Total Sum: " YELLOW, game->userData.score);
		printf("\n");

		printf("The dealer cards are:\n");
		printf("* ");
		ui_print_card(game->dealerData.hand[0]);
		printf("\n* ?\n");

		printf("Press 'H' to Hit, and 'S' to Stand\n");
		printf("\n");

		int ch = terminal_read_key();

		if (ch == 'q')
		{
			game->result = GAME_RESULT_EXIT;
			ui->isRunning = false;
			break;
		}

		else if (ch == 'h' || ch == 'H')
			game_draw_card(&game->deck, &game->userData);

		else if (ch == 's' || ch == 'S')
			break;

		if (game->userData.score > 21)
		{
			game->result = GAME_RESULT_DEFEAT;
			break;
		}
		else if (game->userData.score == 21)
		{
			game->result = GAME_RESULT_VICTORY;
			break;
		}
	}

	if (game->result == GAME_RESULT_EXIT)
		return;

	else if (game->result == GAME_RESULT_DEFEAT)
	{
		++game->clientData.totalDefeats;
		ui_print_lose(game);
		return;
	}

	else if (game->result == GAME_RESULT_VICTORY)
	{
		++game->clientData.totalVictories;
		ui_print_win(game);
		return;
	}

	// Dealer's turn:
	while (game->dealerData.score < 17)
	{
		printf(CLEAR_SCREEN);
		ui_print_header(game);

		printf("Your cards are:\n");
		ui_print_cards(&game->userData);
		printf("%s%d" RESET "\n", "Total Sum: " YELLOW, game->userData.score);
		printf("\n");

		printf("The dealer cards are:\n");
		ui_print_cards(&game->dealerData);
		printf("%s%d" RESET "\n", "Total Sum: " YELLOW, game->dealerData.score);

		printf("Press any key to continue...");
		printf("\n");

		int ch = terminal_read_key();
		if (ch == 'q')
		{
			ui->isRunning = false;
			game->result = GAME_RESULT_EXIT;
			break;
		}

		if (game->dealerData.score > 21)
		{
			game->result = GAME_RESULT_VICTORY;
			break;
		}
		else if (game->dealerData.score == 21 || game->dealerData.score > game->userData.score)
		{
			game->result = GAME_RESULT_DEFEAT;
			break;
		}

		game_draw_card(&game->deck, &game->dealerData);
	}

	if (game->result == GAME_RESULT_EXIT)
		return;

	if (game->result == GAME_RESULT_VICTORY || game->dealerData.score > 21 || game->userData.score > game->dealerData.score)
	{
		++game->clientData.totalVictories;
		ui_print_win(game);
		return;
	}
	else if (game->result == GAME_RESULT_DEFEAT || game->userData.score < game->dealerData.score)
	{
		++game->clientData.totalDefeats;
		ui_print_lose(game);
		return;
	}

	++game->clientData.totalDraws;
	ui_print_draw(game);
}

static bool handle_replay()
{
	printf("Would you like to play another game? (y / n)\n");

	while (true)
	{
		int ch = terminal_read_key();
		if (ch == 'y')
			return true;
		else
			return false;
	}
}

static bool initialize()
{
	write(STDOUT_FILENO, CURSOR_HIDE, 6);
	setvbuf(stdout, NULL, _IONBF, 0);

	srand(time(NULL) + getpid());
	atexit(terminal_exit);

	if (!isatty(STDIN_FILENO))
		return false;

	terminal_enable_raw_mode();
	return true;
}

int main()
{
	// environment initialization:
	if (!initialize())
	{
		fprintf(stderr, "Failed to initialize terminal. Must be a TTY to work");
		return 1;
	}

	// data initialization:
	game_data game = {0};
	ui_data ui = {0};
	ui.isRunning = true;

	ui_print_menu(&game, &ui);

	while (ui.isRunning)
	{
		switch (ui.currentView)
		{
			case UI_VIEW_MAIN_MENU:
				handle_main_menu(&game, &ui);
				break;

			case UI_VIEW_PLAYING:
			{
				handle_playing(&game, &ui);
				if (!handle_replay())
					return 0;
			}
			break;
		}
	}

	return 0;
}
