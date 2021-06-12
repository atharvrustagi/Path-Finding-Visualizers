#include<bits/stdc++.h>
// #include<SFML/Graphics.hpp>
using namespace std;

const int gridSizeX = 20, gridSizeY = 50;
int grid[gridSizeX][gridSizeY];
int srcX, srcY, dstX, dstY;
int adjdist = 100, diadist = 141;

void take_input();
void add_walls();
void display_grid();
bool find_path();
bool out_of_bounds(int a, int b);

int main()  {
    take_input();
    add_walls();
    cout << "Finding shortest path...\n";
    bool done = find_path();
    cout << endl << (done?"Path found!\n":"Path not found.\n");
    display_grid();
    return 0;
}

/*
let adjacent nodes be at a distance 100 from each other
let diagonal nodes be at a distance 141 from each other (sqrt(2))

at each step, select lowest fcost
if tie b/w fcosts, choose lowest hcost
*/

int dist(int a, int b)  {
    if (abs(a) == abs(b))
        return diadist;
    return adjdist;
}

bool find_path()   {
    // returns if a path exists or not
    int 
    gcost[gridSizeX][gridSizeY],    // cost from source
    hcost[gridSizeX][gridSizeY],    // cost from destination
    fcost[gridSizeX][gridSizeY],    // gcost + hcost

    x = srcX, y = srcY;

    // initialising costs
    for (int i=0; i<gridSizeX; ++i) {
        for (int j=0; j<gridSizeY; ++j) {
            gcost[i][j] = INT_MAX;
            fcost[i][j] = INT_MAX;
            hcost[i][j] = (abs(dstX-i) + abs(dstY-j)) * adjdist;
        }
    }

    set<tuple<int, int, int, int>> mp;     // (fcost, hcost, x, y)
    
    // start from the source node
    gcost[x][y] = 0;      // 0 distance from source node
    fcost[x][y] = gcost[x][y] + hcost[x][y];

    mp.insert({fcost[x][y], hcost[x][y], x, y});
    vector<pair<int, int>> vis = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};
    
    while (!mp.empty() && !(x == dstX && y == dstY)) {
        auto p = *mp.begin();   // extracting new node to traverse
        mp.erase(mp.begin());
        x = get<2>(p), y = get<3>(p);    // new node coordinates
        grid[x][y] = 1;     // this node is now visited
        // printf("\nAt node: (%d, %d)", x, y);

        // for every neighboring node
        for (auto &v : vis)    {
            int nbx = v.first+x, nby = v.second+y;
            // cout << out_of_bounds(nbx, nby) << " ";
            if (grid[nbx][nby] || out_of_bounds(nbx, nby))     // if this node is already visited or is blocked or out of bounds
                continue;
            // check if the neighboring node is already in the map
            auto it = mp.find({fcost[nbx][nby], hcost[nbx][nby], nbx, nby});
            // cout << "searched ";

            // update fcost, gcost, hcost
            /*
            new gcost:
                gcost[nbX][nbY] = gcost[x][y] + adjdist
            hcost remains the same
            */
            gcost[nbx][nby] = min(gcost[nbx][nby],  gcost[x][y] + adjdist);
            fcost[nbx][nby] = gcost[nbx][nby] + hcost[nbx][nby];
            // cout << "updated ";

            if (it != mp.end())     // if node is in the map
                mp.erase(it);       // delete the node
            mp.insert({fcost[nbx][nby], hcost[nbx][nby], nbx, nby});    // insert updated neighbor's cost
            // cout << "erased and inserted ";
        }
    
    }
    return (x==dstX && y==dstY);
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
