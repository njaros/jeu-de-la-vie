#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_set>
#include <list>
#include <set>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #include <windows.h>
#else
    #include <unistd.h>

#endif

#define WIDTH 1440
#define HEIGH 720
#define SCALE 10
#define INIT_X WIDTH / (2 * SCALE)
#define INIT_Y HEIGH / (2 * SCALE)

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

    Box()
    {
        setSize({SCALE, SCALE});
    }

};

class Filled: public Box
{

public:

    Filled(const sf::Vector2i& pos)
    {
        setPosition({float(pos.x * SCALE), float(pos.y * SCALE)});
        setFillColor(sf::Color::Blue);
        setOutlineColor(sf::Color::Black);
        setOutlineThickness(1);
    }
};

class Empty: public Box
{

public:

    Empty(const sf::Vector2i& pos)
    {
        setPosition({float(pos.x * SCALE), float(pos.y * SCALE)});
        setFillColor(sf::Color::Black);
    }
};

class Population
{
    Set _pop;
    Lst _neighbours;
    Lst _borns;
    Lst _deads;
    sf::RenderWindow& _window;

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

    Population(sf::RenderWindow& window): _window(window) {}

    Population(sf::RenderWindow& window, const std::initializer_list<sf::Vector2i>& init): _window(window)
    {
        for (const sf::Vector2i& v : init)
        {
            _pop.insert(v);
        }
    }

    Population(sf::RenderWindow& window, const std::vector<sf::Vector2i>& init): _window(window)
    {
        for (const sf::Vector2i& v : init)
        {
            _pop.insert(v);
        }
    }

    void add(const std::vector<sf::Vector2i>& toAdd)
    {
        for (const sf::Vector2i& v : toAdd)
        {
            _pop.insert(v);
        }
    }

    void add(const std::initializer_list<sf::Vector2i>& toAdd)
    {
        for (const sf::Vector2i& v : toAdd)
        {
            _pop.insert(v);
        }
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
            _window.draw(Filled(v));
        }
    }

    void proceed2()
    {
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

    Population population(window, cross({INIT_X, INIT_Y}));

    for (int i = 1; i < 21; ++i)
        population.add(planer1({INIT_X - (10 * i), INIT_Y + (10 * i)}));

    population.add(square({INIT_X + 10, INIT_Y - 10}));


    int i = 0;

    population.draw();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.display();

        #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
            Sleep(5);
        #else
            usleep(5000);
        #endif
        
        population.proceed2();
    }
}