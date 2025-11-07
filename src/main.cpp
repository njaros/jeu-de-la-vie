#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_set>
#include <list>
#include <set>
#include <chrono>

#define WIDTH 1440
#define HEIGH 720
#define INIT_ZOOM 10
#define INIT_X WIDTH / (2 * INIT_ZOOM)
#define INIT_Y HEIGH / (2 * INIT_ZOOM)

typedef std::chrono::steady_clock Clock;
typedef std::chrono::time_point<Clock> Time;

std::vector<sf::Vector2i> planer1(const sf::Vector2i& pos) {
    int x = pos.x;
    int y = pos.y;
    
    return {
        {x, y},
        {x + 1, y},
        {x + 2, y},
        {x + 2, y + 1},
        {x + 1, y + 2}
    };
}

std::vector<sf::Vector2i> square(const sf::Vector2i& pos) {
    int x = pos.x;
    int y = pos.y;
    
    return {
        {x, y},
        {x + 1, y},
        {x, y + 1},
        {x + 1, y + 1}
    };
}

std::vector<sf::Vector2i> test1(const sf::Vector2i& pos) {
    int x = pos.x;
    int y = pos.y;
    
    return {
        {x, y},
        {x + 1, y},
        {x + 2, y},
        {x + 2, y + 1},
        {x + 1, y + 2},
        {x + 2, y + 2}
    };
}

std::vector<sf::Vector2i> cross(const sf::Vector2i& pos) {
    int x = pos.x;
    int y = pos.y;
    
    return {
        {x, y},
        {x + 1, y + 1},
        {x + 2, y + 2},
        {x - 1, y - 1},
        {x - 2, y - 2},
        {x + 1, y - 1},
        {x + 2, y - 2},
        {x - 1, y + 1},
        {x - 2, y + 2},
    };
}


template<> struct std::hash<sf::Vector2i>
{
    std::size_t operator()(const sf::Vector2i& v) const noexcept {
        std::size_t h1 = std::hash<int>{}(v.x);
        std::size_t h2 = std::hash<int>{}(v.y);
        return h1 ^ (h2 << 1);
    }
};

typedef std::list<sf::Vector2i> Lst;
typedef std::unordered_set<sf::Vector2i> Set;

class Box: public sf::RectangleShape
{

public:

    Box(float zoom)
    {
        setSize({zoom - 1, zoom - 1});
    }

};

class Filled: public Box
{

public:

    Filled(const sf::Vector2i& pos, float zoom = 1): Box(zoom)
    {
        setPosition({float(pos.x * zoom), float(pos.y * zoom)});
        setFillColor(sf::Color::Blue);
        setOutlineColor(sf::Color::Black);
        setOutlineThickness(1);
    }
};

class MousePos: public Box
{
public:
    MousePos(const sf::Vector2i& pos, float zoom = 1): Box(zoom)
    {
        setPosition({float(pos.x * zoom), float(pos.y * zoom)});
        setFillColor(sf::Color::Green);
        setOutlineColor(sf::Color::White);
        setOutlineThickness(1);
    }
};

class FillAndMouse: public Box
{
public:
    FillAndMouse(const sf::Vector2i& pos, float zoom = 1): Box(zoom)
    {
        setPosition({float(pos.x * zoom), float(pos.y * zoom)});
        setFillColor(sf::Color::Red);
        setOutlineColor(sf::Color::Black);
        setOutlineThickness(1);
    }
};

class Empty: public Box
{

public:

    Empty(const sf::Vector2i& pos, float zoom = 1): Box(zoom)
    {
        setPosition({float(pos.x * zoom), float(pos.y * zoom)});
        setFillColor(sf::Color::Black);
        setOutlineColor(sf::Color::Black);
        setOutlineThickness(1);
    }
};

class Game
{
    Set _pop;
    Lst _neighbours;
    Lst _borns;
    Lst _deads;
    double _speedPeriodSecond;
    Time _lastMove;
    float _zoom;
    long _shiftX;
    long _shiftY;
    sf::RenderWindow& _window;
    std::optional<sf::Vector2i> _mousePosition;

    void _setNeighboursAndDeads()
    {
        sf::Vector2i v;
        sf::Vector2i check;
        int count;

        _deads.clear();
        _neighbours.clear();

        for (Set::const_iterator cit = _pop.begin(); cit != _pop.end(); ++cit)
        {
            count = 0;
            v = *cit;

            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    if (i != 0 || j != 0)
                    {
                        check = {v.x + i, v.y + j};
                        if (_pop.find(check) == _pop.end())
                        {
                            _neighbours.push_back(check);
                        }
                        else
                        {
                            ++count;
                        }
                    }
                }
            }
            if (count < 2 || count > 3)
            {
                _deads.push_back(v);
            }
        }
    }

    void _setBorns()
    {
        sf::Vector2i v;
        sf::Vector2i check;
        int count;

        _borns.clear();
        for (Lst::const_iterator cit = _neighbours.begin(); cit != _neighbours.end(); ++cit)
        {
            count = 0;
            v = *cit;

            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    if (i != 0 || j != 0)
                    {
                        check = {v.x + i, v.y + j};
                        if (_pop.find(check) != _pop.end())
                        {
                            ++count;
                        }
                    }
                }
            }
            if (count == 3)
            {
                _borns.push_back(v);
            }
        }
    }

public:

    enum objDir
    {
        Right,
        Bottom,
        Left,
        Top
    };

    Game(sf::RenderWindow& window): _window(window), _zoom(INIT_ZOOM), _shiftX(0), _shiftY(0), _speedPeriodSecond(0.1), _lastMove(std::chrono::steady_clock::now()) {}

    Game(sf::RenderWindow& window, const std::initializer_list<sf::Vector2i>& init): _window(window), _zoom(INIT_ZOOM), _shiftX(0), _shiftY(0), _speedPeriodSecond(0.1), _lastMove(std::chrono::steady_clock::now())
    {
        for (const sf::Vector2i& v : init)
        {
            _pop.insert(v);
        }
    }

    Game(sf::RenderWindow& window, const std::vector<sf::Vector2i>& init): _window(window), _zoom(INIT_ZOOM), _shiftX(0), _shiftY(0), _speedPeriodSecond(0.1), _lastMove(std::chrono::steady_clock::now())
    {
        for (const sf::Vector2i& v : init)
        {
            _pop.insert(v);
        }
    }

    void addCannon(const sf::Vector2i& pos, objDir dir = Right)
    {
        int x = pos.x;
        int y = pos.y;

        add({
            {x, y}, {x + 1, y}, {x, y + 1}, {x + 1, y + 1}, //Left square
            {x + 4, y - 2}, {x + 5, y - 1}, {x + 6, y - 1}, //Left bignou
            {x + 9, y - 2}, {x + 9, y - 3}, {x + 9, y + 2}, {x + 9, y + 3}, {x + 10, y}, //Eyes and nose
            {x + 11, y - 3}, {x + 12, y - 2}, {x + 12, y - 1}, {x + 13, y - 1}, //Top of mouth
            {x + 11, y + 3}, {x + 12, y + 2}, {x + 12, y + 1}, {x + 13, y + 1}, //Bottom of mouth
            {x + 14, y}, {x + 15, y}, //tongue
            {x + 23, y - 1}, {x + 24, y}, {x + 25, y + 1}, {x + 26, y + 1}, {x + 26, y}, //Bottom of bow
            {x + 23, y - 3}, {x + 24, y - 4}, {x + 25, y - 5}, {x + 26, y - 5}, {x + 26, y - 4}, //Top of the bow
            {x + 23, y - 2}, {x + 26, y - 2}, //Middle of the bow
            {x + 29, y - 2}, {x + 30, y - 3}, {x + 30, y - 1}, {x + 31, y - 3}, {x + 31, y - 1}, {x + 32, y - 2}, //Arrow
            {x + 34, y - 2}, {x + 35, y - 2}, {x + 34, y - 1}, {x + 35, y - 1}, //Right square
        });
    }

    void addOrDelete(const sf::Vector2i& pos)
    {
        sf::Vector2i truePos = {(int)(((pos.x / this->_zoom) - this->_shiftX)), (int)(((pos.y / this->_zoom) - this->_shiftY))};
        if (_pop.find(truePos) != _pop.end())
            _pop.erase(truePos);
        else
            _pop.insert(truePos);
    }

    void add(const sf::Vector2i& toAdd)
    {
        _pop.insert({(int)(((toAdd.x / this->_zoom) - this->_shiftX)), (int)(((toAdd.y / this->_zoom) - this->_shiftY))});
    }

    void add(const std::vector<sf::Vector2i>& toAdd)
    {
        for (const sf::Vector2i& v : toAdd)
        {
            _pop.insert({v.x - this->_shiftX, v.y - this->_shiftY});
        }
    }

    void add(const std::initializer_list<sf::Vector2i>& toAdd)
    {
        for (const sf::Vector2i& v : toAdd)
        {
            _pop.insert({v.x - this->_shiftX, v.y - this->_shiftY});
        }
    }

    void shiftX(long n)
    {
        this->_shiftX += n;
    }

    void shiftY(long n)
    {
        this->_shiftY += n;
    }

    void zoomIn()
    {
        this->_zoom *= 1.5;
    }

    void zoomOut()
    {
        this->_zoom *= 0.6667;
    }

    void setMousePos(const sf::Vector2i& pos)
    {
        this->_mousePosition = {(int)((pos.x / this->_zoom) - this->_shiftX), (int)((pos.y / this->_zoom) - this->_shiftY)};
    }

    void resetMousePos()
    {
        this->_mousePosition.reset();
    }

    void setSpeed(double d)
    {
        this->_speedPeriodSecond = d;
    }

    void proceed()
    {
        _setNeighboursAndDeads();
        _setBorns();

        for (const sf::Vector2i& v : _borns)
        {
            _pop.insert(v);
            _window.draw(Filled(v));
        }

        for (const sf::Vector2i& v : _deads)
        {
            _pop.erase(v);
            _window.draw(Empty(v));
        }

    }

    void draw()
    {
        _window.clear(sf::Color::Black);

        for (const sf::Vector2i& v : _pop)
        {
            _window.draw(Filled(
                {
                    v.x + this->_shiftX,
                    v.y + this->_shiftY
                },
                this->_zoom
            ));
        }

        if (_mousePosition.has_value())
        {
            sf::Vector2i pos = _mousePosition.value();
            if (_pop.find(_mousePosition.value()) != _pop.end())
                _window.draw(FillAndMouse(
                    {
                        pos.x + this->_shiftX,
                        pos.y + this->_shiftY
                    },
                    this->_zoom
                ));
            else
                _window.draw(MousePos(
                    {
                        pos.x + this->_shiftX,
                        pos.y + this->_shiftY
                    },
                    this->_zoom
                ));
        }

    }

    void proceed2(bool pause = false)
    {
        if (!pause)
        {
            Time now = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = (this->_lastMove - now);
            if (diff.count() + this->_speedPeriodSecond < 0)
            {
                this->_lastMove = now;
                _setNeighboursAndDeads();
                _setBorns();
                
                for (const sf::Vector2i& v : _borns)
                {
                    _pop.insert(v);
                }
                
                for (const sf::Vector2i& v : _deads)
                {
                    _pop.erase(v);
                }
            }
        }

        draw();
    }

};

sf::RenderWindow initWindow()
{
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGH}), "Jeu de la vie");
    sf::Image logo{};

    window.setFramerateLimit(144);
    window.clear(sf::Color::Black);

    if (!logo.loadFromFile("img/logo.png"))
    {
        std::cerr << "image not found";
    }
    else
    {
        window.setIcon(logo.getSize(), logo.getPixelsPtr());
    }

    return window;
}

int main()
{
    sf::RenderWindow window = initWindow();
    const sf::Event::KeyPressed* keyPtr = nullptr;
    const sf::Event::MouseButtonPressed* mousePtr = nullptr;
    bool pause = false;

    Game game(window, cross({INIT_X, INIT_Y}));

    for (int i = 1; i < 21; ++i)
        game.add(planer1({INIT_X - (10 * i), INIT_Y + (10 * i)}));

    game.add(square({INIT_X + 10, INIT_Y - 10}));

    game.addCannon({INIT_X - 50, INIT_Y - 30});

    game.draw();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            if (event->is<sf::Event::KeyPressed>())
            {
                keyPtr = event->getIf<sf::Event::KeyPressed>();
                switch (keyPtr->code)
                {
                    case sf::Keyboard::Key::Left:
                        game.shiftX(1 + (9 * keyPtr->shift));
                    break;
    
                    case sf::Keyboard::Key::Up:
                        game.shiftY(1 + (9 * keyPtr->shift));
                    break;

                    case sf::Keyboard::Key::Right:
                        game.shiftX(-(1 + (9 * keyPtr->shift)));
                    break;

                    case sf::Keyboard::Key::Down:
                        game.shiftY(-(1 + (9 * keyPtr->shift)));
                    break;

                    case sf::Keyboard::Key::Space:
                        pause = !pause;
                    break;

                    case sf::Keyboard::Key::Escape:
                        window.close();
                    break;

                    case sf::Keyboard::Key::Num1:
                        game.setSpeed(0.1);
                    break;

                    case sf::Keyboard::Key::Num2:
                        game.setSpeed(0.05);
                    break;

                    case sf::Keyboard::Key::Num3:
                        game.setSpeed(0.02);
                    break;

                    case sf::Keyboard::Key::Num4:
                        game.setSpeed(0.01);
                    break;

                    case sf::Keyboard::Key::Add:
                        game.zoomIn();
                    break;

                    case sf::Keyboard::Key::Subtract:
                        game.zoomOut();
                    break;

                default:
                    break;
                }
            }

            if (event->is<sf::Event::MouseButtonPressed>())
            {
                mousePtr = event->getIf<sf::Event::MouseButtonPressed>();
                if (mousePtr->button == sf::Mouse::Button::Left)
                {
                    game.addOrDelete(mousePtr->position);
                }
            }

            if (event->is<sf::Event::MouseMoved>())
            {
                sf::Vector2i pos = event->getIf<sf::Event::MouseMoved>()->position;
                game.setMousePos(pos);
            }

            if (event->is<sf::Event::MouseLeft>())
            {
                game.resetMousePos();
            }
        }

        window.display();
        
        game.proceed2(pause);
    }
}