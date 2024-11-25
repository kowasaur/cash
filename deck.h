#ifndef DECK_H
#define DECK_H

#define DECK_SIZE 52

typedef struct {
    char* rank; // card ranks - "Ace", "King", "2" etc
    char* suit; // card suit "Hearts" etc
    int value; // numerical value
} Card;

typedef struct {
    Card cards[DECK_SIZE];
    int top; // index of next card to deal
} Deck;

// function prototypes

void init_deck(Deck* deck);
void shuffle_deck(Deck* deck);
Card deal_card(Deck* deck);

#endif
