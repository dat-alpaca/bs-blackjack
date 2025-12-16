#include <stdio.h>
#include <termio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
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
    return ch;
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

    int playerScore;
    int dealerScore;
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

    else if(card >= 10)
        return 10;
    
    return card;
}

static void draw_card(int* deck, int* deckPosition, int* user, int* currentPosition)
{
    if(*currentPosition >= 13)
        // TODO: fatal
        return;

    user[*currentPosition] = deck[*deckPosition];

    (*deckPosition)++;
    (*currentPosition)++;
}

static void initialize(game_data* data)
{
    srand(time(NULL));

    initialize_deck(data->deck);
    shuffle_deck(data->deck);

    data->dealerScore       = 0;
    data->playerScore       = 0;
    data->deckIndex         = 0;
    data->playerHandIndex   = 0;
    data->dealerHandIndex   = 0;
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

static void print_header()
{
    printf("\n");
    printf(GREEN "-=-= >" RED " Blackjack " GREEN "< =-=-\n" RESET);
    printf("\n");
}

static void print_options()
{
    printf(CLEAR_SCREEN);
    print_header();
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
static void print_card(int card)
{
    switch(card)
    {
        case 1:
            printf("Ace");
            break;

        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            printf("Card %d", card);
            break;
        
        case 11:
            printf("Jack");
            break;

        case 12:
            printf("Queen");
            break;

        case 13:
            printf("King");
            break;
    }
}

static void print_cards(int* hand, int index)
{
    for(int i = 0; i < index; ++i)
    {
        printf("* ");
        print_card(hand[i]);
        printf("\n");
    }
}

static void print_win(game_data* data)
{
    disable_raw_mode();
    printf("%s%d\n", GREEN "You Win!" RESET " You got a score of ", data->playerScore);
    printf("The dealer got a score of: %d\n", data->dealerScore);
}

static void print_lose(game_data* data)
{
    disable_raw_mode();
    printf("%s%d\n", RED "Oh no!" RESET " You got a score of ", data->playerScore);
}

int main()
{
    if (!isatty(STDIN_FILENO)) 
        printf("Must be a TTY to work");

    game_data game = { 0 };
    ui_data ui = { 0 };

    initialize(&game);
    print_menu(&ui);

    enable_raw_mode();

    while(1)
    {
        while (!kbhit());
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

    disable_raw_mode();
    printf(CLEAR_SCREEN);
    print_header();

    // game
    draw_card(game.deck, &game.deckIndex, game.playerHand, &game.playerHandIndex);
    draw_card(game.deck, &game.deckIndex, game.dealerHand, &game.dealerHandIndex);
    draw_card(game.deck, &game.deckIndex, game.playerHand, &game.playerHandIndex);
    draw_card(game.deck, &game.deckIndex, game.dealerHand, &game.dealerHandIndex);

    game.playerScore += get_card_value(game.playerHand[0]);
    game.playerScore += get_card_value(game.playerHand[1]);
    game.dealerScore += get_card_value(game.dealerHand[0]);
    game.dealerScore += get_card_value(game.dealerHand[1]);

    // Game starts:
    printf(CLEAR_SCREEN);
    print_header();

    printf("Your cards are:\n");
    print_cards(game.playerHand, game.playerHandIndex);
    printf("Which sums up to: %d\n\n", game.playerScore);
    printf("Press 'H' to Hit, and 'S' to Stand\n");

    int option = 0;
    while(1)
    {
        // TODO; print round
        enable_raw_mode();
        while (!kbhit());
        int ch = get_char();
        disable_raw_mode();

        if(ch == 'q')
        {
            printf(CLEAR_SCREEN);
            return 1;
        }

        if(ch == 'h' || ch == 'H')
        {
            draw_card(game.deck, &game.deckIndex, game.playerHand, &game.playerHandIndex);
            game.playerScore += get_card_value(game.playerHand[game.playerHandIndex - 1]);
            option = 1;
        }

        if(ch == 's'|| ch == 'S')
        {
            option = 2;
            break;
        }
    
        printf(CLEAR_SCREEN);
        print_header();

        if(option == 1)
        {
            printf("You have decided to hit... And you drew... ");
            print_card(game.playerHand[game.playerHandIndex - 1]);
            printf("\n\n");
        }

        printf("Your cards are:\n");
        print_cards(game.playerHand, game.playerHandIndex);
        printf("Which sums up to: %d\n\n", game.playerScore);
        printf("Press 'H' to Hit, and 'S' to Stand\n");

        if(game.playerScore > 21)
        {
            print_lose(&game);
            return 0;
        }
    }

    printf(CLEAR_SCREEN);
    print_header();
    printf("You have decided to stand. Now is the dealer's turn...\n");

    while(game.dealerScore < 17)
    {
        printf("The dealer draws a... ");

        draw_card(game.deck, &game.deckIndex, game.dealerHand, &game.dealerHandIndex);
        game.dealerScore += get_card_value(game.dealerHand[game.dealerHandIndex - 1]);

        print_card(game.dealerHand[game.dealerHandIndex - 1]);
        printf("So his score is now %d", game.dealerScore);

        if(game.dealerScore > 21)
        {
            print_win(&game);
            return 1;
        }    
    }
}
