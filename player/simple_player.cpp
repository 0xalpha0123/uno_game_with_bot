#include "my_player.h"
#include <map>

#include <iostream>
#include "utils/logger.h"

using namespace std;


void SimplePlayer::receiveCards(const std::vector<const Card*>& cards) {
	int gameNumber = game()->currentSetNumber();

	if (gameNumber != this->currentSetNumber) {
		this->my_cards.clear();
		this->currentSetNumber = gameNumber;
	}

	for (const auto& card : cards) {
		this->my_cards.emplace_back(card);
	}
}


const Card* SimplePlayer::throwCard(int i) {
	const Card* card = this->my_cards[i];
	this->my_cards.erase(this->my_cards.begin() + i);
	return card;
};

int SimplePlayer::getIdCardWithMaxValue(std::vector<int> card_ids) {
	int max_card_id;
	int max_value = -1;

	for (int i = 0; i < card_ids.size(); i++) {
		if (this->my_cards[card_ids[i]]->value > max_value) {
			max_value = this->my_cards[card_ids[i]]->value;
			max_card_id = card_ids[i];
		}
	}
	return max_card_id;
};


int SimplePlayer::getIdCardWithMostPopularColor(std::vector<int> card_ids) {
	std::set<CardColor> colors;
	for (int i = 0; i < card_ids.size(); i++) colors.insert(this->my_cards[card_ids[i]]->color);

	CardColor most_popular_color = getMostPopularColor(colors);

	for (int i = 0; i < card_ids.size(); i++) {
		if (this->my_cards[card_ids[i]]->color == most_popular_color) return card_ids[i];
	};
	return card_ids[0];
};


bool SimplePlayer::checkAbilityToPutCard(const Card* card) {
	const UnoGame* currentGame = game();
	if (card->value == currentGame->topCard()->value || card->color == currentGame->currentColor()) return true;
	return false;
}

std::vector<int> SimplePlayer::getCardsByColor(CardColor& color) {
	std::vector<int> card_ids;
	for (int i = 0; i < my_cards.size(); i++) {
		if (my_cards[i]->color == color && !my_cards[i]->is_wild()) card_ids.push_back(i);
	}
	return card_ids;
};

std::vector<int> SimplePlayer::getCardsByValue(int value) {
	std::vector<int> card_ids;
	for (int i = 0; i < my_cards.size(); i++) {
		if (my_cards[i]->value == value && !my_cards[i]->is_wild()) card_ids.push_back(i);
	}
	return card_ids;
};

std::vector<int> SimplePlayer::getWildCards() {
	std::vector<int> card_ids;
	for (int i = 0; i < my_cards.size(); i++) {
		if (my_cards[i]->is_wild()) card_ids.push_back(i);
	}
	return card_ids;
};


const Card* SimplePlayer::playCard() {
	const UnoGame* currentGame = game();
	const Card* topCard = currentGame->topCard();
	CardColor currentColor = currentGame->currentColor();
	std::vector<int> card_ids;

	card_ids = getCardsByColor(currentColor);
	if (card_ids.size() != 0) return throwCard(getIdCardWithMaxValue(card_ids));
	if (topCard->is_wild()) return throwCard(getIdCardWithMaxValue(getWildCards()));

	card_ids = getCardsByValue(topCard->value);
	if (card_ids.size() != 0) return throwCard(getIdCardWithMostPopularColor(card_ids));
	return throwCard(getIdCardWithMaxValue(getWildCards()));
};

bool SimplePlayer::drawAdditionalCard(const Card* additionalCard) {
	if (checkAbilityToPutCard(additionalCard)) return true;
	else {
		const std::vector<const Card*> newCart = { additionalCard };
		receiveCards(newCart);
		return false;
	}
};

CardColor SimplePlayer::changeColor() {
	return getMostPopularColor();
};

CardColor SimplePlayer::getMostPopularColor(std::set<CardColor> colors) {
	std::map<CardColor, unsigned> cardCount;

	for (CardColor color : colors) cardCount[color] = 0;

	for (int i = 0; i < this->my_cards.size(); i++) {
		if (!this->my_cards[i]->is_wild()) {
			if (cardCount.find(this->my_cards[i]->color) != cardCount.end()) {
				cardCount[this->my_cards[i]->color] += 1;
			}
		}
	}

	unsigned max_count = 0;
	CardColor most_popular_color;

	for (auto& item : cardCount)
	{
		if (item.second > max_count) {
			max_count = item.second;
			most_popular_color = item.first;
		}
	}

	return most_popular_color;
};


void SimplePlayer::printHand() {
	for (int i = 0; i < my_cards.size(); i++) {
		printCard(cout, my_cards[i]);
		cout << "; ";
	}
}