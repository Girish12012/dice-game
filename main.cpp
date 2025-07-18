#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <map>
#include <numeric>
#include <random>

using namespace std;

const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string BOLD = "\033[1m";
const string UNDERLINE = "\033[4m";

const vector<string> DICE_ART = {
    "",
    "┌───────┐\n│       │\n│   ●   │\n│       │\n└───────┘",  // 1
    "┌───────┐\n│ ●     │\n│       │\n│     ● │\n└───────┘",  // 2
    "┌───────┐\n│ ●     │\n│   ●   │\n│     ● │\n└───────┘",  // 3
    "┌───────┐\n│ ●   ● │\n│       │\n│ ●   ● │\n└───────┘",  // 4
    "┌───────┐\n│ ●   ● │\n│   ●   │\n│ ●   ● │\n└───────┘",  // 5
    "┌───────┐\n│ ●   ● │\n│ ●   ● │\n│ ●   ● │\n└───────┘"   // 6
};

class Player {
private:
    string name;
    int score;
    vector<int> diceHistory;
    int wins;
    int bestRoll;
    double averageRoll;

public:
    Player(const string& playerName) : name(playerName), score(0), wins(0), bestRoll(0), averageRoll(0.0) {}

    string getName() const { return name; }
    int getScore() const { return score; }
    const vector<int>& getDiceHistory() const { return diceHistory; }
    int getWins() const { return wins; }
    int getBestRoll() const { return bestRoll; }
    double getAverageRoll() const { return averageRoll; }

    void addToScore(int points) {
        score += points;
        updateStats();
    }

    void addToHistory(int diceValue) {
        diceHistory.push_back(diceValue);
        updateStats();
    }

    void incrementWins() { wins++; }

    void reset() {
        score = 0;
        diceHistory.clear();
        bestRoll = 0;
        averageRoll = 0.0;
    }

private:
    void updateStats() {
        if (diceHistory.empty()) return;
        
        bestRoll = *max_element(diceHistory.begin(), diceHistory.end());
        averageRoll = accumulate(diceHistory.begin(), diceHistory.end(), 0.0) / diceHistory.size();
    }
};

class DiceGame {
private:
    vector<Player> players;
    int rounds;
    int currentRound;
    int diceSides;
    bool gameSaved;
    mt19937 rng;

    int rollDiceWithAnimation() {
        cout << "\nRolling dice";
        for (int i = 0; i < 3; ++i) {
            cout << ".";
            cout.flush();
            this_thread::sleep_for(chrono::milliseconds(300));
        }
        
        uniform_int_distribution<int> dist(1, diceSides);
        int result = dist(rng);
        
        cout << "\n";
        displayDiceArt(result);
        this_thread::sleep_for(chrono::milliseconds(800));
        return result;
    }

    void displayDiceArt(int value) {
        if (value >= 1 && value <= 6) {
            cout << DICE_ART[value] << "\n";
        } else {
            cout << "┌───────┐\n";
            cout << "│ " << setw(2) << value << "   │\n";
            cout << "└───────┘\n";
        }
    }

    void displayHeader(const string& title = "DICE GAME SIMULATOR") {
        system("cls||clear");
        cout << BOLD << CYAN << "============================================\n";
        cout << "          " << UNDERLINE << title << RESET << BOLD << CYAN << "          \n";
        cout << "============================================\n" << RESET;
        
        if (rounds > 0) {
            cout << BOLD << "Round: " << currentRound << "/" << rounds;
            cout << " | Dice: " << diceSides << "-sided";
            cout << " | Players: " << players.size() << "\n\n" << RESET;
        }
    }

    void displayScores(bool showDetails = false) {
        cout << BOLD << YELLOW << "\nCurrent Standings:\n" << RESET;
        cout << "┌───────────────┬────────┬────────┬────────┐\n";
        cout << "│ Player        │ Score  │ Wins   │ Best   │\n";
        cout << "├───────────────┼────────┼────────┼────────┤\n";
        
        for (const auto& player : players) {
            cout << "│ " << setw(13) << left << player.getName() << " │ "
                 << setw(6) << right << player.getScore() << " │ "
                 << setw(6) << right << player.getWins() << " │ "
                 << setw(6) << right << player.getBestRoll() << " │\n";
        }
        cout << "└───────────────┴────────┴────────┴────────┘\n";
        
        if (showDetails) {
            cout << "\n" << BOLD << "Roll History:\n" << RESET;
            for (const auto& player : players) {
                cout << player.getName() << ": ";
                for (int roll : player.getDiceHistory()) {
                    cout << roll << " ";
                }
                cout << "(Avg: " << fixed << setprecision(1) << player.getAverageRoll() << ")\n";
            }
        }
    }

    void displayRoundResults(const vector<pair<string, int>>& roundResults) {
        cout << BOLD << MAGENTA << "\nRound " << currentRound << " Results:\n" << RESET;
        cout << "┌───────────────┬───────────────┐\n";
        cout << "│ Player        │ Roll          │\n";
        cout << "├───────────────┼───────────────┤\n";
        
        for (const auto& result : roundResults) {
            cout << "│ " << setw(13) << left << result.first << " │ "
                 << setw(13) << left << result.second << " │\n";
        }
        cout << "└───────────────┴───────────────┘\n";
    }

    void announceRoundWinner(const string& winner, int points) {
        cout << BOLD << GREEN << "\n🎉 " << winner << " wins this round with " << points << "! 🎉\n" << RESET;
        
        auto it = find_if(players.begin(), players.end(),
            [&winner](const Player& p) { return p.getName() == winner; });
        if (it != players.end()) {
            it->incrementWins();
        }
        
        this_thread::sleep_for(chrono::seconds(2));
    }

    void saveGame() {
    ofstream outFile("dice_game_save.dat", ios::binary);
    if (outFile) {
        outFile.write(reinterpret_cast<const char*>(&rounds), sizeof(rounds));
        outFile.write(reinterpret_cast<const char*>(&currentRound), sizeof(currentRound));
        outFile.write(reinterpret_cast<const char*>(&diceSides), sizeof(diceSides));
        
        size_t numPlayers = players.size();
        outFile.write(reinterpret_cast<const char*>(&numPlayers), sizeof(numPlayers));
        
        for (const auto& player : players) {
            size_t nameLength = player.getName().size();
            outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
            outFile.write(player.getName().c_str(), nameLength);

            int score = player.getScore();
            int wins = player.getWins();
            outFile.write(reinterpret_cast<const char*>(&score), sizeof(score));
            outFile.write(reinterpret_cast<const char*>(&wins), sizeof(wins));
            
            const vector<int>& history = player.getDiceHistory();
            size_t historySize = history.size();
            outFile.write(reinterpret_cast<const char*>(&historySize), sizeof(historySize));
            for (int roll : history) {
                outFile.write(reinterpret_cast<const char*>(&roll), sizeof(roll));
            }
        }
        
        gameSaved = true;
        cout << BOLD << GREEN << "\nGame saved successfully!\n" << RESET;
    } else {
        cout << BOLD << RED << "\nError saving game!\n" << RESET;
    }
    this_thread::sleep_for(chrono::seconds(1));
}

bool loadGame() {
    ifstream inFile("dice_game_save.dat", ios::binary);
    if (inFile) {
        inFile.read(reinterpret_cast<char*>(&rounds), sizeof(rounds));
        inFile.read(reinterpret_cast<char*>(&currentRound), sizeof(currentRound));
        inFile.read(reinterpret_cast<char*>(&diceSides), sizeof(diceSides));
    
        size_t numPlayers;
        inFile.read(reinterpret_cast<char*>(&numPlayers), sizeof(numPlayers));
        players.clear();
        
        for (size_t i = 0; i < numPlayers; ++i) {
            size_t nameLength;
            inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
            string name(nameLength, '\0');
            inFile.read(&name[0], nameLength);
            
            Player player(name);
        
            int score, wins;
            inFile.read(reinterpret_cast<char*>(&score), sizeof(score));
            inFile.read(reinterpret_cast<char*>(&wins), sizeof(wins));
            
            size_t historySize;
            inFile.read(reinterpret_cast<char*>(&historySize), sizeof(historySize));
            for (size_t j = 0; j < historySize; ++j) {
                int roll;
                inFile.read(reinterpret_cast<char*>(&roll), sizeof(roll));
                player.addToHistory(roll);
                player.addToScore(roll);
            }
            
            for (int w = 0; w < wins; ++w) {
                player.incrementWins();
            }
            
            players.push_back(player);
        }
        
        gameSaved = false;
        cout << BOLD << GREEN << "\nGame loaded successfully!\n" << RESET;
        this_thread::sleep_for(chrono::seconds(1));
        return true;
    } else {
        cout << BOLD << RED << "\nNo saved game found!\n" << RESET;
        this_thread::sleep_for(chrono::seconds(1));
        return false;
    }
}
    void showGameRules() {
        displayHeader("GAME RULES");
        
        cout << BOLD << YELLOW << "🌟 Game Modes 🌟\n" << RESET;
        cout << "1. " << BOLD << "Classic Mode:" << RESET << " Highest roll each round wins\n";
        cout << "2. " << BOLD << "Target Mode:" << RESET << " First to reach target score wins\n";
        cout << "3. " << BOLD << "Elimination:" << RESET << " Players with lowest rolls are eliminated\n\n";
        
        cout << BOLD << YELLOW << "🎲 Dice Mechanics 🎲\n" << RESET;
        cout << "- Dice can have 4-12 sides\n";
        cout << "- Special dice rolls (doubles, triples) may trigger bonuses\n";
        cout << "- Consecutive wins grant multiplier bonuses\n\n";
        
        cout << BOLD << YELLOW << "🏆 Scoring System 🏆\n" << RESET;
        cout << "- Round winner gets points equal to their roll\n";
        cout << "- 2x points for back-to-back wins\n";
        cout << "- 5x bonus for rolling the maximum possible value\n\n";
        
        cout << "Press Enter to continue...";
        cin.ignore();
        cin.get();
    }

public:
    DiceGame() : rounds(0), currentRound(0), diceSides(6), gameSaved(false) {
        rng.seed(random_device()());
    }

    void setupGame() {
        displayHeader("GAME SETUP");
        
        cout << BOLD << "Select Game Mode:\n" << RESET;
        cout << "1. Classic Mode (Highest roll wins)\n";
        cout << "2. Target Score Mode\n";
        cout << "3. Elimination Mode\n";
        cout << "Enter choice: ";
        
        int mode;
        while (!(cin >> mode) || mode < 1 || mode > 3) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "Invalid choice! Enter 1-3: " << RESET;
        }
        
        int numPlayers;
        cout << "\n" << BOLD << "Number of Players (2-4): " << RESET;
        while (!(cin >> numPlayers) || numPlayers < 2 || numPlayers > 4) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "Invalid input! Enter 2-4: " << RESET;
        }
        
        cin.ignore();
        players.clear();
        for (int i = 0; i < numPlayers; ++i) {
            cout << "Player " << i+1 << " name: ";
            string name;
            getline(cin, name);
            if (name.empty()) name = "Player " + to_string(i+1);
            players.emplace_back(name);
        }
        
        cout << "\n" << BOLD << "Dice Sides (4-12): " << RESET;
        while (!(cin >> diceSides) || diceSides < 4 || diceSides > 12) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "Invalid input! Enter 4-12: " << RESET;
        }
        
        if (mode == 2) {  
            int target;
            cout << "\n" << BOLD << "Target Score (50-500): " << RESET;
            while (!(cin >> target) || target < 50 || target > 500) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << RED << "Invalid input! Enter 50-500: " << RESET;
            }
            rounds = target;
        } else {
            cout << "\n" << BOLD << "Number of Rounds (3-20): " << RESET;
            while (!(cin >> rounds) || rounds < 3 || rounds > 20) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << RED << "Invalid input! Enter 3-20: " << RESET;
            }
        }
        
        currentRound = 1;
    }

    void playGame() {
        while (currentRound <= rounds) {
            displayHeader("ROUND " + to_string(currentRound));
            displayScores();
            
            cout << BOLD << "\nRound Options:\n" << RESET;
            cout << "1. Play Round\n";
            cout << "2. Save Game\n";
            cout << "3. Show Statistics\n";
            cout << "4. Main Menu\n";
            cout << "Enter choice: ";
            
            int choice;
            cin >> choice;
            
            switch (choice) {
                case 1: {
                    vector<pair<string, int>> roundResults;
                    
                    for (auto& player : players) {
                        displayHeader(player.getName() + "'s Turn");
                        displayScores();
                        
                        cout << BOLD << player.getName() << ", ready to roll? (Press Enter)" << RESET;
                        cin.ignore();
                        cin.get();
                        
                        int roll = rollDiceWithAnimation();
                        player.addToScore(roll);
                        player.addToHistory(roll);
                        roundResults.emplace_back(player.getName(), roll);
                    }
                    
                    auto winner = max_element(roundResults.begin(), roundResults.end(),
                        [](const pair<string, int>& a, const pair<string, int>& b) {
                            return a.second < b.second;
                        });
                
                    int tieCount = count_if(roundResults.begin(), roundResults.end(),
                        [winner](const pair<string, int>& p) {
                            return p.second == winner->second;
                        });
                    
                    if (tieCount > 1) {
                        cout << BOLD << YELLOW << "\nThis round is a tie!\n" << RESET;
                    } else {
                        announceRoundWinner(winner->first, winner->second);
                    }
                    
                    currentRound++;
                    break;
                }
                case 2:
                    saveGame();
                    break;
                case 3:
                    showStatistics();
                    break;
                case 4:
                    return;
                default:
                    cout << RED << "Invalid choice!\n" << RESET;
                    this_thread::sleep_for(chrono::seconds(1));
            }
        }
        
        showFinalResults();
    }

    void showStatistics() {
        displayHeader("GAME STATISTICS");
        
        if (players.empty()) {
            cout << RED << "No game data available.\n" << RESET;
        } else {
            cout << BOLD << YELLOW << "Player Stats:\n" << RESET;
            displayScores(true);
        
            cout << BOLD << YELLOW << "\nGame Analysis:\n" << RESET;
            
            map<int, int> rollFrequency;
            for (const auto& player : players) {
                for (int roll : player.getDiceHistory()) {
                    rollFrequency[roll]++;
                }
            }
            
            if (!rollFrequency.empty()) {
                auto mostFrequent = max_element(rollFrequency.begin(), rollFrequency.end(),
                    [](const pair<int, int>& a, const pair<int, int>& b) {
                        return a.second < b.second;
                    });
                
                cout << "Most frequent roll: " << mostFrequent->first 
                     << " (rolled " << mostFrequent->second << " times)\n";
            }
            
            auto luckiest = max_element(players.begin(), players.end(),
                [](const Player& a, const Player& b) {
                    return a.getAverageRoll() < b.getAverageRoll();
                });
            
            cout << "Luckiest player: " << luckiest->getName() 
                 << " (avg " << fixed << setprecision(1) << luckiest->getAverageRoll() << ")\n";
        }
        
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }

    void showFinalResults() {
        displayHeader("FINAL RESULTS");
        
        sort(players.begin(), players.end(),
            [](const Player& a, const Player& b) {
                return a.getScore() > b.getScore();
            });
        
        cout << BOLD << YELLOW << "\n🏆 FINAL STANDINGS 🏆\n" << RESET;
        cout << "┌──────┬───────────────┬────────┬────────┐\n";
        cout << "│ Rank │ Player        │ Score  │ Wins   │\n";
        cout << "├──────┼───────────────┼────────┼────────┤\n";
        
        for (size_t i = 0; i < players.size(); ++i) {
            string medal;
            if (i == 0) medal = "🥇";
            else if (i == 1) medal = "🥈";
            else if (i == 2) medal = "🥉";
            
            cout << "│ " << setw(4) << left << to_string(i+1) + medal << " │ "
                 << setw(13) << left << players[i].getName() << " │ "
                 << setw(6) << right << players[i].getScore() << " │ "
                 << setw(6) << right << players[i].getWins() << " │\n";
        }
        cout << "└──────┴───────────────┴────────┴────────┘\n";
        
        cout << BOLD << YELLOW << "\n🌟 SPECIAL ACHIEVEMENTS 🌟\n" << RESET;
        
        auto bestRoller = max_element(players.begin(), players.end(),
            [](const Player& a, const Player& b) {
                return a.getBestRoll() < b.getBestRoll();
            });
        
        cout << "Highest Roll: " << bestRoller->getName() << " with " 
             << bestRoller->getBestRoll() << "\n";
        
        auto mostConsistent = min_element(players.begin(), players.end(),
            [](const Player& a, const Player& b) {
                const auto& histA = a.getDiceHistory();
                const auto& histB = b.getDiceHistory();
                
                if (histA.empty() || histB.empty()) return false;
                
                double meanA = accumulate(histA.begin(), histA.end(), 0.0) / histA.size();
                double varianceA = 0.0;
                for (int x : histA) varianceA += pow(x - meanA, 2);
                
                double meanB = accumulate(histB.begin(), histB.end(), 0.0) / histB.size();
                double varianceB = 0.0;
                for (int x : histB) varianceB += pow(x - meanB, 2);
                
                return varianceA < varianceB;
            });
        
        cout << "Most Consistent: " << mostConsistent->getName() 
             << " (avg " << fixed << setprecision(1) << mostConsistent->getAverageRoll() << ")\n";
        
        cout << "\nPress Enter to return to main menu...";
        cin.ignore();
        cin.get();
    }

    void showMainMenu() {
        while (true) {
            displayHeader("MAIN MENU");
            
            cout << BOLD << "1. New Game\n";
            cout << "2. Load Game\n";
            cout << "3. Game Rules\n";
            cout << "4. Exit\n" << RESET;
            cout << "Enter choice: ";
            
            int choice;
            cin >> choice;
            
            switch (choice) {
                case 1:
                    setupGame();
                    playGame();
                    break;
                case 2:
                    if (loadGame()) {
                        playGame();
                    }
                    break;
                case 3:
                    showGameRules();
                    break;
                case 4:
                    if (!gameSaved && !players.empty()) {
                        cout << "Save before exiting? (y/n): ";
                        char saveChoice;
                        cin >> saveChoice;
                        if (tolower(saveChoice) == 'y') {
                            saveGame();
                        }
                    }
                    return;
                default:
                    cout << RED << "Invalid choice!\n" << RESET;
                    this_thread::sleep_for(chrono::seconds(1));
            }
        }
    }
};

int main() {
    DiceGame game;
    game.showMainMenu();
    return 0;
}
