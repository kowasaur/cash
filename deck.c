#include "deck.h"
#include <stdlib.h>
#include <time.h>

// initialise a standard (52 cards)
void init_deck(Deck* deck)
{
    static const char* ranks[] = {"Ace", "2", "3", "4", "5", "6", "7", "8", "9",
            "10", "Jack", "Queen", "King"};
    static const char* suits[] = {"Hearts", "Clubs", "Spades", "Diamonds"};
    int index = 0;

    for (int suit = 0; suit < 4; suit++) {
        for (int rank = 0; rank < 13; rank++) {
            deck->cards[index].rank = (char*)ranks[rank];
            deck->cards[index].suit = (char*)suits[suit];
            deck->cards[index].value = rank + 1; // numerical value
            index++;
        }
    }
    deck->top = 0; // reset to the top
}

// shuffle using fisher-yates algorithm
// https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
void shuffle_deck(Deck* deck)
{
    srand(time(NULL));
    for (int i = DECK_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }
}

// deal a card from the top
Card deal_card(Deck* deck)
{
    if (deck->top < DECK_SIZE) {
        return deck->cards[deck->top++];
    } else {
        // empty deck
        Card empty_card = {NULL, NULL, -1};
        return empty_card;
    }
}
