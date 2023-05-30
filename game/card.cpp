#include "card.h"

bool Card::is_action() const
{
    return value == CardValue::Draw2 
        || value == CardValue::Reverse 
        || value == CardValue::Skip;
}

int Card::getScore() const
{
    return is_wild() ? WILD_CARD_SCORE : is_action() ? ACTION_CARD_SCORE : value;
}