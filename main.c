#include <stdio.h>
#include <termio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/time.h>

#define CLEAR_SCREEN "\033[2J"
#define ERASE_LINE "\33[2K\r"

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define WHITE "\e[0;37m"


#define DECK_SIZE 52

// terminal.h
static struct termios originalTerminalOS;

void disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTerminalOS);
}

void tty_exit(void)
{
    disable_raw_mode();
}

void enable_raw_mode()
{
    tcgetattr(STDIN_FILENO, &originalTerminalOS);
    atexit(tty_exit);

    struct termios raw = originalTerminalOS;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int kbhit (void) 
{
    struct timeval tv;
    fd_set rdfs;

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&rdfs);
    FD_SET (STDIN_FILENO, &rdfs);

    select(STDIN_FILENO + 1, &rdfs, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &rdfs);
}

int get_char()
{
    char ch = getchar();
}

// utils.h:
void swap(int* lhs, int* rhs)
{
    int temp = *lhs;
    *lhs = *rhs;
    *rhs = temp;
}

// game.h:
typedef struct game_data
{
    int deck[DECK_SIZE];
    int playerHand[13];
    int dealerHand[13];

    int deckIndex;
    int playerHandIndex;
    int dealerHandIndex;
} game_data;

static void initialize_deck(int* deck)
{
    int count = 0;
    for(int i = 1; i <= 13; ++i)
    {
        for(int j = 0; j < 4; ++j)
            deck[count++] = i;
    }
}

static void shuffle_deck(int* deck)
{
    for(int i = 0; i < DECK_SIZE; ++i)
    {
        int k = rand() % (i + 1);
        swap(&deck[i], &deck[k]);
    }
}

static int get_card_value(int card)
{
    if(card == 1)
        return 1;

    if(card >= 10)
        return 1;
    
    return card;
}

static void draw_card(int* deck, int* deckPosition, int* user, int* currentPosition)
{
    if(currentPosition >= 13)
        // TODO: fatal
        return;

    user[*currentPosition] = deck[*deckPosition];

    (*deckPosition)++;
    (*currentPosition)++;
}
draw_card(game->playerHand)

static void initialize(game_data* data)
{
    initialize_deck(data->deck);
    shuffle_deck(data->deck);

    data->deckIndex = 0;
    data->playerHandIndex = 0;
    data->dealerHandIndex = 0;
}

// display:
typedef enum ui_option
{
    UI_OPTION_PLAY = 0,
    UI_OPTION_QUIT = 1,
    UI_OPTION_AMOUNT = 2
} ui_option;

typedef struct ui_data
{
    ui_option option;
} ui_data;

static void ui_cycle_option(ui_data* ui)
{
    ui->option = (ui_option)(((int)ui->option + 1) % UI_OPTION_AMOUNT);
}

static void print_options()
{
    printf(CLEAR_SCREEN);
    printf("\n");
    printf(GREEN "-=-= >" RED " Blackjack " GREEN "< =-=-\n" RESET);
    printf("\n");
    printf("Use the W and S keys to switch through the options.\n");
    printf("Press E to select the option chosen.\n\n");
}

static void print_menu(ui_data* ui)
{
    print_options();

    switch (ui->option)
    {
        case UI_OPTION_PLAY:
        {
            printf(RED "Play\n\n" RESET);
            printf(WHITE "Quit\n\n" RESET);
        } break;
        
        case UI_OPTION_QUIT:
        {
            printf(WHITE "Play\n\n" RESET);
            printf(RED "Quit\n\n" RESET);
        } break;
    }
}

// game display

int main()
{
    if (!isatty(STDIN_FILENO)) 
        printf("Must be a TTY to work");

    game_data game = { 0 };
    ui_data ui = { 0 };

    initialize(&game);
    print_menu(&ui);

    enable_raw_mode();

    bool gameRunning = true;
    while(gameRunning)
    {
        while (!kbhit() && gameRunning);
        int ch = get_char();

        if(ch == 'q')
            return 1;

        else if(ch == 'e') // Enter
        {
            if(ui.option == UI_OPTION_QUIT)
            {
                return 1;
            }
            else
            {
                break;
            }
        }

        else if(ch == 'w')
            ui_cycle_option(&ui);

        else if(ch == 's')
            ui_cycle_option(&ui);

        disable_raw_mode();
        print_menu(&ui);
        enable_raw_mode();
    }

    // game
    int playerCard0 = draw_card(game->deck, game->deckIndex, game->playerHand, game->playerHandIndex);
    print("%d\n", playerCard0);
    playerCard0 = draw_card(game->deck, game->deckIndex, game->playerHand, game->playerHandIndex);
    print("%d\n", playerCard0);

    playerCard0 = draw_card(game->deck, game->deckIndex, game->playerHand, game->playerHandIndex);
    print("%d\n", playerCard0);
    playerCard0 = draw_card(game->deck, game->deckIndex, game->playerHand, game->playerHandIndex);
    print("%d\n", playerCard0);
    playerCard0 = draw_card(game->deck, game->deckIndex, game->playerHand, game->playerHandIndex);
    print("%d\n", playerCard0);

}
