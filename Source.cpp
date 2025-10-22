#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>


const int PAYOFF[2][2] = {
    {3, 0},  // A:0 B:0 = 3; A:0 B:1 = 0
    {5, 1}   // A:1 B:0 = 5; A:1 B:1 = 1
};

// ��� ���������
class Strategy {
public:
    virtual ~Strategy() = default;
    virtual int play(const std::vector<std::pair<int, int>>& history) = 0;//���� �������� ������� ,�������
    virtual std::string getName() const = 0;
    virtual void reset() {}
};

// Alex - ������ 1
class Alex : public Strategy {
public:
    int play(const std::vector<std::pair<int, int>>& history) override {
        return 1;
    }
    std::string getName() const override { return "Alex"; }
};

// Bob - ������ 0
class Bob : public Strategy {
public:
    int play(const std::vector<std::pair<int, int>>& history) override {
        return 0;
    }
    std::string getName() const override { return "Bob"; }
};

// Clara - ������� 0, ����� ��������� �������� ���������
class Clara : public Strategy {
public:
    int play(const std::vector<std::pair<int, int>>& history) override {
        if (history.empty()) return 0;//������� ���������
        return history.back().second;//����� ���� ��������� ��� ���������
    }
    std::string getName() const override { return "Clara"; }
};

// Denis - ������� 0, ����� �������� � ���������� ������ ���������
class Denis : public Strategy {
public:
    int play(const std::vector<std::pair<int, int>>& history) override {
        if (history.empty()) return 0;//���������
        return 1 - history.back().second;//��������������� � ���������
    }
    std::string getName() const override { return "Denis"; }
};

// Emma - ������ 0, �� ������ 20 ������ - 1
class Emma : public Strategy {
public:
    int play(const std::vector<std::pair<int, int>>& history) override {
        int round = history.size() + 1;
        return (round % 20 == 0) ? 1 : 0;//������ 20 ������ = �����������
    }
    std::string getName() const override { return "Emma"; }
};

// Frida - 0 �� ��� ���, ���� � ��������� 0, ����� 1
class Frida : public Strategy {
    bool opponent_ever_defected;
public:
    Frida() : opponent_ever_defected(false) {}

    int play(const std::vector<std::pair<int, int>>& history) override {
        if (!history.empty() && history.back().second == 1) {
            opponent_ever_defected = true;
        }
        return opponent_ever_defected ? 1 : 0;
    }

    std::string getName() const override { return "Frida"; }

    void reset() override {
        opponent_ever_defected = false;
    }
};

//������� ��� ��������������,����� ��������������� ������ ����������,����� �����
class George : public Strategy {
    int move_counter;
public:
    George() : move_counter(0) {}

    int play(const std::vector<std::pair<int, int>>& history) override {
        if (history.empty()) {
            move_counter = 1;
            return 0; 
        }

        move_counter++;

       
        if (move_counter % 4 == 1 || move_counter % 4 == 2) {// ��� ���� ������������
            
            return 0;
        }
        else {
            // ��������������� ����� ���������
            return 1 - history.back().second;
        }
    }

    std::string getName() const override { return "George"; }

    void reset() override {
        move_counter = 0;
    }
};

// ���� ����� ����� ������
struct GameResult {
    int scoreA;
    int scoreB;
    int maxDominantSeriesA;
    int maxDominantSeriesB;
};

// ���� ����� ����� �����������
GameResult playGame(Strategy* playerA, Strategy* playerB, int rounds = 200) {
    std::vector<std::pair<int, int>> history;//������� �����
    GameResult result = { 0, 0, 0, 0 };

    int currentSeriesA = 0;
    int currentSeriesB = 0;

    for (int i = 0; i < rounds; i++) {
        int moveA = playerA->play(history);
        int moveB = playerB->play(history);

        result.scoreA += PAYOFF[moveA][moveB];//���������� ����� �� �������
        result.scoreB += PAYOFF[moveB][moveA];

        //������������ �����
        if (moveA == 1 && moveB == 0) {
            currentSeriesA++;
            currentSeriesB = 0;
            result.maxDominantSeriesA = std::max(result.maxDominantSeriesA, currentSeriesA);
        }
        else if (moveA == 0 && moveB == 1) {
            currentSeriesB++;
            currentSeriesA = 0;
            result.maxDominantSeriesB = std::max(result.maxDominantSeriesB, currentSeriesB);
        }
        else {
            currentSeriesA = 0;
            currentSeriesB = 0;
        }

        history.push_back(std::make_pair(moveA, moveB));
    }

    return result;
}

int main() {
    setlocale(LC_ALL, "RU");
    
    // ������� ��� ���������
    std::vector<Strategy*> strategies = {
        new Alex(), new Bob(), new Clara(),
        new Denis(), new Emma(), new Frida(), new George()
    };

    std::vector<std::string> strategyNames;
    for (auto* strategy : strategies) {
        strategyNames.push_back(strategy->getName());
    }

    int n = strategies.size();
    std::vector<std::vector<int>> totalScores(n, std::vector<int>(n, 0));
    std::vector<std::vector<int>> dominantSeries(n, std::vector<int>(n, 0));

    std::cout << "���� �� 200�� ������\n\n";

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue;

            strategies[i]->reset();
            strategies[j]->reset();

            GameResult result = playGame(strategies[i], strategies[j]);

            totalScores[i][j] = result.scoreA;
            totalScores[j][i] = result.scoreB;

            dominantSeries[i][j] = result.maxDominantSeriesA;
            dominantSeries[j][i] = result.maxDominantSeriesB;
        }
    }

    
    std::cout << "\n������ ���� ���������:\n";
    std::cout << "=======================\n";

    // �� ����� �����
    std::cout << "\n����:\n";
    std::cout << "-----\n";
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {  // �������� � i+1 ����� �������� ��������
            std::cout << strategyNames[i] << " vs " << strategyNames[j]
                << ": " << totalScores[i][j] << " - " << totalScores[j][i] << " �����\n";
        }
    }

    // ����� �����
    std::cout << "\n������������ �����:\n";
    std::cout << "-------------------\n";
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {  // �������� � i+1 ����� �������� ��������
            std::cout << strategyNames[i] << " vs " << strategyNames[j]
                << ": ����� " << dominantSeries[i][j] << " - " << dominantSeries[j][i] << "\n";
        }
    }

    
    int bestTotalScore = 0;
    int worstTotalScore = 1000000;
    int bestMaxSeries = 0;
    int worstMaxSeries = 1000000;
    std::string bestScoreStrategy, worstScoreStrategy, bestSeriesStrategy, worstSeriesStrategy;

    for (int i = 0; i < n; i++) {
        int totalScore = 0;
        int maxSeries = 0;
        int gamesCount = 0;

        for (int j = 0; j < n; j++) {
            if (i != j) {
                totalScore += totalScores[i][j];
                maxSeries = std::max(maxSeries, dominantSeries[i][j]);
                gamesCount++;
            }
        }

        // ������/������ �� �����
        if (totalScore > bestTotalScore) {
            bestTotalScore = totalScore;
            bestScoreStrategy = strategyNames[i];
        }
        if (totalScore < worstTotalScore) {
            worstTotalScore = totalScore;
            worstScoreStrategy = strategyNames[i];
        }

        // ������/������ �� ������
        if (maxSeries > bestMaxSeries) {
            bestMaxSeries = maxSeries;
            bestSeriesStrategy = strategyNames[i];
        }
        if (maxSeries < worstMaxSeries) {
            worstMaxSeries = maxSeries;
            worstSeriesStrategy = strategyNames[i];
        }
    }

    
    std::cout << "\n����� ������ ����������:\n";
    std::cout << "==========================\n";
    std::cout << "������ �� �����: " << bestScoreStrategy << " (" << bestTotalScore << " �����)\n";
    std::cout << "������ �� �����: " << worstScoreStrategy << " (" << worstTotalScore << " �����)\n";
    std::cout << "������ �� ������: " << bestSeriesStrategy << " (�����: " << bestMaxSeries << ")\n";
    std::cout << "������ �� ������: " << worstSeriesStrategy << " (�����: " << worstMaxSeries << ")\n";

    
    
    std::cout << "\n����� ���������� GEORGE:\n";
    std::cout << "========================\n";

    int georgeIndex = -1;
    for (int i = 0; i < n; i++) {
        if (strategyNames[i] == "George") {
            georgeIndex = i;
            break;
        }
    }

    if (georgeIndex != -1) {
        int totalScore = 0;
        int maxSeries = 0;

        for (int j = 0; j < n; j++) {
            if (georgeIndex != j) {
                totalScore += totalScores[georgeIndex][j];
                maxSeries = std::max(maxSeries, dominantSeries[georgeIndex][j]);
            }
        }

        std::cout << "����� �����: " << totalScore << "\n";
        std::cout << "������������ ����� �������������: " << maxSeries << "\n";
    }

    for (auto* strategy : strategies) {
        delete strategy;
    }

    return 0;
}