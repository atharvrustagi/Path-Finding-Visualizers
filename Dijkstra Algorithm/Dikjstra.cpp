#include<bits/stdc++.h>
#include<SFML/Graphics.hpp>
using namespace sf;

int dimX = 1000, 
    dimY = 800,
    gap = 50, mx = 1000,
    gridSizeY = dimX/gap, gridSizeX = dimY/gap;
bool DIAGONAL = 0;
int adjdist = 100, diadist = 141;
float arrowSize = 100;
int srcX=-1, srcY=-1, dstX=-1, dstY=-1;
int animCount = 30, frameDelay = 15;    // miliseconds
int grid[500][500];
std::vector<std::pair<int, short>> path;  // final path
/* the 2nd value of the pair can have 8 values, which are the directions from where it came from
0 - top left
1 - top
2 - top right
3 - left
5 - right
6 - bottom left
7 - bottom
8 - right
*/

void draw(RenderWindow &win);
void plot(Vector2i &pos);
void initialise();
int dijkstra(RenderWindow &win);
void display_grid();
bool take_input();
int dist(int a, int b);
bool out_of_bounds(int x, int y);

int main()  {
    main_loop:
    if (take_input())   {
        dimY = gridSizeX * gap, dimX = gridSizeY * gap;
    }
    initialise();
    RenderWindow win(VideoMode(dimX, dimY), "A* Pathfinder");
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
        draw(win);
        if (done && !calc)  {
            int g_cost = dijkstra(win);
            if (g_cost != -1)    {
                std::cout << "Path found!\n";
                display_grid();
                printf("Cost of path: %d\n", g_cost);
            }
            else
                std::cout << "Path not found.\n";
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
     arrowColor = Color(18, 255, 18);

RectangleShape square;
RectangleShape lineRec(Vector2f(1, 2));
CircleShape src((float)gap/3, 20);
RectangleShape dst;
ConvexShape arrow(6);

bool take_input()   {
    char c;
    printf("Do you want the window to open with default dimensions and values? (y/n): ");
    std::cin >> c;
    if (c!='n')
        return 0;
    printf("Grid Dimensions: (X <space> Y): ");
    std::cin >> gridSizeY >> gridSizeX;
    printf("Size of Each Square (in pixels): ");
    std::cin >> gap;
    printf("Travel to cells diagonally? (y/n): ");
    std::cin >> c;
    DIAGONAL = (c=='y');
    return 1;
}

void initialise()   {
    path.clear();
    srcX = srcY = dstX = dstY = -1;
    memset(grid, 0, sizeof(grid));
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
    // source and destination marking
    if (srcX != -1)
        win.draw(src);
    if (dstX != -1)
        win.draw(dst);
    
    // arrows
    for (auto &p:path)  {
        arrow.setRotation(p.second);
        arrow.setPosition((p.first%mx)*gap + (float)gap/2, (p.first/mx)*gap + (float)gap/2);
        win.draw(arrow);
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
    sleep(milliseconds(frameDelay));
}

void display_grid() {
    // displays the grid state
    std::cout << std::endl;
    for (int i=0; i<gridSizeX; ++i) {
        for (int j=0; j<gridSizeY; ++j) {
            if (grid[i][j]>=400)
                std::cout << "  ";
            else if (grid[i][j]>=300)
                std::cout << "@ ";
            else if (grid[i][j]>=200)
                std::cout << "# ";
            // else if (grid[i][j]==100+animCount)
            //     std::cout << "* ";
            else
                std::cout << "  ";
        }
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
    return (grid[x][y+ny] > 200 && grid[x][y+ny] < 300) && (grid[x+nx][y] > 200 && grid[x+nx][y] < 300);
}

void plot(Vector2i &pos)    {
    int px = std::min(dimX, pos.x)/gap, py = std::min(dimY, pos.y)/gap;
    px = std::max(0, pos.x)/gap, py = std::max(0, pos.y)/gap;
    if (srcX == -1) {
        srcY = px;
        srcX = py;
        grid[srcX][srcY] = 301;
        src.setPosition(srcY * gap + (float)gap/2, srcX * gap + (float)gap/2);
        return;
    }
    if (dstX == -1) {
        dstX = py;
        dstY = px;
        grid[dstX][dstY] = 301;
        dst.setPosition(dstY * gap + (float)gap/2, dstX * gap + (float)gap/2);
        return;
    }
    if ( (py==srcX && px==srcY) || (py==dstX && px==dstY) )
        return;
    grid[py][px] = 201;
}

int dijkstra(RenderWindow &win)   {
    int parent[gridSizeX][gridSizeY],
        gcost [gridSizeX][gridSizeY];
    int x, y;

    memset(parent, -1, sizeof(parent));
    for (int i=0; i<gridSizeX; ++i)
        for (int j=0; j<gridSizeY; ++j)
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
            grid[x][y] = 101;

        for (auto &v : vis) {
            int nbx = v.first+x, nby = v.second+y;
            if (out_of_bounds(nbx, nby) || grid[nbx][nby] && (grid[nbx][nby]<400 || grid[nbx][nby]>500) && !(nbx==dstX && nby==dstY) || blocked(v.first, v.second, x, y))
                continue;
            
            grid[nbx][nby] = 401;
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
    grid[srcX][srcY] = 301;
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
        grid[temp_path[i].first / mx][temp_path[i].first % mx] = 301;
        draw(win);
    }
    grid[dstX][dstY] = 301;
    for (auto it = temp_path.rbegin(); it != temp_path.rend(); ++it)    {
        path.push_back(*it);
        draw(win);
    }
    int p = parent[dstX][dstY];
    int g_cost = gcost[p/mx][p%mx] + dist(dstX-p/mx, dstY-p%mx);
    return g_cost;
}
