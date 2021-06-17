#include<bits/stdc++.h>
#include<SFML/Graphics.hpp>
using namespace sf;

const int dimX = 1000, 
          dimY = 800,
          gap = 100, mx = 1000,
          gridSizeY = dimX/gap, gridSizeX = dimY/gap;
int adjdist = 100, diadist = 141;
int srcX=-1, srcY=-1, dstX=-1, dstY=-1;
int grid[gridSizeX][gridSizeY], 
    fcost[gridSizeX][gridSizeY], 
    gcost[gridSizeX][gridSizeY], 
    hcost[gridSizeX][gridSizeY];

std::set<std::tuple<int, int, int, int>> mp;
std::vector<std::pair<int, int>> vis = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};     // for adjacent neighbors only

void draw(RenderWindow &win);
void plot(Vector2i &pos, bool done=0);
void initialise();
bool find_path();
void display_grid();
int dist(int a, int b);
bool out_of_bounds(int x, int y);

int main()  {
    RenderWindow win(VideoMode(dimX, dimY), "A* Debugger");
    initialise();
    bool drag=0, start=0;
    while (win.isOpen())    {
        Event event;
        while (win.pollEvent(event))    {
            if (event.type == Event::Closed)
                win.close();
            else if (start) {
                if (event.type == Event::MouseButtonReleased)  {
                    Vector2i pos = Mouse::getPosition(win);
                    plot(pos, 1);
                    draw(win);
                }
            }
            else if (dstX != -1 && event.type == Event::KeyPressed) {
                if (Keyboard::isKeyPressed(Keyboard::Enter))    {
                    start = 1;
                    drag = 0;
                }
            }
            else if (dstX!=-1 && event.type == Event::MouseButtonPressed)   {
                drag = !drag;
            }
            else if (event.type == Event::MouseButtonReleased)
                if (event.mouseButton.button==Mouse::Left)  {
                    Vector2i pos = Mouse::getPosition(win);
                    plot(pos);
                }
        }
        if (drag && !start)   {
            Vector2i pos = Mouse::getPosition(win);
            plot(pos);
            draw(win);
        }
        if (!start) {
            draw(win);
        }
    }
    return 0;
}

auto lineColor = Color(100, 200, 148),
     pathColor = Color(255, 255, 255), 
     travColor = Color(255, 18, 128),
     unviColor = Color(18, 155, 18),
     wallColor = Color(128, 128, 128), 
     srcColor = Color(0, 0, 0), 
     dstColor = Color(0, 0, 0), 
     arrowColor = Color(18, 255, 18);

RectangleShape square(Vector2f(gap, gap));
RectangleShape lineRec(Vector2f(1, 2));

Font font;
Text text;

void initialise()   {
    font.loadFromFile("arial.ttf");
    text.setFont(font);
    text.setCharacterSize(gap/5);
    text.setFillColor(sf::Color::Black);
    lineRec.setFillColor(lineColor);
    square.setOrigin(float(gap)/2, float(gap)/2);
}

bool out_of_bounds(int x, int y)    {
    return x<0 || y<0 || x>=gridSizeX || y>=gridSizeY;
}

int dist(int a, int b)  {
    if (abs(a) == abs(b))
        return diadist;
    return adjdist;
}

void plot(Vector2i &pos, bool done)    {
    int px = std::min(dimX, pos.x)/gap, py = std::min(dimY, pos.y)/gap;
    px = std::max(0, pos.x)/gap, py = std::max(0, pos.y)/gap;
    std::swap(px, py);
    if (srcX == -1) {
        srcY = py;
        srcX = px;
        grid[srcX][srcY] = 301;
        // src.setPosition(srcY * gap + (float)gap/2, srcX * gap + (float)gap/2);
        return;
    }
    if (dstX == -1) {
        dstX = px;
        dstY = py;
        grid[dstX][dstY] = 301;
        // hcost calculations
        for (int i=0; i<gridSizeX; ++i) {
            for (int j=0; j<gridSizeY; ++j) {
                gcost[i][j] = INT_MAX;
                fcost[i][j] = INT_MAX;
                hcost[i][j] = (abs(dstX-i) + abs(dstY-j)) * adjdist;    // (std::max(abs(dstX-i), abs(dstY-j)) - std::min(abs(dstX-i), abs(dstY-j))) * adjdist + std::min(abs(dstX-i), abs(dstY-j)) * diadist;    // 
            }
        }
        gcost[srcX][srcY] = 0;
        fcost[srcX][srcY] = hcost[srcX][srcY];
        mp.insert({fcost[srcX][srcY], hcost[srcX][srcY], srcX, srcY});
        // dst.setPosition(dstY * gap + (float)gap/2, dstX * gap + (float)gap/2);
        return;
    }
    if ( !done &&( (px==srcX && py==srcY) || (px==dstX && py==dstY)) )
        return;
    if (!done)  {
        grid[px][py] = 201;
        return;
    }
    auto it = mp.find({fcost[px][py], hcost[px][py], px, py});
    if (it == mp.end())
        return;
    grid[px][py] = 101;
    mp.erase(it);
    for (auto &v: vis)  {
        int nbx = px + v.first, nby = py + v.second;
        if (grid[nbx][nby] && grid[nbx][nby] != 401 && !(nbx==dstX && nby==dstY) || out_of_bounds(nbx, nby))
            continue;
        grid[nbx][nby] = 401;
        auto nit = mp.find({fcost[nbx][nby], hcost[nbx][nby], nbx, nby});
        if (gcost[nbx][nby] > gcost[px][py] + dist(v.first, v.second)) {
            gcost[nbx][nby] = gcost[px][py] + dist(v.first, v.second);
        }
        fcost[nbx][nby] = gcost[nbx][nby] + hcost[nbx][nby];
        if (nit != mp.end())
            mp.erase(nit);
        mp.insert({fcost[nbx][nby], hcost[nbx][nby], nbx, nby});
    }

}

void draw_texts(RenderWindow &win, int x, int y)  {
    FloatRect f;
    text.setString(std::to_string(gcost[x][y]));
    f = text.getLocalBounds();
    text.setOrigin((f.top+f.height)/2, (f.left+f.width)/2);
    text.setPosition(y*gap + (float)gap/2, x*gap + (float)gap/6);
    win.draw(text);

    text.setString(std::to_string(hcost[x][y]));
    f = text.getLocalBounds();
    text.setOrigin((f.top+f.height)/2, (f.left+f.width)/2);
    text.setPosition(y*gap + (float)gap/2, x*gap + (float)gap/2);
    win.draw(text);

    text.setString(std::to_string(fcost[x][y]));
    f = text.getLocalBounds();
    text.setOrigin((f.top+f.height)/2, (f.left+f.width)/2);
    text.setPosition(y*gap + (float)gap/2, x*gap + 5*(float)gap/6);
    win.draw(text);
}

void draw(RenderWindow &win) {
    win.clear();
    for (int i=0; i<gridSizeX; ++i) {
        for (int j=0; j<gridSizeY; ++j) {
            if (!grid[i][j])
                continue;
            
            if (grid[i][j] > 400) {   // unvisited node (in set)
                square.setFillColor(unviColor);
                square.setPosition(gap*j + (float)gap/2, gap*i + (float)gap/2);
                win.draw(square);
                draw_texts(win, i, j);
            }
            else if (grid[i][j] > 300) {   // path node
                square.setFillColor(pathColor);
                square.setPosition(gap*j + (float)gap/2, gap*i + (float)gap/2);
                win.draw(square);
            }
            else if (grid[i][j] > 200)  {  // wall node
                square.setFillColor(wallColor);
                square.setPosition(gap*j + (float)gap/2, gap*i + (float)gap/2);
                win.draw(square);
            }
            else if (grid[i][j] > 100)  {   // opened node
                square.setFillColor(travColor);
                square.setPosition(gap*j + (float)gap/2, gap*i + (float)gap/2);
                win.draw(square);
                draw_texts(win, i, j);
            }
        }
    }
    square.setFillColor(pathColor);
    square.setPosition(gap*srcY + (float)gap/2, gap*srcX + (float)gap/2);
    win.draw(square);

    // draw lines
    lineRec.setPosition(0, 0);
    lineRec.setSize(Vector2f(dimX, 2));
    for (int i=0; i<dimY; i+=gap)   {
        lineRec.move(0, gap);
        win.draw(lineRec);
    }
    lineRec.setSize(Vector2f(2, dimY));
    lineRec.setPosition(0, 0);
    for (int j=0; j<dimX; j+=gap)   {
        lineRec.move(gap, 0);
        win.draw(lineRec);
    }
    Event event;
    while (win.pollEvent(event))
            if (event.type == Event::Closed)
                win.close();
    win.display();
    sleep(milliseconds(15));
}
