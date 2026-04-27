#pragma once

#include <algorithm>
#include <iostream>
#include <string_view>
#include <vector>

#include "utils.hpp"

namespace cppMan {

namespace Settings {
constexpr int wrong_guesses_allowed {6};
}

namespace WordList {
using svVec = std::vector<std::string_view>;
inline const svVec words {
    "mystery",
    "broccoli",
    "account",
    "almost",
    "spaghetti",
    "opinion",
    "beautiful",
    "distance",
    "luggage"
};
std::string_view getRandomWord() { return words[Random::randn<size_t>(0, words.size() - 1)]; }
}  // namespace WordList

class Session {
   public:
    [[nodiscard]] std::string_view getWord() const { return m_word; }
    [[nodiscard]] int              wrongGuessesLeft() const { return m_wrong_guesses_left; }
    void                           removeGuess() { --m_wrong_guesses_left; }
    [[nodiscard]] bool             isLetterGuessed(char letter) const {
        return m_letter_guessed[toIndex(letter)];
    }
    void setLetterGuessed(char letter) { m_letter_guessed[toIndex(letter)] = true; }

    [[nodiscard]] bool isLetterInWord(char letter) const {
        return std::ranges::any_of(m_word, [letter](char chr) { return chr == letter; });
    }
    bool won() {
        return std::ranges::all_of(m_word, [this](char letter) { return isLetterGuessed(letter); });
    }

   private:
    std::string_view  m_word {WordList::getRandomWord()};
    int               m_wrong_guesses_left {Settings::wrong_guesses_allowed};
    std::vector<bool> m_letter_guessed {std::vector<bool>(26)};

    // convert a letter into an array index via (letter % ascii_modulo)-1.
    // This works with both lower case and upper case letters.
    static size_t toIndex(char letter) {
        constexpr int ascii_modulo = 32;
        return static_cast<size_t>((letter % ascii_modulo) - 1);
    }
};
void draw(const Session& session) {
    std::cout << '\n';

    std::cout << "The word: ";
    for (auto letter : session.getWord()) {
        if (session.isLetterGuessed(letter)) {
            std::cout << letter;
        } else {
            std::cout << '_';
        }
    }
    std::cout << "  Wrong guesses: ";
    for (int i = 0; i < session.wrongGuessesLeft(); ++i) {
        std::cout << '+';
    }

    for (char letter = 'a'; letter <= 'z'; ++letter) {
        if (session.isLetterGuessed(letter) && !session.isLetterInWord(letter)) {
            std::cout << letter;
        }
    }

    std::cout << '\n';
}

char getGuess(const Session& session) {
    while (true) {
        const char* err_msg = "That wasn't a valid input. Try again.\n";

        std::cout << "Enter your next letter: ";
        char letter {};
        std::cin >> letter;
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
        if (letter < 'a' || letter > 'z') {
            std::cout << err_msg;
            continue;
        }
        // If the letter was already guessed, try again
        if (session.isLetterGuessed(letter)) {
            std::cout << "You already guessed that. Try again.\n";
            continue;
        }
        return letter;
    }
}

void handleGuesses(Session& session, char letter) {
    session.setLetterGuessed(letter);
    if (session.isLetterInWord(letter)) {
        std::cout << "Yes, '" << letter << "' is in the word!\n";
        return;
    }
    std::cout << "No, '" << letter << "' is not in the word!\n";
    session.removeGuess();
}

void playGame() {
    std::cout << "Welcome to C++man (a variant of Hangman)\n";
    std::cout << "To win: guess the word.  To lose: run out of pluses.\n";

    Session session {};

    while ((session.wrongGuessesLeft() != 0) && !session.won()) {
        draw(session);
        char letter {getGuess(session)};
        handleGuesses(session, letter);
    }
    draw(session);

    if (session.wrongGuessesLeft() == 0) {
        {
            std::cout << "You lost! The word was: " << session.getWord() << '\n';
        }
    } else {
        std::cout << "You won!\n";
    }
}
}  // namespace cppMan
