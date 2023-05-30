#include "game/uno_game.h"


class SimplePlayer : public UnoPlayer
{
    std::string my_name;
    std::vector<const Card*> my_cards;
    int currentSetNumber;

public:
    SimplePlayer(std::string name) : my_name(name), currentSetNumber(0) {};
    std::string name() const { return my_name; };

    void receiveCards(const std::vector<const Card*>& cards);
    const Card* playCard();
    bool drawAdditionalCard(const Card* additionalCard);
    CardColor changeColor();

    const Card* throwCard(int i);
    CardColor getMostPopularColor(std::set<CardColor> colors = { CardColor::Red, CardColor::Blue, CardColor::Yellow, CardColor::Green });
    bool checkAbilityToPutCard(const Card* card);

    int getIdCardWithMaxValue(std::vector<int> card_ids);
    int getIdCardWithMostPopularColor(std::vector<int> card_ids);

    std::vector<int> getCardsByColor(CardColor& color);
    std::vector<int> getCardsByValue(int value);
    std::vector<int> getWildCards();

    void printHand();
    };