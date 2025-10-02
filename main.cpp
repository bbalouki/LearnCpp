#include <iostream>
#include <memory>

struct Sword {
    int Damage{10};
};

class Character {
   public:
    std::unique_ptr<Sword> Weapon{};
    Character() : Weapon{std::make_unique<Sword>()} {};

    Character(const Character& Source)
        : Weapon{std::make_unique<Sword>(*Source.Weapon)} {}

    Character& operator=(const Character& Source) {
        Weapon = std::make_unique<Sword>(*Source.Weapon);

        return *this;
    }
};

int main() {
    Character A{};
    A.Weapon->Damage = 20;

    Character B{A};

    std::cout << "\nA's Weapon:         " << A.Weapon;
    std::cout << "\nA's Weapon Damage:  " << A.Weapon->Damage;
    std::cout << "\nB's Weapon:         " << B.Weapon;
    std::cout << "\nB's Weapon Damage:  " << B.Weapon->Damage;
    std::cout << '\n';
}
