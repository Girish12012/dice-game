#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <ctime>
#include <cstdlib>
#include <iomanip>

using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

struct MoodEntry {
    std::string date;
    std::string mood;
    std::string activity;
};
vector<string> quotes = {
    "Believe you can and you're halfway there.",
    "Every day is a second chance.",
    "You are stronger than you think.",
    "Stay positive, work hard, make it happen.",
    "Difficult roads often lead to beautiful destinations."
};
map<string, vector<string>> moodActivities = {
    {"happy", {"Share your happiness with a friend!", "Go for a walk and enjoy nature.", "Listen to your favorite music."}},
    {"sad", {"Write down your feelings.", "Watch a comforting movie.", "Talk to someone you trust."}},
    {"angry", {"Try deep breathing exercises.", "Go for a run or do some exercise.", "Listen to calming music."}},
    {"anxious", {"Practice meditation for 5 minutes.", "Make a to-do list to organize your thoughts.", "Read a book you enjoy."}},
    {"tired", {"Take a short nap.", "Drink a glass of water.", "Stretch your body."}},
    {"excited", {"Start a new project!", "Share your excitement with someone.", "Plan something fun for the weekend."}}
};

void printLogo() {
    cout << CYAN << BOLD;
    cout << MAGENTA << BOLD << string(56, '=') << RESET << endl;
    cout << YELLOW << BOLD << "        Welcome to the Mood Tracker!" << RESET << endl;
    cout << MAGENTA << BOLD << string(56, '=') << RESET << endl;
}

string getCurrentDate() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", ltm);
    return string(buf);
}

void saveMoodEntry(const MoodEntry& entry, const string& filename) {
    ofstream file(filename, ios::app);
    if (file.is_open()) {
        file << entry.date << "," << entry.mood << "," << entry.activity << "\n";
        file.close();
    }
}

vector<MoodEntry> loadMoodHistory(const string& filename) {
    vector<MoodEntry> history;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        size_t pos1 = line.find(",");
        size_t pos2 = line.rfind(",");
        if (pos1 != string::npos && pos2 != string::npos && pos1 != pos2) {
            MoodEntry entry;
            entry.date = line.substr(0, pos1);
            entry.mood = line.substr(pos1 + 1, pos2 - pos1 - 1);
            entry.activity = line.substr(pos2 + 1);
            history.push_back(entry);
        }
    }
    return history;
}

void showMoodHistory(const string& filename) {
    vector<MoodEntry> history = loadMoodHistory(filename);
    cout << BLUE << BOLD << "\n--- Mood History ---" << RESET << endl;
    if (history.empty()) {
        cout << YELLOW << "No mood history found.\n" << RESET;
        return;
    }
    cout << left << setw(12) << "Date" << setw(12) << "Mood" << "Activity" << endl;
    cout << string(50, '-') << endl;
    for (const auto& entry : history) {
        cout << left << setw(12) << entry.date << setw(12) << entry.mood << entry.activity << endl;
    }
    cout << MAGENTA << string(50, '-') << RESET << endl;
}

void detectMood() {
    cout << GREEN << BOLD << "\nHow are you feeling today?" << RESET << endl;
    cout << CYAN << "Choose from the following or type your own mood:" << RESET << endl;
    int idx = 1;
    vector<string> moods;
    for (const auto& pair : moodActivities) {
        cout << YELLOW << idx << ". " << pair.first << RESET << endl;
        moods.push_back(pair.first);
        idx++;
    }
    cout << YELLOW << idx << ". Other (type your own)" << RESET << endl;
    cout << BOLD << "Enter your choice (number or mood): " << RESET;
    string input;
    getline(cin, input);
    string mood;
    if (!input.empty() && isdigit(input[0])) {
        int choice = stoi(input);
        if (choice >= 1 && choice <= (int)moods.size()) {
            mood = moods[choice - 1];
        } else {
            cout << BOLD << "Enter your mood: " << RESET;
            getline(cin, mood);
        }
    } else {
        mood = input;
    }
    string activity;
    if (moodActivities.count(mood)) {
        const auto& acts = moodActivities[mood];
        activity = acts[rand() % acts.size()];
    } else {
        activity = "Do something you enjoy or talk to a friend.";
    }
    string quote = quotes[rand() % quotes.size()];
    cout << GREEN << BOLD << "\nSuggested activity: " << RESET << activity << endl;
    cout << BLUE << BOLD << "Motivational quote: " << RESET << '"' << quote << '"' << endl;
    MoodEntry entry = {getCurrentDate(), mood, activity};
    saveMoodEntry(entry, "mood_history.txt");
    cout << MAGENTA << "Your mood and activity have been saved." << RESET << endl;
}

void printMenu() {
    cout << endl;
    cout << BOLD << CYAN << "==== Mood Tracker Menu ====" << RESET << endl;
    cout << GREEN << "1. Detect mood and get suggestions" << RESET << endl;
    cout << YELLOW << "2. View mood history" << RESET << endl;
    cout << RED << "3. Exit" << RESET << endl;
    cout << BOLD << "Choose an option: " << RESET;
}

int main() {
    srand((unsigned)time(0));
    printLogo();
    while (true) {
        printMenu();
        string choice;
        getline(cin, choice);
        if (choice == "1") {
            detectMood();
        } else if (choice == "2") {
            showMoodHistory("mood_history.txt");
        } else if (choice == "3") {
            cout << BOLD << GREEN << "Goodbye! Stay positive!" << RESET << endl;
            break;
        } else {
            cout << RED << "Invalid option. Please try again." << RESET << endl;
        }
    }
    return 0;
} 