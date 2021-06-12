#include<bits/stdc++.h>
// #include<SFML/Graphics.hpp>
using namespace std;

const int gridSizeX = 50, gridSizeY = 50;
bool grid[gridSizeX][gridSizeY];
int srcX, srcY, dstX, dstY;

void take_input();
void display_grid();
bool find_path();

int main()  {
    take_input();
    bool done = find_path();
    cout << (done?"Path found!\n":"Path not found.\n");
    display_grid();
    cin >> done;
    return 0;
}

bool find_path()   {
    // returns if a path exists or not
    int gcost[gridSizeX][gridSizeY], 
        hcost[gridSizeX][gridSizeY], 
        fcost[gridSizeX][gridSizeY];
    map<pair<int, int>, pair<int, int>> mp;     // (fcost, gcost) -> (x, y)

    return 1;
}

void display_grid() {
    // displays the grid state
    cout << endl;
    for (int i=0; i<gridSizeX; ++i) {
        for (int j=0; j<gridSizeY; ++j)
            if (grid[i][j])
                cout << "# ";
            else
                cout << ". ";
        cout << endl;
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
