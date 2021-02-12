#include <bits/stdc++.h>
#include <windows.h>
#include <conio.h>
#include <math.h>

using namespace std;

HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
DWORD dwWritten;
HWND myconsole = GetConsoleWindow();
HDC dc = GetDC(myconsole);

int XSIZE = 195;
int YSIZE = 49;
int SCALE = 1;

int NUM_POINTS = 20;
int INITIAL_POP = 25;
int MUTATION_RATE = 3;

void fillxy (int x, int y, char c)
{
    FillConsoleOutputCharacter(hOutput, c, 1, {x,y}, &dwWritten);
}

vector<pair<int,int>> generatePoints()
{
    vector<pair<int,int>> points;

    for (int i = 0; i < NUM_POINTS; i++)
    {
        int xrand = rand()%XSIZE;
        int yrand = 4 + rand()%(YSIZE-4);

        points.push_back(make_pair(xrand,yrand));
    }
    return points;
}

void DDA(int X0, int X1, int Y0, int Y1, vector<pair<int,int>> &drawn, bool canDraw)
{
    /*
        Implementation of DDA c/o GeeksForGeeks
        https://www.geeksforgeeks.org/dda-line-generation-algorithm-computer-graphics/
    */

    int dx = X1 - X0;
    int dy = Y1 - Y0;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    float Xinc = dx/(float) steps;
    float Yinc = dy/(float) steps;
    float X = X0;
    float Y = Y0;
    for (int i = 0; i <= steps; i++)
    {
        if (canDraw) { FillConsoleOutputCharacter(hOutput, '.', 1, {X,Y}, &dwWritten); }
        drawn.push_back(make_pair(X,Y));
        X += Xinc;
        Y += Yinc;
    }
}

vector<vector<int>> generatePermutations()
{
    vector<vector<int>> v;
    vector<int> temp(NUM_POINTS);

    for (int i = 0; i < NUM_POINTS; i++)
    {
        temp[i]=i;
    }

    for (int i = 0; i < INITIAL_POP; i++)
    {
        random_shuffle ( temp.begin(), temp.end() );
        temp.push_back(temp[0]);
        v.push_back(temp);
        temp.pop_back();
    }
    return v;
}

void gotoxy( int x, int y )
{
    COORD p = { x, y };
    FillConsoleOutputCharacter(hOutput, ' ', 10, {x,y}, &dwWritten);
    SetConsoleCursorPosition( GetStdHandle( STD_OUTPUT_HANDLE ), p );
}

pair<vector<int>,vector<int>> selection(vector<pair<int,int>> points, vector<pair<int,int>> drawn, vector<vector<int>> permutations)
{
    float dist = 0.00f;
    float minDistance = INT_MAX;
    float minDistance2 = INT_MAX;

    vector<int> parent1;
    vector<int> parent2;

    for (int i = 0; i < INITIAL_POP; i++)
    {
        dist = 0.00f;

        for (int j = 0; j < NUM_POINTS; j++)
        {
            float x2 = points[permutations[i][j+1]].first;
            float x1 = points[permutations[i][j]].first;

            float y2 = points[permutations[i][j+1]].second;
            float y1 = points[permutations[i][j]].second;

            dist += sqrtf((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));

            DDA(points[permutations[i][j]].first, points[permutations[i][j+1]].first,
                points[permutations[i][j]].second, points[permutations[i][j+1]].second,
                drawn, false);
        }

        if (dist < minDistance) { minDistance2 = minDistance; minDistance = dist;
        parent2 = parent1; parent1 = permutations[i]; }
        else if (dist < minDistance2) { minDistance2 = dist; parent2 = permutations[i]; }

        drawn.clear();
    }

    // draw best path

    gotoxy(0,0);

    for (auto it : parent1) { cout << it << " "; }
    cout << "\ndistance: " << minDistance << "\n";

    for (int i = 0; i < NUM_POINTS; i++)
    {
        for (int z = 0; z < NUM_POINTS; z++) { fillxy(points[z].first,points[z].second, char(219)); }

        DDA(points[parent1[i]].first, points[parent1[i+1]].first,
        points[parent1[i]].second, points[parent1[i+1]].second,
        drawn, true);

        Sleep(50); // to show lines being drawn
    }

    for (int z = 0; z < NUM_POINTS; z++) { fillxy(points[z].first,points[z].second, char(219)); }
    Sleep(75); // to show full path before next gen
    for (int z = 0; z < drawn.size(); z++) { fillxy(drawn[z].first, drawn[z].second, ' '); }

    drawn.clear();

    return make_pair(parent1, parent2);
}

vector<vector<int>> genNew(pair<vector<int>,vector<int>> parents)
{
    // OX1 Crossover method

    vector<vector<int>> newPop;

    for (int i = 0; i < INITIAL_POP-1; i++)
    {
        vector<int> child(NUM_POINTS, -1);

        int r1 = rand()%NUM_POINTS;
        int r2 = rand()%(NUM_POINTS-r1);
        r2+=r1;
        int idx = (r2+1)%NUM_POINTS;
        int searchIdx = (r2+1)%NUM_POINTS;

        for (int j = r1; j <= r2; j++) { child[j] = parents.first[j]; }

        while (count(child.begin(), child.end(), -1)!=0)
        {
            if (child[idx]==-1)
            {
                bool inSet = false;
                for (int j = 0; j < NUM_POINTS; j++)
                {
                    if (child[j] == parents.second[searchIdx])
                    {
                        inSet = true;
                        break;
                    }
                }
                if (!inSet)
                {
                    child[idx] = parents.second[searchIdx];
                    idx = (idx + 1)%NUM_POINTS;
                } else
                {
                    searchIdx = (searchIdx+1)%NUM_POINTS;
                }
            }
        }
        for (int j = 1; j < child.size()-2; j++)
        {
            int rMutate = 1 + rand()%100;
            if (rMutate < MUTATION_RATE)
            {
                int mutationIdx = rand()%(child.size()-2);

                swap(child[j], child[mutationIdx]);
            }
        }
        child.push_back(child[0]);
        newPop.push_back(child);
    }
    newPop.push_back(parents.first);

    return newPop;
}

int main()
{
    srand(time(NULL));
    vector<pair<int,int>> points = generatePoints();
    vector<pair<int,int>> drawn;
    vector<vector<int>> permutations = generatePermutations();
    int generations = 0;

    while (true)
    {
        cout << "generations: " << generations;
        generations++;
        pair<vector<int>,vector<int>> parents = selection(points, drawn, permutations);
        permutations = genNew(parents);
    }
}
