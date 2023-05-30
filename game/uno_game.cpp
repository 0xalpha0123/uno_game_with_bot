#include "uno_game.h"

void UnoPlayer::addToGame(
    int playerIndex, 
    const UnoGame *currentGame, 
    MessageQueue *queue)
{
    this->currentGame = currentGame;
    this->playerIndex_ = playerIndex;
    this->messageQueue = queue;
}

bool UnoPlayer::say(const std::string &message)
{
    if (messageQueue == nullptr) return false;
    return messageQueue->addMessage(playerIndex(), message);
}

UnoGame::UnoGame():
    messageQueue(DEFAULT_MESSAGE_QUEUE_LIMIT),
    players(),
    currentDirection_(),
    currentColor_(),
    deck(),
    discardPile(),
    activePlayerIndex_(-1),
    currentSetScore_(0),
    playerInfo(),
    randomEngine(),
    broadcaster(nullptr),
    setsLimit(DEFAULT_SETS_LIMIT),
    turnsLimit(DEFAULT_TURNS_LIMIT),
    currentDiller_(0)
{
    players.reserve(MAX_NUMBER_OF_PLAYERS);
    playerInfo.reserve(MAX_NUMBER_OF_PLAYERS);
    deck.reserve(DECK_SIZE);
    discardPile.reserve(DECK_SIZE);
    broadcaster.setQueue(&messageQueue);
    prepareDeck();
}

UnoGame::~UnoGame()
{
    clearDeck();
}

const Card *UnoGame::topCard() const
{
    if (discardPile.empty()) return nullptr;
    return discardPile.back();
}

std::vector<int> UnoGame::numberOfCards() const
{
    std::vector<int> result;
    std::transform(
        playerInfo.begin(), 
        playerInfo.end(), 
        std::back_inserter(result),
        [](const PlayerInfo& info) { return info.hand.size(); });
    return result;
}

std::vector<int> UnoGame::currentScores() const
{
    std::vector<int> result;
    std::transform(
        playerInfo.begin(), 
        playerInfo.end(), 
        std::back_inserter(result),
        [](const PlayerInfo& info) { return info.currentScore; });
    return result;
}

int UnoGame::cardsLeft() const
{
    return deck.size();
}

void UnoGame::addPlayer(UnoPlayer *player)
{
    if (player == nullptr) return;
    if (numberOfPlayers() >= MAX_NUMBER_OF_PLAYERS)
        throw std::overflow_error("Maximum number of players reached");
    
    // Сообщаем игроку необходимую информацию об игре
    player->addToGame(players.size(), this, &messageQueue);
    
    // Сохраняем игрока
    players.push_back(player);

    // Сохраняем информацию об игроке
    playerInfo.push_back(PlayerInfo());

    // Подписываем игрока на игровые события
    broadcaster.addListener(player);
}

void UnoGame::addObserver(Observer *observer)
{
    if (observer == nullptr) return;
    broadcaster.addListener(observer);
}

void UnoGame::shufflePlayers()
{
    std::vector<int> permutation(numberOfPlayers());
    for (int i = 0; i < numberOfPlayers(); i++) permutation[i] = i;
    std::shuffle(permutation.begin(), permutation.end(), randomEngine);
    shufflePlayers_(permutation);
}

void UnoGame::shufflePlayers(const std::vector<int> &permutation)
{
    if (permutation.size() > numberOfPlayers()) 
        throw std::out_of_range("Permutation is too long");
    for (int x : permutation)
        if (x < 0 || x >= numberOfPlayers())
            throw std::out_of_range("Invalid value in permutation");
    shufflePlayers_(permutation);
}

void UnoGame::setCurrentDiller(int diller) 
{
  if (numberOfPlayers() == 0) {
    currentDiller_ = 0;
    return;
  }
  currentDiller_ = diller % numberOfPlayers();
}

void UnoGame::setMessageQueueSizeLimit(int m)
{
    messageQueue.setLimit(m);
}

void UnoGame::setRandomGeneratorSeed(unsigned seed)
{
    randomEngine.seed(seed);
}

void UnoGame::initPlayerInfo()
{
    for (PlayerInfo& info: playerInfo) info.currentScore = 0;
    for (UnoPlayer * player : players) 
        broadcaster.handlePlayerEntered(player->playerIndex(), player->name());
    currentSetNumber_ = 0;
    moveToDeck();
}

std::tuple<int, int> UnoGame::runSet()
{
    return runSet_();
}

std::tuple<int, int> UnoGame::runGame()
{
    initPlayerInfo();
    
    int winner = -1, score = 0; 

    setCurrentDiller(0);
    
    do
    {
        if (setsLimit > 0 && currentSetNumber_ >= setsLimit)
        {
            std::tie(winner, score) = findWinner();
            broadcaster.handleSetsLimitReached(winner, score);
            return std::make_tuple(winner, score);
        }
        std::tie(winner, score) = runSet_();
        stepCurrentDiller();
        if (winner < 0) continue;
    } 
    while(playerInfo.at(winner).currentScore < WINNING_SCORE);
    
    score = playerInfo.at(winner).currentScore;
    
    broadcaster.handlePlayerWonGame(winner, score);

    return std::make_tuple(winner, score);
}

void UnoGame::stepCurrentDiller() 
{
  if (numberOfPlayers() == 0) return;
  currentDiller_ = (currentDiller_ + 1) % numberOfPlayers();
}

std::vector<const Card *> UnoGame::chooseCards(const UnoPlayer *player, int numberOfCards)
{
    return std::vector<const Card *>();
}

const Card *UnoGame::chooseFirstCard()
{
    return nullptr;
}

void UnoGame::shufflePlayers_(const std::vector<int> &permutation)
{
    int i = 0;
    for (int j : permutation) 
    {
        std::iter_swap(players.begin() + i, players.begin() + j);
        if (!playerInfo.empty())
            std::iter_swap(playerInfo.begin() + i, playerInfo.begin() + j);
        ++i;
    }
}

void UnoGame::prepareDeck()
{
    std::vector<CardColor> colors = 
        {CardColor::Blue, CardColor::Green, CardColor::Red, CardColor::Yellow};
    std::vector<int> doubles = {CardValue::Draw2, CardValue::Skip, CardValue::Reverse};
    for (int val = 1; val <= 9; ++val) doubles.push_back(val);
    for (CardColor c : colors)
    {
        for (int val : doubles) 
        {
            deck.push_back(new Card(c, val));
            deck.push_back(new Card(c, val));
        }
        deck.push_back(new Card(c, 0));
    }
    for (int i = 0; i < 4; ++i)
    {
        // Цвет может быть любой
        deck.push_back(new Card(CardColor::Blue, CardValue::Wild));
        deck.push_back(new Card(CardColor::Blue, CardValue::WildDraw4));
    }
}

void UnoGame::moveToDeck()
{
    clearHands();
    deck.insert(deck.cend(), discardPile.begin(), discardPile.end());
    discardPile.clear();
}

void UnoGame::shuffleDeck()
{
    std::shuffle(deck.begin(), deck.end(), randomEngine);
}

void UnoGame::clearDeck()
{
    moveToDeck();
    for (const Card * c : deck) delete c;
}

void UnoGame::clearHands()
{
    for (auto & info : playerInfo)
    {
        deck.insert(deck.cend(), info.hand.begin(), info.hand.end());
        info.hand.clear();
    }
}

std::vector<const Card *> UnoGame::getCardsFromDeck(const UnoPlayer *player, int numberOfCards)
{
    if (player == nullptr || numberOfCards == 0) 
        return std::vector<const Card*>();
    if (numberOfCards > deck.size() + discardPile.size() - 1)
    {
        // Выдать такое количество карт физически невозможно, поэтому выдаем,
        // сколько можем
        return getCardsFromDeck(player, deck.size() + discardPile.size() - 1);   
    }
    if (numberOfCards > deck.size())
    {
        // Надо переместить из стопки сброса в колоду все карты кроме верхней 
        // и перемешать
        flushDiscardPile();
        broadcaster.handleDeckShuffled();
    }
    auto forPlayer = chooseCards(player, numberOfCards);
    // По умолчанию выдаем с конца колоды
    if (forPlayer.empty()) 
    {
        forPlayer.insert(forPlayer.cend(), 
            deck.rbegin(), deck.rbegin() + numberOfCards);   
        deck.resize(deck.size() - numberOfCards);
    }
    // Переопределенное поведение
    else 
    {
        if (forPlayer.size() != numberOfCards) 
            throw std::length_error("Invalid number of cards");
        std::vector<const Card*> newDeck;
        newDeck.reserve(DECK_SIZE);
        std::copy_if(deck.begin(), deck.end(), std::back_inserter(newDeck),
            [&forPlayer](const Card* card) {
            return std::find(forPlayer.begin(), forPlayer.end(), card) == forPlayer.end();
        });
        if (newDeck.size() != deck.size() - numberOfCards)
            throw std::domain_error("Invalid values in choosen hand");
        deck = std::move(newDeck);
    }
    auto & playerHand = playerInfo.at(player->playerIndex()).hand;
    playerHand.insert(playerHand.cend(), forPlayer.begin(), forPlayer.end());
    return forPlayer;
}

bool UnoGame::dealCards(UnoPlayer *player, int numberOfCards)
{
    auto forPlayer = getCardsFromDeck(player, numberOfCards);
    if (forPlayer.empty()) return false;
    player->receiveCards(forPlayer);
    return forPlayer.size() == numberOfCards;
}

void UnoGame::placeFirstCard()
{
    do {
        const Card * firstCard = chooseFirstCard();
        // Поведение по умолчанию
        if (firstCard == nullptr) 
        {
            discardPile.push_back(deck.back());
            deck.pop_back();
        }
        // переопределенное поведение
        else 
        {
            auto cardEntry = std::find(deck.begin(), deck.end(), firstCard);
            if (cardEntry == deck.end()) 
                throw std::domain_error("Invalid first card value");
            discardPile.push_back(*cardEntry);
            deck.erase(cardEntry);
        }
    // Карта не может быть "Возьми 4", так что пытаемся еще раз, если попалась она.
    } while(topCard()->value == CardValue::WildDraw4);
    
    // Если вытащили не с первого раза, замешиваем карты из сброса в колоду 
    if (discardPile.size() > 1)
        flushDiscardPile();
}

std::tuple<int, int> UnoGame::runSet_()
{
    // Проверим, что игроков хотя бы 2
    if (numberOfPlayers() < MIN_NUMBER_OF_PLAYERS)
        throw std::underflow_error("Too few players!");

    // Инициализация
    activePlayerIndex_ = currentDiller_;
    currentSetScore_ = 0;
    currentDirection_ = GameDirection::Direct;
    currentTurnNumber_ = 0;
    
    ++currentSetNumber_;

    // Подготовка колоды
    moveToDeck();

    // Сообщаем о том, что началась партия
    broadcaster.handleSetStarted(currentSetNumber_);
    
    shuffleDeck();
    broadcaster.handleDeckShuffled();

    // Подготовка игроков

    // Список игроков этой партии
    // из этого списка могут исключаться игроки при дисквалификации
    std::list<UnoPlayer*> set_players(players.begin(), players.end());

    // Каждому раздаем по 7 карт
    for (UnoPlayer * player : set_players) 
    {
        dealCards(player, INITIAL_CARDS_NUMBER);
        broadcaster.handlePlayerDealt(player->playerIndex(), INITIAL_CARDS_NUMBER);
    }
    
    // В сброс помещается карта из колоды
    placeFirstCard();
    if (!topCard()->is_wild()) currentColor_ = topCard()->color;
    broadcaster.handleFirstCardPlaced(topCard());

    // Если лежит "Закажи цвет", то первый игрок заказывает цвет
    if (topCard()->value == CardValue::Wild)
    {
        CardColor newColor = activePlayer()->changeColor();
        currentColor_ = newColor;
        broadcaster.handlePlayerChangedColor(activePlayerIndex_, newColor);
    }
    
    // Если лежит "Обратный ход", то меняется направление игры
    if (topCard()->value == CardValue::Reverse)
    {
        currentDirection_ = GameDirection::Inverse;
        broadcaster.handleDirectionChanged(currentDirection_);
    }

    // Должно ли выполняться действие карты действий 
    // Действие не выполняется для игрока, который ходит после
    // дисквалифицированного игрока или после игрока, пропустившего ход
    bool actionShouldApply = true;
    
    // Основной игровой цикл
    while (true)
    {
        currentTurnNumber_++;
        if (turnsLimit > 0 && currentTurnNumber_ >= turnsLimit)
        {
            broadcaster.handleTurnsLimitReached();
            return std::make_tuple(-1, 0);
        }
        // Должен ли игрок пропустить ход
        bool shouldSkip = false;
        int additionalCards = 0;
        // Применяем действие карты на игрока
        if (actionShouldApply)
        {
            switch (topCard()->value)
            {
                case CardValue::Draw2:
                    shouldSkip = true;
                    additionalCards = 2;
                    break;
                case CardValue::Skip:
                    shouldSkip = true;
                    break;
                case CardValue::WildDraw4:
                    shouldSkip = true;
                    additionalCards = 4;
                    break;
            }
        }
        // Игрок берет карты пропускает ход
        if (shouldSkip)
        {
            if (additionalCards > 0) 
                dealCards(activePlayer(), additionalCards);
            broadcaster.handlePlayerDrewAndSkip(activePlayerIndex_, additionalCards);
            // Переход к следующему игроку
            activePlayerIndex_ = nextPlayerIndex(set_players);
            // На следующего игрока верхняя карта не действует
            actionShouldApply = false;
            continue;
        }
        // Карты активного игрока
        auto & hand = playerInfo.at(activePlayerIndex_).hand;

        const Card * newCard = nullptr;

        // Если игрок может положить карту, то у него запрашивается карта
        if (canPlaceCard(activePlayer(), topCard()))
            newCard = activePlayer()->playCard();
        // Если игрок не может положить карту, он тянет еще одну из колоды
        else 
        {
            // Если в колоде и сбросе есть хотя бы одна карта, кроме верхней 
            // карты сброса
            if (deck.size() + discardPile.size() > 1)
            {
                const Card * additionalCard = getCardsFromDeck(activePlayer(), 1).at(0);
                // Спрашиваем игрока, хочет ли он такую карту положить
                bool place = activePlayer()->drawAdditionalCard(additionalCard);
                // Если да, то кладем ее
                if (place) newCard = additionalCard;
                // Если нет, то игрок пропускает ход
                else shouldSkip = true;
            }
            // Если игроку неоткуда тянуть карту, то он просто пропускает ход
            else shouldSkip = true;
            broadcaster.handlePlayerDrewAnotherCard(activePlayerIndex_);
        }
        if (shouldSkip) 
        {
            activePlayerIndex_ = nextPlayerIndex(set_players);
            actionShouldApply = false;
            continue;
        }

        // Пытаемся найти карту, которую положил игрок у него на руках
        
        // Итератор, указывающий на карту в руке игрока или hand.end(), если 
        // такой карты нет.
        auto handEntry = std::find_if(
            hand.begin(), hand.end(), 
            [newCard](const Card * card) { 
                return newCard != nullptr
                    && card->color == newCard->color 
                    && card->value == newCard->value;
            }); 
        bool shouldDisqualify = newCard == nullptr 
            || !newCard->is_valid()
            || handEntry == hand.end()  // такой карты у него на руках нет
            || (haveMatchingColor(activePlayer(), currentColor_) 
                && newCard->value == CardValue::WildDraw4)
            || (!newCard->is_wild()
                && newCard->value != topCard()->value 
                && newCard->color != currentColor_);
        
        if (shouldDisqualify)
        {
            // Дисквалификация игрока

            // Считаем его очки и добавляем в текущий выигрыш
            int score = countHandScore(activePlayerIndex_);
            int disqualifiedPlayer = activePlayerIndex_;
            currentSetScore_ += score;
            // Определяем следующего игрока
            int tempActivePlayerIndex = nextPlayerIndex(set_players);
            set_players.erase(activePlayerIterator(set_players));
            broadcaster.handlePlayerDisqualified(
                disqualifiedPlayer, score, newCard);
            activePlayerIndex_ = tempActivePlayerIndex;
            if (set_players.size() == 1) break;
            actionShouldApply = false;
            continue;
        }

        newCard = *handEntry;
        // Положить newCard в discardPile.
        discardPile.push_back(newCard);
        
        // Убрать из руки игрока newCard.
        hand.erase(handEntry);

        if (!newCard->is_wild()) currentColor_ = newCard->color;

        broadcaster.handleCardPlayed(activePlayerIndex_, newCard);
        
        // Если у игрока не осталось карт — выйти из цикла
        if (hand.empty()) break;

        // Если newCard — дикая, то спросить новый цвет
        if (newCard->is_wild()) 
        {
            CardColor newColor = activePlayer()->changeColor();
            currentColor_ = newColor;
            broadcaster.handlePlayerChangedColor(activePlayerIndex_, newColor);
        }

        // Если newCard — "Обратный ход", то сменить направление.
        if (newCard->value == CardValue::Reverse)
        {
            currentDirection_ = currentDirection_ == GameDirection::Direct
                ? GameDirection::Inverse
                : GameDirection::Direct;
            broadcaster.handleDirectionChanged(currentDirection_);
        }

        // Перейти к следующему игроку 
        activePlayerIndex_ = nextPlayerIndex(set_players);
        actionShouldApply = true;
    }
    // Для всех игроков в set_players посчитать очки
    for (UnoPlayer * player : set_players)
    {
        if (activePlayerIndex_ == player->playerIndex()) continue;
        currentSetScore_ += countHandScore(player->playerIndex());
    }
    playerInfo.at(activePlayerIndex_).currentScore += currentSetScore_;
    // вернуть итоги
    broadcaster.handlePlayerWonSet(activePlayerIndex_, currentSetScore_);
    return std::make_tuple(activePlayerIndex_, currentSetScore_);
}

void UnoGame::flushDiscardPile()
{
    if (discardPile.size() < 2) return;
    // Переносим из стопки сброса в колоду все карты, кроме верхней
    deck.insert(deck.cend(), discardPile.begin(), discardPile.end() - 1);
    // Перемещаем верхнюю карту в начало
    std::swap(discardPile.back(), discardPile.front());
    discardPile.resize(1);
    shuffleDeck();
}

UnoPlayer *UnoGame::activePlayer()
{
    return players.at(activePlayerIndex_);
}

std::list<UnoPlayer *>::iterator UnoGame::activePlayerIterator(
    std::list<UnoPlayer *>& playersList)
{
    int current = activePlayerIndex_;
    return std::find_if(
        playersList.begin(), 
        playersList.end(),
        [current](UnoPlayer * player) { return player->playerIndex() == current; });
}

int UnoGame::nextPlayerIndex(std::list<UnoPlayer *>& playersList)
{
    auto it = activePlayerIterator(playersList);
    if (currentDirection_ == GameDirection::Direct)
    {
        ++it;
        if (it == playersList.end()) it = playersList.begin();
    }
    else if (it == playersList.begin()) it = --playersList.end();
    else --it;
    return (*it)->playerIndex();
}

bool UnoGame::canPlaceCard(UnoPlayer *player, const Card *topCard_)
{
    auto& hand = playerInfo.at(player->playerIndex()).hand;
    if (haveMatchingColor(player, currentColor_)) return true;
    return std::any_of(
        hand.begin(), hand.end(), 
        [topCard_](const Card * card) { 
            return card->is_wild() || card->value == topCard_->value;
        });
}

bool UnoGame::haveMatchingColor(UnoPlayer *player, CardColor color)
{
    auto& hand = playerInfo.at(player->playerIndex()).hand;
    return std::any_of(
        hand.begin(), hand.end(), 
        [color](const Card * card) { 
            return !card->is_wild() && card->color == color; 
        });
}

int UnoGame::countHandScore(int playerIndex)
{
    int sum = 0;
    for (const Card* card: playerInfo.at(playerIndex).hand)
        sum += card->getScore();
    return sum;
}

std::tuple<int, int> UnoGame::findWinner()
{
    auto scores = currentScores();
    auto maxScoreIterator = std::max_element(scores.begin(), scores.end());
    int maxScore = *maxScoreIterator;
    int maxScorePlayers = std::count(scores.begin(), scores.end(), maxScore);
    if (maxScorePlayers > 1) return std::make_tuple(-1, maxScore);
    return std::make_tuple(maxScoreIterator - scores.begin(),  maxScore);
}

bool UnoGame::deckIsConsistent()
{
    std::set<const Card*> deckSet(deck.begin(), deck.end());
    return deckSet.size() == deck.size();
}

void UnoGame::EventBroadcaster::flushMessages()
{
    if (queue == nullptr) return;
    int player;
    std::string message;
    for (auto it = queue->cbegin(); it != queue->cend(); ++it)
    {
        std::tie(player, message) = *it;
        handlePlayerSaid(player, message);
    }
    if (queue->hasOverflow()) handleMessageOverflow();
    queue->clear();
}

void UnoGame::EventBroadcaster::afterEach(GameEvent event)
{
    if (event == GameEvent::PlayerSaid || event == GameEvent::MessageOverflow)
        return;
    flushMessages();
}

UnoGame::EventBroadcaster::EventBroadcaster(MessageQueue *messageQueue):
    queue(messageQueue), listeners()
{
}

void UnoGame::EventBroadcaster::addListener(Observer *listener)
{
    if (listener == nullptr) return;
    listeners.push_back(listener);
}
