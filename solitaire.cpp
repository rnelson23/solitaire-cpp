#include <iostream>
#include <algorithm>
#include <random>
#include <array>
#include <regex>

enum {STOCK, WASTE, COLUMN, FOUNDATION};
enum {ACE = 0, TEN = 9, KING = 12};
enum {RED, BLACK};

class Card {
public:
    int rank = 0;
    int suit = 0;
    int color = 0;

    bool flip = false;

    std::string face;
    std::string back =
            "┌───────────┐\x1b[1B\x1b[13D"
            "│ ░░░░░░░░░ │\x1b[1B\x1b[13D"
            "│ ░░░░░░░░░ │\x1b[1B\x1b[13D"
            "│ ░░░░░░░░░ │\x1b[1B\x1b[13D"
            "│ ░░░░░░░░░ │\x1b[1B\x1b[13D"
            "│ ░░░░░░░░░ │\x1b[1B\x1b[13D"
            "│ ░░░░░░░░░ │\x1b[1B\x1b[13D"
            "│ ░░░░░░░░░ │\x1b[1B\x1b[13D"
            "└───────────┘";
};

class Pile {
public:
    int type = 0;

    std::vector<Card> cards;
    std::string empty =
            "┌─         ─┐\x1b[1B\x1b[13D"
            "             \x1b[1B\x1b[13D"
            "             \x1b[1B\x1b[13D"
            "             \x1b[1B\x1b[13D"
            "             \x1b[1B\x1b[13D"
            "             \x1b[1B\x1b[13D"
            "             \x1b[1B\x1b[13D"
            "             \x1b[1B\x1b[13D"
            "└─         ─┘";

    void move(Pile &pile, int numCards = 1) {
        pile.cards.insert(pile.cards.end(), cards.end() - numCards, cards.end());
        cards.erase(cards.end() - numCards, cards.end());
    }
};

class State {
public:
    int score = 0;
    int draw = 1;

    bool moved = false;

    Pile stock;
    Pile waste;

    std::vector<Card> wasteCards;

    std::array<Pile, 7> tableau{};
    std::array<Pile, 4> foundations{};
};

void initializeTable();
void finishTable();
void resetTable();

void move(const std::string &sourceStr, const std::string &destinationStr, int numCards = 1);
Pile &getSource(State &newState, const std::string &sourceStr);

bool canMove(const Card &card, const Pile &pile);
bool canMove(State &state);

bool tableSolved();
bool tableSolvable();

int maxTableauLines();

void printTable(const std::string &prompt);
std::string genPrompt(const std::string &prompt);

std::string genStock();
std::string genWaste();
std::string genFoundations();
std::string genTableau();

std::string genSpaces(int num);
std::string genDashes(int num);

std::vector<State> states;

int main() {
    bool autocomplete = true;
    initializeTable();

    std::cout << "\n"
                 "                  ██████╗ █████╗ ██╗     ██╗████████╗ █████╗ ██╗██████╗ ███████╗\n"
                 "                 ██╔════╝██╔══██╗██║     ██║╚══██╔══╝██╔══██╗██║██╔══██╗██╔════╝\n"
                 "                 ╚█████╗ ██║  ██║██║     ██║   ██║   ███████║██║██████╔╝█████╗\n"
                 "                  ╚═══██╗██║  ██║██║     ██║   ██║   ██╔══██║██║██╔══██╗██╔══╝\n"
                 "                 ██████╔╝╚█████╔╝███████╗██║   ██║   ██║  ██║██║██║  ██║███████╗\n"
                 "                 ╚═════╝  ╚════╝ ╚══════╝╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝╚═╝  ╚═╝╚══════╝\n\n"

                 "    Welcome to solitaire! The game consists of a stock pile, a talon (or waste) pile, four\n"
                 "foundation piles, and seven tableau piles. The goal of the game is to build up the foundation\n"
                 "piles from ace to king. The tableau contains seven piles of increasing number of cards, i.e.\n"
                 "column one has 1 card, columns two has 2 cards, etc. Cards can only move onto tableau cards of\n"
                 "the opposite color and one rank higher. Cards can only move onto foundation cards of the same\n"
                 "suit and one rank lower. The commands are listed below:\n\n"

                 "  [W]      [C|F]<num> -> Move a card from the [W]aste pile to a [C]olumn or [F]oundation pile\n"
                 "  [C]<num> [C|F]<num> -> Move a card from a [C]olumn to another [C]olumn or [F]oundation pile\n"
                 "  [F]<num>   [C]<num> -> Move a card from a [F]oundation pile to a [C]olumn\n\n"

                 "  [S] -> Move card(s) from the [S]tock pile to the waste pile\n"
                 "  [U] -> [U]ndoes a move\n\n"

                 "  [O] -> Sets the game to [O]ne card draw\n"
                 "  [T] -> Sets the game to [T]hree card draw\n\n"

                 "  [R] -> [R]esets and reshuffles the game\n"
                 "  [Q] -> [Q]uit the game\n\n\n\n";

    while (true) {
        int numCards = 1;

        std::string input;
        std::string sourceStr;
        std::string destinationStr;

        if (tableSolved()) {
            printTable("You win! Play again (y/n)? ");

            std::string res;
            std::getline(std::cin, res);

            for (char &c : res) c = (char)std::tolower(c);
            if (!res.starts_with('y')) {
                std::cout << "\x1b[" << maxTableauLines() + 14 << "E\n";
                return 0;
            }

            resetTable();

            continue;
        }

        if (tableSolvable() && autocomplete) {
            printTable("All cards revealed! Auto Complete (y/n)? ");

            std::string res;
            std::getline(std::cin, res);

            for (char &c : res) c = (char)std::tolower(c);
            if (res.starts_with('y')) {
                finishTable();

            } else {
                autocomplete = false;
            }

            continue;
        }

        if (states.back().stock.cards.empty() && !canMove(states.back())) {
            printTable("No more moves! Undo last move (y/n)? ");

            std::string res;
            std::getline(std::cin, res);

            for (char &c : res) c = (char)std::tolower(c);
            if (res.starts_with('y')) {
                states.pop_back();

            } else {
                printTable("You lost! Play again (y/n)? ");

                res.clear();
                std::getline(std::cin, res);

                for (char &c : res) c = (char)std::tolower(c);
                if (!res.starts_with('y')) {
                    std::cout << "\x1b[" << maxTableauLines() + 14 << "E\n";
                    return 0;
                }

                resetTable();
            }

            continue;
        }

        printTable("> ");
        std::getline(std::cin, input);

        for (char &c : input) c = (char)std::tolower(c);

        std::regex e(R"(\b(\d*)([swcfuotrq]?\d*))");
        std::smatch m;

        while (std::regex_search(input, m, e)) {
            if (sourceStr.empty()) {
                sourceStr = m[2].str();

                if (!m[1].str().empty()) {
                    numCards = std::stoi(m[1].str());
                }

            } else destinationStr = m[2].str();

            input = m.suffix().str();
        }

        if (destinationStr.empty()) destinationStr = "w";
        if (sourceStr.empty()) continue;

        switch (sourceStr.front()) {
            case 'w':
            case 'c':
            case 'f':
            case 's':
                move(sourceStr, destinationStr, numCards);
                break;

            case 'u':
                states.pop_back();
                break;

            case 'o':
                states.back().draw = 1;
                resetTable();
                break;

            case 't':
                states.back().draw = 3;
                resetTable();
                break;

            case 'r':
                resetTable();
                break;

            case 'q':
                std::cout << "\x1b[" << maxTableauLines() + 14 << "E\n";
                return 0;
        }
    }
}

void initializeTable() {
    std::array<std::string, 13> ranks = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    std::array<std::string, 4> suits = {"♥", "♦", "♠", "♣"};

    std::random_device rdev{};
    std::default_random_engine e{rdev()};

    State initialState;

    initialState.stock.type = STOCK;
    initialState.waste.type = WASTE;

    for (Pile &foundation : initialState.foundations) {
        foundation.type = FOUNDATION;
    }

    Pile &stock = initialState.stock;

    for (int suit = 0; suit < 4; suit++) {
        for (int rank = 0; rank < 13; rank++) {
            Card card;

            if (suit < 2) card.color = RED;
            else card.color = BLACK;

            std::stringstream face;

            std::string rankStr = ranks[rank];
            std::string suitStr = suits[suit];

            std::string topRank = rank == TEN ? rankStr : rankStr + " ";
            std::string bottomRank = rank == TEN ? rankStr : " " + rankStr;

            std::string color = card.color == RED ? "\x1b[1;31m" : "\x1b[1;90m";
            std::string reset = "\x1b[0m";

            std::string newLine = "\x1b[1B\x1b[13D";

            face << "┌───────────┐" << newLine
                 << "│ " << color << topRank << "      " << suitStr << reset << " │" << newLine
                 << "│ " << color << "         " << reset << " │" << newLine
                 << "│ " << color << "         " << reset << " │" << newLine
                 << "│ " << color << "    " << suitStr << "    " << reset << " │" << newLine
                 << "│ " << color << "         " << reset << " │" << newLine
                 << "│ " << color << "         " << reset << " │" << newLine
                 << "│ " << color << suitStr << "      " << bottomRank << reset << " │" << newLine
                 << "└───────────┘";

            card.rank = rank;
            card.suit = suit;
            card.face = face.str();

            stock.cards.push_back(card);
        }
    }

    std::shuffle(stock.cards.begin(), stock.cards.end(), e);

    for (int i = 0; i < 7; i++) {
        Pile &column = initialState.tableau[i];
        column.type = COLUMN;

        stock.move(column, i + 1);
        column.cards.back().flip = true;
    }

    states.push_back(initialState);
}

void finishTable() {
    Pile deck;

    for (Pile &column : states.back().tableau) {
        for (Card &card : column.cards) {
            column.move(deck);
            card.flip = true;
        }
    }

    for (Card &card : states.back().waste.cards) {
        states.back().waste.move(deck);
        card.flip = true;
    }

    for (Card &card : states.back().stock.cards) {
        states.back().stock.move(deck);
        card.flip = true;
    }

    for (int i = 0; i < 4; i++) {
        Pile &foundation = states.back().foundations[i];

        int numFoundationCards = (int)foundation.cards.size() - 1;
        int nextCardSuit;

        for (int j = numFoundationCards; j < 13; j++) {
            if (j > -1) nextCardSuit = foundation.cards.back().suit;
            int nextCardRank = j + 1;

            int numDeckCards = (int)deck.cards.size();
            Pile temp;

            for (int k = 0; k < numDeckCards; k++) {
                deck.move(temp);
                Card &card = temp.cards.back();

                if (card.rank != nextCardRank) continue;
                if (j > -1 && card.suit != nextCardSuit) continue;

                temp.move(foundation);
                break;
            }

            if (!temp.cards.empty()) temp.move(deck, (int)temp.cards.size());
        }
    }
}

void resetTable() {
    int draw = states.back().draw;

    states.clear();
    initializeTable();

    states.back().draw = draw;
}

void move(const std::string &sourceStr, const std::string &destinationStr, int numCards) {
    State newState = states.back();
    Pile &source = getSource(newState, sourceStr);

    switch (destinationStr.front()) {
        case 'w': {
            Pile &stock = source;
            Pile &waste = newState.waste;

            if (stock.cards.empty()) {
                int numWasteCards = (int)waste.cards.size();

                for (int i = 0; i < numWasteCards; i++) {
                    waste.move(stock);
                    stock.cards.back().flip = false;
                }

                newState.moved = false;

            } else {
                int numStockCards = 1;
                if (newState.draw == 3) numStockCards = stock.cards.size() < 3 ? (int)stock.cards.size() : 3;

                for (int i = 0; i < numStockCards; i++) {
                    stock.cards.back().flip = true;
                    stock.move(waste);
                }
            }

            break;
        }

        case 'c': {
            int index = std::stoi(destinationStr.substr(1, 1)) - 1;

            Pile &column = newState.tableau[index];
            Card &card = source.cards[source.cards.size() - numCards];

            if (canMove(card, column)) {
                source.move(column, numCards);
            }

            break;
        }

        case 'f': {
            int index = std::stoi(destinationStr.substr(1, 1)) - 1;

            Pile &foundation = newState.foundations[index];
            Card &card = source.cards.back();

            if (canMove(card, foundation)) {
                source.move(foundation);
                newState.score += 10;
            }

            break;
        }
    }

    if (sourceStr.starts_with('c') && !source.cards.empty() && !source.cards.back().flip) {
        source.cards.back().flip = true;
        newState.score += 5;
    }

    if (sourceStr.starts_with('s')) {
        if (newState.waste.cards.empty()) newState.wasteCards.clear();
        else newState.wasteCards.push_back(newState.waste.cards.back());
    }

    if (sourceStr.starts_with('w')) {
        if (newState.waste.cards.empty()) newState.wasteCards.clear();
        else {
            newState.wasteCards.pop_back();
            newState.wasteCards.push_back(newState.waste.cards.back());
            newState.moved = true;
        }
    }

    states.push_back(newState);
}

Pile &getSource(State &newState, const std::string &sourceStr) {
    Pile *sourcePtr;

    switch (sourceStr.front()) {
        case 's':
            sourcePtr = &newState.stock;
            break;

        case 'w':
            sourcePtr = &newState.waste;
            break;

        case 'c':
            sourcePtr = &newState.tableau[std::stoi(sourceStr.substr(1, 1)) - 1];
            break;

        case 'f':
            sourcePtr = &newState.foundations[std::stoi(sourceStr.substr(1, 1)) - 1];
            break;
    }

    return *sourcePtr;
}

bool canMove(const Card &card, const Pile &pile) {
    if (pile.cards.empty()) {
        if (pile.type == COLUMN && card.rank == KING) return true;
        if (pile.type == FOUNDATION && card.rank == ACE) return true;

        return false;
    }

    Card otherCard = pile.cards.back();

    if (pile.type == COLUMN) {
        if (card.rank == otherCard.rank && card.suit == otherCard.suit) return false;
        return card.color != otherCard.color && card.rank + 1 == otherCard.rank;

    } else {
        return card.suit == otherCard.suit && card.rank == otherCard.rank + 1;
    }
}

bool canMove(State &state) {
    if (state.draw == 3 && state.moved) return true;

    for (Pile &column : state.tableau) {
        if (column.cards.empty()) continue;
        int numFaceDown = 0;

        for (int i = 0; i < column.cards.size(); i++) {
            Card &card = column.cards[i];

            if (!card.flip) {
                numFaceDown++;
                continue;
            }

            if (i == 0) {
                if (card.rank == KING) continue;

                int numKingsToMove = 4;

                for (Pile &newColumn : state.tableau) {
                    if (newColumn.cards.front().rank == KING) numKingsToMove--;
                }

                if (numKingsToMove == 0) continue;
            }

            for (Pile &newColumn : state.tableau) {
                if (!canMove(card, newColumn)) continue;
                if (i == numFaceDown) return true;

                Card &newCard = column.cards[i - 1];

                for (Pile &foundation : state.foundations) {
                    if (canMove(newCard, foundation)) return true;
                }
            }
        }

        Card &card = column.cards.back();

        for (Pile &foundation : state.foundations) {
            if (canMove(card, foundation)) return true;
        }
    }

    std::vector<Card> *wasteCardsPtr;

    if (state.draw == 3) wasteCardsPtr = &state.wasteCards;
    else wasteCardsPtr = &state.waste.cards;

    std::vector<Card> &wasteCards = *wasteCardsPtr;

    for (Card &card : wasteCards) {
        for (Pile &column : state.tableau) {
            if (canMove(card, column)) return true;
        }

        for (Pile &foundation : state.foundations) {
            if (canMove(card, foundation)) return true;
        }
    }

    return false;
}

bool tableSolved() {
    bool allFoundationsFinished = true;

    for (Pile &foundation : states.back().foundations) {
        if (foundation.cards.size() < 13) {
            allFoundationsFinished = false;
        }
    }

    return allFoundationsFinished;
}

bool tableSolvable() {
    bool allColumnsRevealed = true;

    for (Pile &column : states.back().tableau) {
        if (!column.cards.empty() && !column.cards.front().flip) {
            allColumnsRevealed = false;
        }
    }

    return allColumnsRevealed;
}

int maxTableauLines() {
    int numCards = 0;

    for (Pile &column : states.back().tableau) {
        if (column.cards.size() > numCards) {
            numCards = (int)column.cards.size();
        }
    }

    int numLines = (numCards * 2) + 9;
    if (numLines == 9) numLines += 2;

    return numLines;
}

void printTable(const std::string &prompt) {
    std::ostringstream output;

    int maxLines = maxTableauLines() + 16;

    output << "\x1b[2F";
    for (int i = 0; i < maxLines; i++) { output << "\x1b[2K\n"; }
    output << "\x1b[0J\x1b[" << maxLines << "A";

    output << genPrompt(prompt);
    output << "\x1b[2E";

    output << genStock();
    output << "\x1b[10A\x1b[1C";
    output << genWaste();
    output << "\x1b[10A\x1b[1C";
    output << genFoundations();
    output << "\x1b[2E";
    output << genTableau();

    int promptOffset = 12 + (int)std::to_string(states.back().score).length() + (int)prompt.length();

    output << "\x1b[" << maxLines - 2 << "F";
    output << "\x1b[" << promptOffset << "C";

    std::cout << output.str();
}

std::string genPrompt(const std::string &prompt) {
    std::ostringstream output;

    std::string scoreString = std::to_string(states.back().score);

    int numScoreDigits = (int)scoreString.length();
    int numPromptChars = (int)prompt.length() + numScoreDigits;

    std::string promptString = prompt + genSpaces(83 - numPromptChars);

    std::string scoreDashes = genDashes(numScoreDigits);
    std::string promptDashes = genDashes(83 - numScoreDigits);

    output << "┌────────" << scoreDashes << "─┬─" << promptDashes << "─┐\x1b[1E";
    output << "│ Score: " << scoreString << " │ " << promptString << " │\x1b[1E";
    output << "└────────" << scoreDashes << "─┴─" << promptDashes << "─┘";

    return output.str();
}

std::string genStock() {
    std::ostringstream output;

    Pile &stock = states.back().stock;

    int numCards = (int)stock.cards.size();

    output << "Stock" << "\x1b[1E";
    output << "(" << numCards << " Cards)" << "\x1b[1E";

    if (numCards > 0) output << stock.cards.back().back;
    else output << stock.empty;

    return output.str();
}

std::string genWaste() {
    std::ostringstream output;

    Pile &waste = states.back().waste;

    int numCards = (int)waste.cards.size();
    int numDigits = (int)std::to_string(numCards).length();

    output << "Waste" << "\x1b[1B\x1b[5D";
    output << "(" << numCards << " Cards)" << "\x1b[1B\x1b[" << 8 + numDigits << "D";

    int space = 14;

    if (numCards > 0) {
        if (states.back().draw == 3) {
            int index = numCards - 3;
            if (index < 0) index = 0;

            for (int i = index; i < numCards - 1; i++) {
                output << waste.cards[i].face << "\x1b[8D ";
                space -= 6;

                for (int j = 0; j < 8; j++) {
                    output << "\x1b[1A\x1b[1D ";
                }
            }
        }

        output << waste.cards.back().face;

    } else {
        output << waste.empty;
    }

    output << "\x1b[" << space << "C";

    return output.str();
}

std::string genFoundations() {
    std::ostringstream output;

    for (int i = 0; i < 4; i++) {
        Pile &foundation = states.back().foundations[i];

        int numCards = (int)foundation.cards.size();
        int numDigits = (int)std::to_string(numCards).length();

        output << "Foundation " << i + 1 << "\x1b[1B\x1b[12D";
        output << "(" << numCards << " Cards)" << "\x1b[1B\x1b[" << 8 + numDigits << "D";

        if (numCards > 0) output << foundation.cards.back().face;
        else output << foundation.empty;

        if (i < 3) output << "\x1b[10A\x1b[1C";
    }

    return output.str();
}

std::string genTableau() {
    std::ostringstream output;

    for (int i = 0; i < 7; i++) {
        Pile &column = states.back().tableau[i];

        int numCards = (int)column.cards.size();
        int numDigits = (int)std::to_string(numCards).length();
        int numLines = (numCards * 2) + 9;

        output << "Column " << i + 1 << "\x1b[1B\x1b[8D";
        output << "(" << numCards << " Cards)" << "\x1b[1B\x1b[" << 8 + numDigits << "D";

        if (numCards > 0) {
            for (int j = 0; j < numCards; j++) {
                Card &card = column.cards[j];

                output << (card.flip ? card.face : card.back);
                if (j < numCards - 1) output << "\x1b[6A\x1b[13D";
            }

        } else {
            output << column.empty;
            numLines += 2;
        }

        int difference = maxTableauLines() - numLines;

        if (i < 6) output << "\x1b[" << numLines - 1 << "A\x1b[1C";
        else if (difference > 0) output << "\x1b[" << difference << "B";
    }

    return output.str();
}

std::string genSpaces(int num) {
    std::ostringstream output;

    for (int i = 0; i < num; i++) {
        output << " ";
    }

    return output.str();
}

std::string genDashes(int num) {
    std::ostringstream output;

    for (int i = 0; i < num; i++) {
        output << "─";
    }

    return output.str();
}
