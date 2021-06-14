#include<bits/stdc++.h>
#include<SFML/Graphics.hpp>
using namespace sf;

const int dimX = 1000, 
          dimY = 800,
          gap = 25, mx = 1000,
          gridSizeY = dimX/gap, gridSizeX = dimY/gap;
int adjdist = 100, diadist = 141;

int srcX=-1, srcY=-1, dstX=-1, dstY=-1;
int animCount = 30;
int grid[gridSizeX][gridSizeY];

void draw();
void plot(Vector2i &pos);
void initialise();
bool find_path();
void display_grid();
int dist(int a, int b);
bool out_of_bounds(int x, int y);

RenderWindow win(VideoMode(dimX, dimY), "A* Pathfinder");
int main()  {
    initialise();
    bool drag=0, done=0, calc=0;
    while (win.isOpen())    {
        Event event;
        while (win.pollEvent(event))    {
            if (event.type == Event::Closed)
                win.close();
            else if (done);
            else if (dstX != -1 && event.type == Event::KeyPressed) {
                if (Keyboard::isKeyPressed(Keyboard::Enter))
                    done = 1;
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
        if (drag && !done)   {
            Vector2i pos = Mouse::getPosition(win);
            plot(pos);
        }
        draw();
        if (done && !calc)  {
            if (find_path())    {
                std::cout << "Path found!\n";
                display_grid();
            }
            else
                std::cout << "Path not found.\n";
            calc = 1;
        }
    }

    return 0;
}

void display_grid() {
    // displays the grid state
    std::cout << std::endl;
    for (int i=0; i<gridSizeX; ++i) {
        for (int j=0; j<gridSizeY; ++j)
            // if (grid[i][j]==100+animCount)
            //     std::cout << "* ";
            if (grid[i][j]==200+animCount)
                std::cout << "# ";
            else if (grid[i][j]==300+animCount)
                std::cout << "@ ";
            // else if (grid[i][j]==400+animCount)
            //     std::cout << "' ";
            else
                std::cout << "  ";
        std::cout << std::endl;
    }
}

int dist(int a, int b)  {
    if (abs(a) == abs(b))
        return diadist;
    return adjdist;
}

bool out_of_bounds(int x, int y)    {
    return x<0 || y<0 || x>=gridSizeX || y>=gridSizeY;
}

bool blocked(int nx, int ny, int x, int y)  {
    if (abs(nx) != abs(ny))
        return 0;
    return grid[x][y+ny] && grid[x+nx][y];
}

bool find_path()   {
    int gcost[gridSizeX][gridSizeY],
        hcost[gridSizeX][gridSizeY],
        fcost[gridSizeX][gridSizeY],
        parent[gridSizeX][gridSizeY];

    int x, y;

    for (int i=0; i<gridSizeX; ++i) {
        for (int j=0; j<gridSizeY; ++j) {
            gcost[i][j] = INT_MAX;
            fcost[i][j] = INT_MAX;
            hcost[i][j] = hcost[i][j] = (abs(dstX-i) + abs(dstY-j)) * adjdist;    // (std::max(abs(dstX-i), abs(dstY-j)) - std::min(abs(dstX-i), abs(dstY-j))) * adjdist + std::min(abs(dstX-i), abs(dstY-j)) * diadist;    // 
            parent[i][j] = -1;
        }
    }

    gcost[srcX][srcY] = 0;
    fcost[srcX][srcY] = gcost[srcX][srcY] + hcost[srcX][srcY];
    std::set<std::tuple<int, int, int, int>> mp;
    mp.insert({fcost[srcX][srcY], hcost[srcX][srcY], srcX, srcY});

    std::vector<std::pair<int, int>> vis = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};     // for adjacent neighbors only
    // std::vector<std::pair<int, int>> vis = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, {-1, -1}, {1, -1}, {1, 1}, {-1, 1}};    // for adjacent and diagonal neighbors
    
    while (!mp.empty() && !(x == dstX && y == dstY)) {
        draw();
        auto p = *mp.begin();
        mp.erase(mp.begin());
        x = std::get<2>(p), y = std::get<3>(p);
        if (!(x==srcX && y==srcY))
            grid[x][y] = 101;
        for (auto &v : vis)    {
            int nbx = v.first+x, nby = v.second+y;
            if (grid[nbx][nby] && !(nbx==dstX && nby==dstY) || out_of_bounds(nbx, nby) || blocked(v.first, v.second, x, y))
                continue;
            
            grid[nbx][nby] = 401;
            auto it = mp.find({fcost[nbx][nby], hcost[nbx][nby], nbx, nby});
            if (gcost[nbx][nby] > gcost[x][y] + dist(v.first, v.second)) {
                gcost[nbx][nby] = gcost[x][y] + dist(v.first, v.second);
                parent[nbx][nby] = x*mx + y;
            }
            fcost[nbx][nby] = gcost[nbx][nby] + hcost[nbx][nby];

            if (it != mp.end())
                mp.erase(it);
            mp.insert({fcost[nbx][nby], hcost[nbx][nby], nbx, nby});
        }
    }
    grid[srcX][srcY] = 301;
    if (!(x==dstX && y==dstY))
        return 0;
    while (!(x==srcX && y==srcY))   {
        int p = parent[x][y];
        grid[x][y] = 301;
        x = p / mx;
        y = p % mx;
        draw();
    }
    return 1;
}

void plot(Vector2i &pos)    {
    int px = std::min(dimX, pos.x)/gap, py = std::min(dimY, pos.y)/gap;
    px = std::max(0, pos.x)/gap, py = std::max(0, pos.y)/gap;
    if (srcX == -1) {
        srcY = px;
        srcX = py;
        grid[srcX][srcY] = 301;
        return;
    }
    if (dstX == -1) {
        dstX = py;
        dstY = px;
        grid[dstX][dstY] = 301;
        return;
    }
    if ( (py==srcX && px==srcY) || (py==dstX && px==dstY) )
        return;
    grid[py][px] = 201;
}

auto lineColor = Color(100, 200, 148),
     pathColor = Color(255, 255, 255), 
     travColor = Color(255, 18, 128),
     unviColor = Color(18, 255, 18),
     wallColor = Color(128, 128, 128);

RectangleShape square(Vector2f(gap, gap));
RectangleShape lineRec(Vector2f(1, 2));
void initialise()   {
    lineRec.setFillColor(lineColor);
    square.setOrigin(float(gap)/2, float(gap)/2);
    memset(grid, 0, sizeof(grid));
}

void draw() {
    win.clear();
    for (int i=0; i<gridSizeX; ++i) {
        for (int j=0; j<gridSizeY; ++j) {
            if (!grid[i][j])
                continue;
            
            if (grid[i][j] % 100 < animCount)
                ++grid[i][j];
            float newscale = float(grid[i][j] % 100) / animCount;
            square.setScale(newscale, newscale);
            
            if (grid[i][j] > 400) {   // unvisited node (in set)
                square.setFillColor(unviColor);
                square.setPosition(gap*j + (float)gap/2, gap*i + (float)gap/2);
                win.draw(square);
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
            }
        }
    }
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
