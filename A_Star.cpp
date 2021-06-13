#include<bits/stdc++.h>
#include<SFML/Graphics.hpp>
using namespace sf;
// using namespace std;

const int dimX = 1000, 
          dimY = 800,
          gap = 25, mx = 1000,
          gridSizeY = dimX/gap, gridSizeX = dimY/gap;
int adjdist = 100, diadist = 141;

int srcX=-1, srcY=-1, dstX=-1, dstY=-1;

int grid[gridSizeX][gridSizeY];

void draw(bool delay=0);
void plot(Vector2i &pos);
void initialise();
bool find_path();
bool find_path2();
int dist(int a, int b);
bool out_of_bounds(int x, int y);

void display_grid() {
    // displays the grid state
    std::cout << std::endl;
    for (int i=0; i<gridSizeX; ++i) {
        for (int j=0; j<gridSizeY; ++j)
            if (grid[i][j]==1)
                std::cout << "* ";
            else if (grid[i][j]==2)
                std::cout << "# ";
            else if (grid[i][j]==3)
                std::cout << "@ ";
            else
                std::cout << ". ";
        std::cout << std::endl;
    }
}

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
                // std::cout << drag;
                drag = !drag;
            }
            else if (event.type == Event::MouseButtonReleased)
                if (event.mouseButton.button==Mouse::Left)  {
                    Vector2i pos = Mouse::getPosition(win);
                    plot(pos);
                    // printf("srcx: %d, srcy: %d, dstx: %d, dsty: %d \n", srcX, srcY, dstX, dstY);
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

int dist(int a, int b)  {
    if (abs(a) == abs(b))
        return diadist;
    return adjdist;
}

bool out_of_bounds(int x, int y)    {
    return x<0 || y<0 || x>=gridSizeX || y>=gridSizeY;
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
            hcost[i][j] = (abs(dstX-i) + abs(dstY-j)) * adjdist;
            parent[i][j] = -1;
        }
    }
    grid[dstX][dstY] = 0;   // not visited yet
    gcost[srcX][srcY] = 0;
    fcost[srcX][srcY] = gcost[srcX][srcY] + hcost[srcX][srcY];
    std::set<std::tuple<int, int, int, int>> mp;
    mp.insert({fcost[srcX][srcY], hcost[srcX][srcY], srcX, srcY});

    std::vector<std::pair<int, int>> vis = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};     // for adjacent neighbors only
    // vector<pair<int, int>> vis = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, {-1, -1}, {1, -1}, {1, 1}, {-1, 1}};    // for adjacent and diagonal neighbors
    
    while (!mp.empty() && !(x == dstX && y == dstY)) {
        draw(1);
        auto p = *mp.begin();
        mp.erase(mp.begin());
        x = std::get<2>(p), y = std::get<3>(p);
        grid[x][y] = 1;
        for (auto &v : vis)    {
            int nbx = v.first+x, nby = v.second+y;
            if (grid[nbx][nby] || out_of_bounds(nbx, nby))
                continue;
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
    grid[srcX][srcY] = 3;
    if (!(x==dstX && y==dstY))
        return 0;
    while (!(x==srcX && y==srcY))   {
        int p = parent[x][y];
        grid[x][y] = 3;
        x = p / mx;
        y = p % mx;
    }
    return 1;
}

void plot(Vector2i &pos)    {
    int px = std::min(dimX, pos.x)/gap, py = std::min(dimY, pos.y)/gap;
    px = std::max(0, pos.x)/gap, py = std::max(0, pos.y)/gap;
    if (srcX == -1) {
        srcY = px;
        srcX = py;
        grid[srcX][srcY] = 3;
        return;
    }
    if (dstX == -1) {
        dstX = py;
        dstY = px;
        grid[dstX][dstY] = 3;
        return;
    }
    if ( (py==srcX && px==srcY) || (py==dstX && px==dstY) )
        return;
    grid[py][px] = 2;
}

auto lineColor = Color(100, 200, 148),
     pathColor = Color(255, 255, 255), 
     travColor = Color(255, 108, 128),
     wallColor = Color(128, 128, 128);

RectangleShape square(Vector2f(gap, gap));
RectangleShape lineRec(Vector2f(1, 2));
void initialise()   {
    lineRec.setFillColor(lineColor);
    memset(grid, 0, sizeof(grid));
}

void draw(bool delay) {
    win.clear();
    for (int i=0; i<gridSizeX; ++i) {
        for (int j=0; j<gridSizeY; ++j) {
            if (grid[i][j]==3 || (i==srcX && j==srcY) || (i==dstX && j==dstY)) {   // path node
                square.setFillColor(pathColor);
                square.setPosition(gap*j, gap*i);
                win.draw(square);
            }
            else if (grid[i][j]==1)  {   // opened node
                square.setFillColor(travColor);
                square.setPosition(gap*j, gap*i);
                win.draw(square);
            }
            else if (grid[i][j]==2)  {  // wall node
                square.setFillColor(wallColor);
                square.setPosition(gap*j, gap*i);
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
    win.display();
    if (delay)
        sleep(milliseconds(30));
}
