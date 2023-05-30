#pragma once

#include <ostream>
#include <stdexcept>
#include <vector>

#include "../game/uno_game.h"


class StatsObserver : public Observer
{

public:
    template<class T>
    using Table = std::vector<std::vector<T>>;

    /// @brief Структура, хранящая базовую статистическую информацию
    /// о многомерном наборе данных. Возвращается методами `getWinsMV` и `getScoresMV`. 
    struct MV
    {
        /// @brief вектор средних значений
        std::vector<double> mean;

        /// @brief матрица ковариации
        std::vector<std::vector<double>> var;
    };

private: 
    /// @brief scores[i][j] — очки `i`-того игрока в `j`-тую игру
    Table<int> scores;
    /// @brief scores[i][j] — true, если `i`-тый игрок выиграл `j`-тую игру
    Table<bool> wins;

    /// @brief указатель на игру, за которой наблюдает объект
    const UnoGame * game;

    /// @brief печать таблицы в поток вывода
    template<class T>
    void printTSV(const Table<T>&, std::ostream&) const;

    /// @brief подсчет среднего и матрицы ковариации
    template<class T>
    MV getMV(const Table<T>&) const;

    void assureIsAllocated();
    void registerWin(int winnerIndex);
public:
    StatsObserver(const UnoGame* game);

    /// @brief резервирует место во всех векторах
    void reserve(int numberOfPlayers, size_t numberOfGames);


    // Методы наблюдателя
     
    void handlePlayerWonGame(int playerIndex, int totalScore) override 
        { registerWin(playerIndex); }

    void handleSetsLimitReached(int winnerIndex, int winnerScore) override 
        { registerWin(winnerIndex); }

    const Table<int>& getScores() const { return scores; }
    const Table<bool>& getWins() const  { return wins;   }

    /// @brief вывод количества очков за все игры в формате TSV в поток вывода
    void printScoresTSV(std::ostream& out) const { return printTSV(scores, out); }

    /// @brief вывод победивших игроков за все игры в формате TSV в поток вывода
    void printWinsTSV(std::ostream& out) const   { return printTSV(wins, out);   }

    /// @brief подсчет среднего и матрицы ковариации для очков игроков
    MV getScoresMV() const { return getMV(scores); } 

    /// @brief подсчет среднего и матрицы ковариации для побед игроков
    MV getWinsMV() const   { return getMV(wins);   }
};


/// @brief Запуск `numberOfGames` игр подряд с подсчетом статистики.
/// Предполагается, что в `game` уже добавлены все игроки.
/// После каждой игры игроки меняются местами.
StatsObserver runGames(UnoGame& game, int numberOfGames);

/// @brief Расчет среднего значения последовательности
/// @throws std::underflow_error если begin == end
template<class iterator>
double mean(iterator begin, iterator end);

/// @brief Расчет ковариации двух последовательностей
/// @throws std::underflow_error если begin == end
template<class iterator>
double cov(iterator begin1, iterator end1, iterator begin2);

template<class iterator>
double mean(iterator begin, iterator end)
{
    size_t count = 0;
    double sum = 0;
    for(; begin != end; ++begin, ++count) 
        sum += *begin;
    if (count == 0) throw std::underflow_error("Cannot calculate covariation for empty sequence");
    else return sum / count;
}

template<class iterator>
double cov(iterator begin1, iterator end1, iterator begin2)
{
    size_t count = 0;
    double sum1 = 0, sum2 = 0, sumProd = 0;
    for (; begin1 != end1; ++begin1, ++begin2, ++count)
    {
        sum1    += *begin1;
        sum2    += *begin2;
        sumProd += *begin1 * *begin2;
    }
    if (count == 0) throw std::underflow_error("Cannot calculate covariation for empty sequence");
    return sumProd / count - (sum1 / count) * (sum2 / count);
}

template<class T>
StatsObserver::MV StatsObserver::getMV(const Table<T>& table) const
{
  std::vector<double> meanVector(table.size());
  std::vector<std::vector<double>> var(table.size(), std::vector<double>(table.size()));
  
  for (int i = 0; i < table.size(); ++i)
  {
      meanVector[i] = mean(table[i].begin(), table[i].end());
      for (int j = i; j < table.size(); ++j) 
          var[i][j] = var[j][i] = 
              cov(table[i].begin(), table[i].end(), table[j].begin());
  }

  return { meanVector, var };
}

template<class T>
void StatsObserver::printTSV(const Table<T>& table, std::ostream& out) const 
{
    for (int i = 0; i < table.size(); ++i)
        out << "Player" << i << '\t';
    out << '\n';
    for (int j = 0; j < table.front().size(); ++j) 
    {
        for (int i = 0; i < table.size(); ++i)
            out << table[i][j] << '\t';
        out << '\n';
    }
}
