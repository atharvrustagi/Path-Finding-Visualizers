#include<bits/stdc++.h>
using namespace std;

const int gridSizeX = 20, gridSizeY = 50, mx=1000;
char grid[gridSizeX][gridSizeY];
int srcX, srcY, dstX, dstY;
int adjdist = 100, diadist = 141;

void take_input();
void add_walls();
void display_grid();
bool find_path();
bool out_of_bounds(int a, int b);
int dist(int a, int b);

int main()  {
    take_input();
    add_walls();
    cout << "\nFinding shortest path...";
    bool done = find_path();
    cout << endl << (done?"Path found!\n":"Path not found.\n");
    display_grid();
    return 0;
}

int dist(int a, int b)  {
    if (abs(a) == abs(b))
        return diadist;
    return adjdist;
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

    gcost[srcX][srcY] = 0;
    fcost[srcX][srcY] = gcost[srcX][srcY] + hcost[srcX][srcY];
    set<tuple<int, int, int, int>> mp;
    mp.insert({fcost[srcX][srcY], hcost[srcX][srcY], srcX, srcY});

    vector<pair<int, int>> vis = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};     // for adjacent neighbors only
    // vector<pair<int, int>> vis = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}, {-1, -1}, {1, -1}, {1, 1}, {-1, 1}};    // for adjacent and diagonal neighbors
    
    while (!mp.empty() && !(x == dstX && y == dstY)) {
        auto p = *mp.begin();
        mp.erase(mp.begin());
        x = get<2>(p), y = get<3>(p);
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
    if (!(x==dstX && y==dstY))
        return 0;
    while (!(x==srcX && y==srcY))   {
        int p = parent[x][y];
        grid[x][y] = 3;
        x = p / mx;
        y = p % mx;
    }
    grid[srcX][srcY] = 3;
    return 1;
}

bool out_of_bounds(int x, int y)    {
    return x<0 || y<0 || x>=gridSizeX || y>=gridSizeY;
}

void display_grid() {
    // displays the grid state
    cout << endl;
    for (int i=0; i<gridSizeX; ++i) {
        for (int j=0; j<gridSizeY; ++j)
            if (grid[i][j]==1)
                cout << "* ";
            else if (grid[i][j]==2)
                cout << "# ";
            else if (grid[i][j]==3)
                cout << "@ ";
            else
                cout << ". ";
        cout << endl;
    }
}

void add_walls()    {
    int n, x1, x2, y1, y2, i=0;
    cout << "Enter the number of walls: ";
    cin >> n;
    while (i++ < n) {
        printf("Walls #%d (x1, y1, x2, y2) : ", i);
        cin >> x1 >> y1 >> x2 >> y2;
        for (int x=x1; x<=x2; ++x)
            for (int y=y1; y<=y2; ++y)
                grid[x][y] = 2;
    }
}

void take_input()   {
    // takes input for source, destination
    memset(grid, 0, sizeof(grid));
    printf("Enter starting/source point (x <space> y): ");
    cin >> srcX >> srcY;
    printf("Enter ending/destination point (x <space> y): ");
    cin >> dstX >> dstY;
}
