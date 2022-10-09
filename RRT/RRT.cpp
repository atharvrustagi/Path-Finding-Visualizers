#include <iostream>
#include <SFML/Graphics.hpp>

// system variables
const int dimX = 500, dimY = 500; // dimensions for the search space
const int maxNodes = 100;   // maximum number of nodes that are created
const std::pair<int, int> src = {10, 10},   // source
                          dst = {490, 490}; // destination



sf::RenderWindow window(sf::VideoMode(dimX, dimY), "RRT Algorithm");

// put all the initialisation stuff in this function
const static auto init = []() {
    printf("Initialising.\n");
    srand(time(0));
    window.clear(sf::Color(255, 255, 255));
    printf("Initialisation done.\n");
    return 0;
} ();

void draw() {
    window.clear(sf::Color(255, 255, 255));
    sf::sleep(sf::seconds(1));
    window.display();
}

int main() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();
        draw();
    }
}