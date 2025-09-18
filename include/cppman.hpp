#pragma once

#include <iostream>
#include <string_view>
#include <vector>

#include "utils.hpp"

namespace cppMan {

namespace Settings {
constexpr int wrong_guesses_allowed{6};
}

namespace WordList {
using svVec = std::vector<std::string_view>;
svVec words{"mystery", "broccoli",  "account",  "almost", "spaghetti",
            "opinion", "beautiful", "distance", "luggage"};
std::string_view getRandomWord() {
    return words[Random::randn<size_t>(0, words.size() - 1)];
}
}  // namespace WordList

class Session {
   public:
    std::string_view getWord() const { return m_word; }
    int wrongGuessesLeft() const { return m_wrong_guesses_left; }
    void removeGuess() { --m_wrong_guesses_left; }
    bool isLetterGuessed(char c) const { return m_letter_guessed[toIndex(c)]; }
    void setLetterGuessed(char c) { m_letter_guessed[toIndex(c)] = true; }

    bool isLetterInWord(char c) const {
        for (auto ch : m_word) {
            if (ch == c) return true;
        }
        return false;
    }
    bool won() {
        for (auto c : m_word) {
            if (!isLetterGuessed(c)) return false;
        }
        return true;
    }

   private:
    std::string_view m_word{WordList::getRandomWord()};
    int m_wrong_guesses_left{Settings::wrong_guesses_allowed};
    std::vector<bool> m_letter_guessed{std::vector<bool>(26)};

    // convert a letter into an array index via (letter % 32)-1.
    // This works with both lower case and upper case letters.
    size_t toIndex(char c) const { return static_cast<size_t>((c % 32) - 1); }
};
void draw(const Session &s) {
    std::cout << '\n';

    std::cout << "The word: ";
    for (auto c : s.getWord()) {
        if (s.isLetterGuessed(c))
            std::cout << c;
        else
            std::cout << '_';
    }
    std::cout << "  Wrong guesses: ";
    for (int i = 0; i < s.wrongGuessesLeft(); ++i) std::cout << '+';

    for (char c = 'a'; c <= 'z'; ++c) {
        if (s.isLetterGuessed(c) && !s.isLetterInWord(c)) std::cout << c;
    }

    std::cout << '\n';
}

char getGuess(const Session &s) {
    while (true) {
        const char *err_msg = "That wasn't a vilid input. Try again.\n";

        std::cout << "Enter your next letter: ";
        char c{};
        std::cin >> c;
        // If user did something bad, try again
        if (!std::cin) {
            // Fix it
            Utils::handleBadInputs();
            std::cout << err_msg;
            continue;
        }
        // Clear out any extraneous input
        Utils::handleBadInputs();

        // If the user entered an invalid char, try again
        if (c < 'a' || c > 'z') {
            std::cout << err_msg;
            continue;
        }
        // If the letter was already guessed, try again
        if (s.isLetterGuessed(c)) {
            std::cout << "You already guesses that. Try again.\n";
            continue;
        }
        return c;
    }
}

void handleGuesses(Session &s, char c) {
    s.setLetterGuessed(c);
    if (s.isLetterInWord(c)) {
        std::cout << "Yes, '" << c << "' is in the word!\n";
        return;
    }
    std::cout << "No, '" << c << "' is not in the word!\n";
    s.removeGuess();
}

void playGame() {
    std::cout << "Welcome to C++man (a variant of Hangman)\n";
    std::cout << "To win: guess the word.  To lose: run out of pluses.\n";

    Session s{};

    while (s.wrongGuessesLeft() && !s.won()) {
        draw(s);
        char c{getGuess(s)};
        handleGuesses(s, c);
    }
    draw(s);

    if (!s.wrongGuessesLeft())
        std::cout << "You lost! The word was: " << s.getWord() << '\n';
    else
        std::cout << "You won!\n";
}
}  // namespace cppMan
