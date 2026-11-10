#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

class BuoyancySimulation {
private:
    static constexpr unsigned int windowWidth = 1000;
    static constexpr unsigned int windowHeight = 650;
    static constexpr float gravity = 9.81f;
    static constexpr float bodyWidth = 82.0f;
    static constexpr float bodyHeight = 82.0f;
    static constexpr float surfaceY = 285.0f;
    static constexpr float tankLeft = 105.0f;
    static constexpr float tankRight = 455.0f;
    static constexpr float tankBottom = 585.0f;

    sf::RenderWindow window;
    sf::Font font;

    float bodyDensity = 700.0f;
    float liquidDensity = 1000.0f;
    float bodyCenterY = surfaceY - 125.0f;
    float velocity = 0.0f;
    float acceleration = gravity;
    float submergedFraction = 0.0f;
    bool running = true;

public:
    BuoyancySimulation()
        : window(sf::VideoMode({windowWidth, windowHeight}), "Buoyancy simulation - Variant 16") {
        window.setFramerateLimit(60);
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
            throw std::runtime_error("Cannot load font C:/Windows/Fonts/arial.ttf");
        }
    }

    void run() {
        sf::Clock clock;

        while (window.isOpen()) {
            float dt = std::min(clock.restart().asSeconds(), 0.03f);
            handleEvents();

            if (running) {
                updatePhysics(dt);
            }

            drawScene();
        }
    }

private:
    void handleEvents() {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Space) {
                    running = !running;
                }
                if (key->code == sf::Keyboard::Key::R) {
                    reset();
                }
                if (key->code == sf::Keyboard::Key::Up) {
                    bodyDensity = std::min(bodyDensity + 50.0f, 3000.0f);
                }
                if (key->code == sf::Keyboard::Key::Down) {
                    bodyDensity = std::max(bodyDensity - 50.0f, 100.0f);
                }
                if (key->code == sf::Keyboard::Key::Right) {
                    liquidDensity = std::min(liquidDensity + 50.0f, 1500.0f);
                }
                if (key->code == sf::Keyboard::Key::Left) {
                    liquidDensity = std::max(liquidDensity - 50.0f, 500.0f);
                }
            }

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button == sf::Mouse::Button::Left) {
                    sf::Vector2f position(
                        static_cast<float>(mouse->position.x),
                        static_cast<float>(mouse->position.y)
                    );

                    if (buttonAt(position, {{600.0f, 535.0f}, {155.0f, 42.0f}})) {
                        running = !running;
                    }
                    if (buttonAt(position, {{775.0f, 535.0f}, {155.0f, 42.0f}})) {
                        reset();
                    }
                    if (buttonAt(position, {{835.0f, 205.0f}, {42.0f, 38.0f}})) {
                        bodyDensity = std::min(bodyDensity + 50.0f, 3000.0f);
                    }
                    if (buttonAt(position, {{885.0f, 205.0f}, {42.0f, 38.0f}})) {
                        bodyDensity = std::max(bodyDensity - 50.0f, 100.0f);
                    }
                    if (buttonAt(position, {{835.0f, 300.0f}, {42.0f, 38.0f}})) {
                        liquidDensity = std::min(liquidDensity + 50.0f, 1500.0f);
                    }
                    if (buttonAt(position, {{885.0f, 300.0f}, {42.0f, 38.0f}})) {
                        liquidDensity = std::max(liquidDensity - 50.0f, 500.0f);
                    }
                }
            }
        }
    }

    bool buttonAt(const sf::Vector2f& point, const sf::FloatRect& bounds) const {
        return bounds.contains(point);
    }

    void reset() {
        bodyCenterY = surfaceY - 125.0f;
        velocity = 0.0f;
        acceleration = gravity;
        submergedFraction = 0.0f;
        running = true;
    }

    void updatePhysics(float dt) {
        float bodyBottom = bodyCenterY + bodyHeight / 2.0f;
        float submergedHeight = std::clamp(bodyBottom - surfaceY, 0.0f, bodyHeight);

        submergedFraction = submergedHeight / bodyHeight;

        float buoyancyToWeightRatio = (liquidDensity / bodyDensity) * submergedFraction;
        acceleration = gravity * (1.0f - buoyancyToWeightRatio);

        velocity += acceleration * dt;
        bodyCenterY += velocity * dt;

        float lowestCenter = tankBottom - bodyHeight / 2.0f;
        if (bodyCenterY > lowestCenter) {
            bodyCenterY = lowestCenter;
            velocity = 0.0f;
        }

        float highestCenter = surfaceY - bodyHeight / 2.0f - 100.0f;
        if (bodyCenterY < highestCenter) {
            bodyCenterY = highestCenter;
            velocity = 0.0f;
        }
    }

    void drawText(const std::string& value, float x, float y, unsigned int size = 20,
                  sf::Color color = sf::Color(235, 238, 245)) {
        sf::Text text(font, value, size);
        text.setPosition({x, y});
        text.setFillColor(color);
        window.draw(text);
    }

    void drawButton(const std::string& label, const sf::FloatRect& bounds) {
        sf::RectangleShape button({bounds.size.x, bounds.size.y});
        button.setPosition(bounds.position);
        button.setFillColor(sf::Color(54, 69, 91));
        button.setOutlineColor(sf::Color(112, 139, 173));
        button.setOutlineThickness(1.5f);
        window.draw(button);

        sf::Text text(font, label, 17);
        text.setFillColor(sf::Color::White);
        text.setPosition({bounds.position.x + 10.0f, bounds.position.y + 8.0f});
        window.draw(text);
    }

    void drawScene() {
        window.clear(sf::Color(20, 27, 39));

        drawText("BUOYANCY: BODY AT THE AIR-LIQUID INTERFACE", 35.0f, 22.0f, 25);
        drawText("Variant 16 | Gravity and Archimedes' force", 38.0f, 58.0f, 17,
                 sf::Color(163, 181, 204));

        sf::RectangleShape tank({tankRight - tankLeft, tankBottom - surfaceY});
        tank.setPosition({tankLeft, surfaceY});
        tank.setFillColor(sf::Color(40, 111, 160, 105));
        tank.setOutlineColor(sf::Color(185, 207, 226));
        tank.setOutlineThickness(3.0f);
        window.draw(tank);

        sf::RectangleShape waterSurface({tankRight - tankLeft, 4.0f});
        waterSurface.setPosition({tankLeft, surfaceY});
        waterSurface.setFillColor(sf::Color(105, 202, 239));
        window.draw(waterSurface);

        sf::RectangleShape body({bodyWidth, bodyHeight});
        body.setPosition({tankLeft + (tankRight - tankLeft - bodyWidth) / 2.0f,
                          bodyCenterY - bodyHeight / 2.0f});
        body.setFillColor(sf::Color(232, 151, 72));
        body.setOutlineColor(sf::Color(255, 218, 165));
        body.setOutlineThickness(2.0f);
        window.draw(body);

        drawText("AIR", 118.0f, 245.0f, 16, sf::Color(190, 202, 218));
        drawText("LIQUID", 118.0f, 305.0f, 16, sf::Color(190, 225, 245));
        drawText("Container", 225.0f, 600.0f, 16, sf::Color(163, 181, 204));

        drawText("SIMULATION PARAMETERS", 535.0f, 112.0f, 21);
        drawText("Body density: " + number(bodyDensity) + " kg/m3", 555.0f, 170.0f);
        drawButton("+", {{835.0f, 205.0f}, {42.0f, 38.0f}});
        drawButton("-", {{885.0f, 205.0f}, {42.0f, 38.0f}});
        drawText("Liquid density: " + number(liquidDensity) + " kg/m3", 555.0f, 265.0f);
        drawButton("+", {{835.0f, 300.0f}, {42.0f, 38.0f}});
        drawButton("-", {{885.0f, 300.0f}, {42.0f, 38.0f}});

        drawText("RESULTS", 555.0f, 365.0f, 21);
        drawText("Submerged part: " + number(submergedFraction * 100.0f) + " %", 555.0f, 405.0f);
        drawText("Vertical speed: " + number(velocity) + " m/s", 555.0f, 438.0f);
        drawText("Acceleration: " + number(acceleration) + " m/s2", 555.0f, 471.0f);

        drawButton(running ? "Pause (Space)" : "Start (Space)", {{600.0f, 535.0f}, {155.0f, 42.0f}});
        drawButton("Reset (R)", {{775.0f, 535.0f}, {155.0f, 42.0f}});

        drawText("Controls: Up/Down - body density | Left/Right - liquid density",
                 35.0f, 625.0f, 14, sf::Color(163, 181, 204));

        window.display();
    }

    std::string number(float value) const {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(1) << value;
        return stream.str();
    }
};

int main() {
    try {
        BuoyancySimulation simulation;
        simulation.run();
    }
    catch (const std::exception& error) {
        return 1;
    }

    return 0;
}
