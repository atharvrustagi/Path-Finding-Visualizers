#include<bits/stdc++.h>
#include<SFML/Graphics.hpp>
using namespace sf;

int dimX = 1850,
    dimY = 900,
    gap = 50,
    gridSize = dimY / gap;  // square grid
bool DIAGONAL = 0;
int adjdist = 100, diadist = 141, mx = 1000;
int srcX=-1, srcY=-1, dstX=-1, dstY=-1;
int grid1[500][500], grid2[500][500];
int animCount = 60, frameDelay = 5;    // miliseconds
int gridIntensity = 40;
float arrowSize = 100;

std::vector<std::pair<int, short>> path1, path2;

void draw(RenderWindow &win);
void plot(Vector2i &pos);
void initialise();
void rand_maze(int intensity=30);
bool take_input();
int dijkstra(RenderWindow &win);
int a_star(RenderWindow &win);
int dist(int a, int b);
bool out_of_bounds(int x, int y);

int main()  {
    main_loop:
    take_input();
    initialise();
    RenderWindow win(VideoMode(dimX, dimY), "A_Star vs Dijkstra Pathfinder");
    bool drag=0, done=0, calc=0;
    while (win.isOpen())    {
        Event event;
        while (win.pollEvent(event))    {
            if (event.type == Event::Closed)
                win.close();
            else if (calc && event.type == Event::KeyPressed)
                goto main_loop;
            else if (done);
            else if (dstX != -1 && event.type == Event::KeyPressed) {
                if (Keyboard::isKeyPressed(Keyboard::Enter))    {
                    rand_maze(gridIntensity);
                    done = 1;
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
        if (drag && !done)   {
            Vector2i pos = Mouse::getPosition(win);
            plot(pos);
        }
        draw(win);
        if (done && !calc)  {
            int a_star_cost = a_star(win);
            int dijkstra_cost = dijkstra(win);
            if (a_star_cost == -1)  {
                printf("Path not found\n");
            }
            else    {
                printf("Path found.\n");
                printf("A_Star cost of Path: %d\n", a_star_cost);
                printf("Dijkstra cost of Path: %d\n", dijkstra_cost);
            }
            calc = 1;
            printf("Press any key to start again\n");
        }
    }
    return 0;
}

auto lineColor = Color(10, 20, 14),
     pathColor = Color(255, 255, 255), 
     travColor = Color(255, 18, 128),
     unviColor = Color(18, 18, 200),
     wallColor = Color(128, 128, 128), 
     srcColor = Color(0, 0, 0), 
     dstColor = Color(0, 0, 0), 
     arrowColor = Color(18, 255, 18),
     dividerColor = Color(255, 100, 0);

RectangleShape square;
RectangleShape lineRec(Vector2f(1, 2));
RectangleShape divider(Vector2f(dimX-2*dimY, dimY));
CircleShape src((float)gap/3, 20);
RectangleShape dst;
ConvexShape arrow(6);

bool take_input()   {
    char c;
    printf("Do you want the window to open with default dimensions and values? (y/n): ");
    std::cin >> c;
    if (c!='n')
        return 0;
    printf("Size of Each Square in pixels (factor of 900): ");
    std::cin >> gap;
    printf("Travel to cells diagonally? (y/n): ");
    std::cin >> c;
    DIAGONAL = (c=='y');
    printf("Grid Intensity: (out of 100): ");
    std::cin >> gridIntensity;
    return 1;
}

void rand_maze(int intensity)    {  // intensity (out of 100)
    int n = 0, nmax = gridSize * gridSize * intensity / 100;
    while (n < nmax)    {
        int x = rand()%gridSize, y = rand()%gridSize;
        if (grid1[x][y])
            continue;
        grid1[x][y] = 201;
        grid2[x][y] = 201;
        ++n;
    }
}

void plot(Vector2i &pos)    {
    if (pos.x > dimY)
        return;
    int px = std::min(dimY, pos.x)/gap, py = std::min(dimY, pos.y)/gap;
    px = std::max(0, pos.x)/gap, py = std::max(0, pos.y)/gap;
    if (srcX == -1) {
        srcY = px;
        srcX = py;
        grid1[srcX][srcY] = 301;
        grid2[srcX][srcY] = 301;
        src.setPosition(srcY * gap + 0.5f * gap, srcX * gap + 0.5f * gap);
        return;
    }
    if (dstX == -1) {
        dstX = py;
        dstY = px;
        grid1[dstX][dstY] = 301;
        grid2[dstX][dstY] = 301;
        dst.setPosition(dstY * gap + 0.5f * gap, dstX * gap + 0.5f * gap);
        return;
    }
    if ( (py==srcX && px==srcY) || (py==dstX && px==dstY) )
        return;
    grid1[py][px] = 201;
    grid2[py][px] = 201;
}

void initialise()   {
    gridSize = dimY / gap;
    divider.setPosition(dimY, 0);
    divider.setFillColor(dividerColor);
    divider.setOutlineColor(Color::White);
    divider.setOutlineThickness(4);
    path1.clear();
    path2.clear();
    srcX = srcY = dstX = dstY = -1;
    memset(grid1, 0, sizeof(grid1));
    memset(grid2, 0, sizeof(grid2));
    lineRec.setFillColor(lineColor);
    square.setSize(Vector2f((float)gap, (float)gap));
    square.setOrigin(float(gap)/2, float(gap)/2);
    src.setRadius((float)gap/3);
    src.setOrigin(src.getRadius(), src.getRadius());
    src.setFillColor(srcColor);
    dst.setSize((Vector2f(2*(float)gap/3, 2*(float)gap/3)));
    dst.setOrigin(dst.getSize().x/2, dst.getSize().y/2);
    dst.setFillColor(dstColor);
    // arrow
    arrow.setPoint(0, Vector2f(0, 0));
    arrow.setPoint(1, Vector2f(arrowSize, 0));
    arrow.setPoint(2, Vector2f(2*arrowSize, 2*arrowSize));
    arrow.setPoint(3, Vector2f(arrowSize, 4*arrowSize));
    arrow.setPoint(4, Vector2f(0, 4*arrowSize));
    arrow.setPoint(5, Vector2f(arrowSize, 2*arrowSize));
    arrow.setFillColor(arrowColor);
    arrow.setScale(0.65*gap/arrowSize/4, 0.55*gap/arrowSize/4);
    arrow.setOrigin(arrowSize, 2*arrowSize);
}

void draw(RenderWindow &win) {
    win.clear();
    for (int i=0; i<gridSize; ++i) {
        for (int j=0; j<gridSize; ++j) {
            if (grid1[i][j]) {  // for grid 1
                if (grid1[i][j] % 100 < animCount)
                    ++grid1[i][j];
                float newscale = 1.f * (grid1[i][j] % 100) / animCount;
                square.setScale(newscale, newscale);

                if (grid1[i][j] > 400) {   // unvisited node (in set)
                    square.setFillColor(unviColor);
                    square.setPosition(gap*j + 0.5f * gap, gap*i + 0.5f * gap);
                }
                else if (grid1[i][j] > 300) {   // path node
                    square.setFillColor(pathColor);
                    square.setPosition(gap*j + 0.5f * gap, gap*i + 0.5f * gap);
                }
                else if (grid1[i][j] > 200)  {  // wall node
                    square.setFillColor(wallColor);
                    square.setPosition(gap*j + 0.5f * gap, gap*i + 0.5f * gap);
                }
                else if (grid1[i][j] > 100)  {   // opened node
                    square.setFillColor(travColor);
                    square.setPosition(gap*j + 0.5f * gap, gap*i + 0.5f * gap);
                }
                win.draw(square);
            }

            if (grid2[i][j]) {  // for grid 2
                if (grid2[i][j] % 100 < animCount)
                    ++grid2[i][j];
                float newscale = 1.f * (grid2[i][j] % 100) / animCount;
                square.setScale(newscale, newscale);

                if (grid2[i][j] > 400) {   // unvisited node (in set)
                    square.setFillColor(unviColor);
                    square.setPosition(dimX - dimY + gap*j + 0.5f * gap, gap*i + 0.5f * gap);
                }
                else if (grid2[i][j] > 300) {   // path node
                    square.setFillColor(pathColor);
                    square.setPosition(dimX - dimY + gap*j + 0.5f * gap, gap*i + 0.5f * gap);
                }
                else if (grid2[i][j] > 200)  {  // wall node
                    square.setFillColor(wallColor);
                    square.setPosition(dimX - dimY + gap*j + 0.5f * gap, gap*i + 0.5f * gap);
                }
                else if (grid2[i][j] > 100)  {   // opened node
                    square.setFillColor(travColor);
                    square.setPosition(dimX - dimY + gap*j + 0.5f * gap, gap*i + 0.5f * gap);
                }
                win.draw(square);
            }
        }
    }
    // source and destination marking
    if (srcX != -1)
        win.draw(src);
    if (dstX != -1)
        win.draw(dst);
    
    // arrows
    for (auto &p: path1)  {
        arrow.setRotation(p.second);
        arrow.setPosition((p.first%mx)*gap + (float)gap/2, (p.first/mx)*gap + (float)gap/2);
        win.draw(arrow);
    }

    for (auto &p: path2)  {
        arrow.setRotation(p.second);
        arrow.setPosition(dimX-dimY+(p.first%mx)*gap + (float)gap/2, (p.first/mx)*gap + (float)gap/2);
        win.draw(arrow);
    }

    // draw lines
    lineRec.setPosition(0, 0);
    lineRec.setSize(Vector2f(dimY, 2));
    for (int i=0; i<dimY; i+=gap)   {
        lineRec.move(0, gap);
        win.draw(lineRec);
        lineRec.move(dimX-dimY, 0);
        win.draw(lineRec);
        lineRec.move(-dimX+dimY, 0);
    }
    lineRec.setSize(Vector2f(2, dimY));
    lineRec.setPosition(0, 0);
    for (int j=0; j<=dimY; j+=gap)   {
        win.draw(lineRec);
        lineRec.move(dimX-dimY, 0);
        win.draw(lineRec);
        lineRec.move(-dimX+dimY, 0);
        lineRec.move(gap, 0);
    }
    // divider
    win.draw(divider);

    Event event;
    while (win.pollEvent(event))
            if (event.type == Event::Closed)
                win.close();
    win.display();
    sleep(milliseconds(frameDelay));
}

int dist(int a, int b)  {
    if (abs(a) == abs(b))
        return diadist;
    return adjdist;
}

bool out_of_bounds(int x, int y)    {
    return x<0 || y<0 || x>=gridSize || y>=gridSize;
}

bool blocked1(int nx, int ny, int x, int y)  {
    if (abs(nx) != abs(ny))
        return 0;
    return (grid1[x][y+ny] > 200 && grid1[x][y+ny] < 300) && (grid1[x+nx][y] > 200 && grid1[x+nx][y] < 300);
}

bool blocked2(int nx, int ny, int x, int y)  {
    if (abs(nx) != abs(ny))
        return 0;
    return (grid2[x][y+ny] > 200 && grid2[x][y+ny] < 300) && (grid2[x+nx][y] > 200 && grid2[x+nx][y] < 300);
}

int a_star(RenderWindow &win)   {
    int gcost[gridSize][gridSize],
        hcost[gridSize][gridSize],
        fcost[gridSize][gridSize],
        parent[gridSize][gridSize];

    int x, y;
    memset(parent, -1, sizeof(parent));
    for (int i=0; i<gridSize; ++i) {
        for (int j=0; j<gridSize; ++j) {
            gcost[i][j] = INT_MAX;
            fcost[i][j] = INT_MAX;
            hcost[i][j] = DIAGONAL ? (std::max(abs(dstX-i), abs(dstY-j)) - std::min(abs(dstX-i), abs(dstY-j))) * adjdist + std::min(abs(dstX-i), abs(dstY-j)) * diadist : (abs(dstX-i) + abs(dstY-j)) * adjdist;
        }
    }

    gcost[srcX][srcY] = 0;
    fcost[srcX][srcY] = gcost[srcX][srcY] + hcost[srcX][srcY];
    std::set<std::tuple<int, int, int, int>> mp;
    mp.insert({fcost[srcX][srcY], hcost[srcX][srcY], srcX, srcY});

    std::vector<std::pair<int, int>> vis;
    if (!DIAGONAL)
        vis = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};     // for adjacent neighbors only
    else
        vis = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, {-1, -1}, {1, -1}, {1, 1}, {-1, 1}};    // for adjacent and diagonal neighbors
    
    while (!mp.empty() && !(x == dstX && y == dstY)) {
        auto p = *mp.begin();
        mp.erase(mp.begin());
        x = std::get<2>(p), y = std::get<3>(p);
        if (!(x==srcX && y==srcY))
            grid1[x][y] = 101;
        for (auto &v : vis)    {
            int nbx = v.first+x, nby = v.second+y;
            if (out_of_bounds(nbx, nby) || grid1[nbx][nby] && (grid1[nbx][nby]<400 || grid1[nbx][nby]>500) && !(nbx==dstX && nby==dstY) || blocked1(v.first, v.second, x, y))
                continue;
            
            draw(win);
            
            grid1[nbx][nby] = 401;
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
    grid1[srcX][srcY] = 301;
    if (!(x==dstX && y==dstY))
        return -1;
    // rotation angle map for arrow
    int map[9] = {-135, -90, -45, 180, INT_MAX, 0, 135, 90, 45};
    std::vector<std::pair<int, short>> temp_path;
    while (!(x==srcX && y==srcY))   {
        int p = parent[x][y];   // parent of x, y is p
        short direction = (x-p/mx+1) * 3 + y-p%mx+1;
        temp_path.push_back({p, map[direction]});
        x = p / mx;
        y = p % mx;
    }
    for (int i=temp_path.size()-1; i >= 0; --i)  {
        grid1[temp_path[i].first / mx][temp_path[i].first % mx] = 301;
        draw(win);
    }
    grid1[dstX][dstY] = 301;
    for (auto it = temp_path.rbegin(); it != temp_path.rend(); ++it)    {
        path1.push_back(*it);
        draw(win);
    }
    int p = parent[dstX][dstY];
    int g_cost = gcost[p/mx][p%mx] + dist(dstX-p/mx, dstY-p%mx);
    return g_cost;
}

int dijkstra(RenderWindow &win)   {
    int parent[gridSize][gridSize],
        gcost [gridSize][gridSize];
    int x, y;

    memset(parent, -1, sizeof(parent));
    for (int i=0; i<gridSize; ++i)
        for (int j=0; j<gridSize; ++j)
            gcost[i][j] = INT_MAX;
    gcost[srcX][srcY] = 0;
    
    std::set<std::pair<int, int>> s;    // (gcost, point->(x*mx + y))
    s.insert({0, srcX * mx + srcY});
    
    std::vector<std::pair<int, int>> vis;
    if (!DIAGONAL)
        vis = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};     // for adjacent neighbors only
    else
        vis = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, {-1, -1}, {1, -1}, {1, 1}, {-1, 1}};    // for adjacent and diagonal neighbors
    
    while (!s.empty() && !(x==dstX && y==dstY))  {
        auto p = *s.begin();
        s.erase(s.begin());
        x = p.second/mx, y = p.second%mx;
        if (!(x==srcX && y==srcY))
            grid2[x][y] = 101;

        for (auto &v : vis) {
            int nbx = v.first+x, nby = v.second+y;
            if (out_of_bounds(nbx, nby) || grid2[nbx][nby] && (grid2[nbx][nby]<400 || grid2[nbx][nby]>500) && !(nbx==dstX && nby==dstY) || blocked2(v.first, v.second, x, y))
                continue;
            
            grid2[nbx][nby] = 401;
            draw(win);
            auto it = s.find({gcost[nbx][nby], nbx*mx+nby});
            if (gcost[nbx][nby] > gcost[x][y] + dist(v.first, v.second))    {
                gcost[nbx][nby] = gcost[x][y] + dist(v.first, v.second);
                parent[nbx][nby] = x*mx + y;
            }

            if (it != s.end())
                s.erase(it);
            s.insert({gcost[nbx][nby], nbx*mx+nby});
        }
    }
    grid2[srcX][srcY] = 301;
    if (!(x==dstX && y==dstY))
        return -1;
    int map[9] = {-135, -90, -45, 180, INT_MAX, 0, 135, 90, 45};
    std::vector<std::pair<int, short>> temp_path;
    while (!(x==srcX && y==srcY))   {
        int p = parent[x][y];   // parent of x, y is p
        short direction = (x-p/mx+1) * 3 + y-p%mx+1;
        temp_path.push_back({p, map[direction]});
        x = p / mx;
        y = p % mx;
    }

    for (int i=temp_path.size()-1; i >= 0; --i)  {
        grid2[temp_path[i].first / mx][temp_path[i].first % mx] = 301;
        draw(win);
    }
    grid2[dstX][dstY] = 301;
    for (auto it = temp_path.rbegin(); it != temp_path.rend(); ++it)    {
        path2.push_back(*it);
        draw(win);
    }
    int p = parent[dstX][dstY];
    int g_cost = gcost[p/mx][p%mx] + dist(dstX-p/mx, dstY-p%mx);
    return g_cost;
}
