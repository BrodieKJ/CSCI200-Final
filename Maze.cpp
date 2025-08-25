#include "Maze.h"
#include "random.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <stack>
#include <utility>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

Maze::Maze()
{
    width = 22;
    height = 22;
    startPos = -1;
    startY = -1;
    endPos = -1;
    endY = -1;

    maze = new int *[height];
    for (int i = 0; i < height; i++)
    {
        maze[i] = new int[width];
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (i == 0 || j == 0 || i == height - 1 || j == width - 1)
            {
                maze[i][j] = 1;
            }
            else
            {
                maze[i][j] = 0;
            }
        }
    }
}

Maze::~Maze()
{
    for (int i = 0; i < height; i++)
    {
        delete[] maze[i];
    }
    delete[] maze;
}

void Maze::resizeMaze(int newWidth, int newHeight)
{
    if (newWidth < 5 || newHeight < 5)
    {
        newWidth = max(newWidth, 5) | 1;
        newHeight = max(newHeight, 5) | 1;
    }

    int **newMaze = new int *[newHeight];

    for (int i = 0; i < newHeight; i++)
    {
        newMaze[i] = new int[newWidth];
    }

    for (int i = 0; i < newHeight; i++)
    {
        for (int j = 0; j < newWidth; j++)
        {
            if (i == 0 || j == 0 || i == newHeight - 1 || j == newWidth - 1)
            {
                newMaze[i][j] = 1;
            }
            else
            {
                newMaze[i][j] = 0;
            }
        }
    }

    if (maze != nullptr)
    {
        for (int i = 0; i < height; i++)
        {
            delete[] maze[i];
        }
        delete[] maze;
    }

    maze = newMaze;
    width = newWidth;
    height = newHeight;

    startPos = -1;
    startY = -1;
    endPos = -1;
    endY = -1;
}

void Maze::generateMaze()
{
    if (startPos < 0 || startY < 0 || endPos < 0 || endY < 0)
    {
        cout << "Cannot generate maze: Invalid start or end positions" << endl;
        return;
    }

    bool mazeSolved = false;
    int attempts = 0;
    const int MAX_ATTEMPTS = 100;

    while (!mazeSolved && attempts < MAX_ATTEMPTS)
    {
        attempts++;

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                maze[i][j] = 1;
            }
        }

        bool **visited = new bool *[height];

        for (int i = 0; i < height; i++)
        {
            visited[i] = new bool[width]();

            for (int j = 0; j < width; j++)
            {
                visited[i][j] = false;
            }
        }

        int startX = Random::getInstance().gen(1, width - 2);
        int dfsStartY = Random::getInstance().gen(1, height - 2);

        stack<pair<int, int>> stack;

        stack.push({startX, dfsStartY});

        visited[dfsStartY][startX] = true;

        maze[dfsStartY][startX] = 0;

        vector<pair<int, int>> directions = {
            {0, -2},
            {2, 0},
            {0, 2},
            {-2, 0}};

        while (!stack.empty())
        {
            int x = stack.top().first;
            int y = stack.top().second;

            vector<pair<int, int>> unvisitedNeighbors;

            for (const auto &dir : directions)
            {
                int nx = x + dir.first;
                int ny = y + dir.second;

                if (nx > 0 && nx < width - 1 && ny > 0 && ny < height - 1 && !visited[ny][nx])
                {
                    unvisitedNeighbors.push_back({nx, ny});
                }
            }

            if (!unvisitedNeighbors.empty())
            {

                int randomIndex = Random::getInstance().gen(0, unvisitedNeighbors.size() - 1);

                int nx = unvisitedNeighbors[randomIndex].first;
                int ny = unvisitedNeighbors[randomIndex].second;

                int wallX = x + (nx - x) / 2;
                int wallY = y + (ny - y) / 2;

                maze[wallY][wallX] = 0;
                maze[ny][nx] = 0;

                visited[ny][nx] = true;

                stack.push({nx, ny});
            }
            else
            {
                stack.pop();
            }
        }

        for (int i = 0; i < height; i++)
        {
            delete[] visited[i];
        }

        delete[] visited;

        maze[startY][startPos] = 2;
        maze[endY][endPos] = 3;

        if (startY == 0)
        {
            maze[1][startPos] = 0;
        }
        else if (startY == height - 1)
        {
            maze[height - 2][startPos] = 0;
        }
        else if (startPos == 0)
        {
            maze[startY][1] = 0;
        }
        else
        {
            maze[startY][width - 2] = 0;
        }

        mazeSolved = solveMaze();
    }

    if (!mazeSolved)
    {
        cout << "Failed to generate a solvable maze after " << MAX_ATTEMPTS << " attempts" << endl;

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (i == 0 || j == 0 || i == height - 1 || j == width - 1)
                {
                    maze[i][j] = 1;
                }
                else
                {
                    maze[i][j] = 0;
                }
            }
        }

        if (startPos >= 0 && startY >= 0)
        {
            maze[startY][startPos] = 2;
        }
        if (endPos >= 0 && endY >= 0)
        {
            maze[endY][endPos] = 3;
        }
    }
    else
    {
        cout << "Generated solvable maze after " << attempts << " attempts" << endl;
    }
}

bool Maze::solveMaze()
{
    bool **visited = new bool *[height];

    pair<int, int> **parent = new pair<int, int> *[height];

    for (int i = 0; i < height; i++)
    {
        visited[i] = new bool[width]();
        parent[i] = new pair<int, int>[width];
    }

    const vector<pair<int, int>> directions = {
        {0, -1}, {-1, 0}, {0, 1}, {1, 0}};

    int startX = startPos;
    int currentStartY = this->startY;
    int targetX = endPos;
    int targetY = this->endY;

    queue<pair<int, int>> q;

    q.push({currentStartY, startX});
    visited[currentStartY][startX] = true;
    parent[currentStartY][startX] = {-1, -1};

    bool found = false;

    while (!q.empty() && !found)
    {
        auto [x, y] = q.front();
        q.pop();

        if (x == targetY && y == targetX)
        {
            found = true;
            break;
        }

        for (const auto &dir : directions)
        {
            int nx = x + dir.first;
            int ny = y + dir.second;

            if (nx >= 0 && nx < height && ny >= 0 && ny < width && !visited[nx][ny] && maze[nx][ny] != 1)
            {
                q.push({nx, ny});
                visited[nx][ny] = true;
                parent[nx][ny] = {x, y};
            }
        }
    }

    if (found)
    {
        int x = targetY;
        int y = targetX;

        while (x != -1 && y != -1)
        {
            if (maze[x][y] != 2 && maze[x][y] != 3)
            {
                maze[x][y] = 4;
            }

            auto [px, py] = parent[x][y];
            x = px;
            y = py;
        }
    }

    for (int i = 0; i < height; i++)
    {
        delete[] visited[i];
        delete[] parent[i];
    }
    delete[] visited;
    delete[] parent;

    return found;
}

void Maze::setCell(int x, int y, int type)
{
    if (x >= 0 && x < height && y >= 0 && y < width)
    {
        maze[x][y] = type;
    }
}

void Maze::setStart(int start)
{
    if (start == 0 || start == width - 1 || (start > 0 && start < width - 1))
    {
        startPos = start;
    }
}

void Maze::setEnd(int end)
{
    if (end == 0 || end == width - 1 || (end > 0 && end < width - 1))
    {
        endPos = end;
    }
}

int Maze::getCell(int x, int y)
{
    if (x >= 0 && x < height && y >= 0 && y < width)
    {
        return maze[x][y];
    }
    return 1;
}

int Maze::getStart()
{

    return startPos;
}

int Maze::getEnd()
{

    return endPos;
}

void Maze::setEndY(int y)
{
    if (y == 0 || y == height - 1 || (y > 0 && y < height - 1))
    {
        endY = y;
    }
}

int Maze::getEndY()
{
    return endY;
}

void Maze::setStartY(int y)
{
    if (y == 0 || y == height - 1 || (y > 0 && y < height - 1))
    {
        startY = y;
    }
}

int Maze::getStartY()
{
    return startY;
}

void Maze::saveMaze(const string &filename)
{

    ofstream file(filename);

    if (file.is_open())
    {

        for (int i = 0; i < height; i++)
        {

            for (int j = 0; j < width; j++)
            {

                file << maze[i][j] << " ";
            }

            file << endl;
        }

        file.close();
    }
    else
    {

        cerr << "Failed to open file: " << filename << endl;
    }
}

void Maze::loadMaze(const string &filename)
{

    ifstream file(filename);

    if (file.is_open())
    {

        for (int i = 0; i < height; i++)
        {

            for (int j = 0; j < width; j++)
            {

                file >> maze[i][j];
            }
        }

        file.close();
    }
    else
    {

        cerr << "Failed to open file: " << filename << endl;
    }
}

Maze::Maze(const Maze &other) : maze(nullptr), width(0), height(0), startPos(0), endPos(0)
{
    width = other.width;
    height = other.height;
    startPos = other.startPos;
    startY = other.startY;
    endPos = other.endPos;
    endY = other.endY;

    maze = new int *[height];
    for (int i = 0; i < height; i++)
    {
        maze[i] = new int[width];
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            maze[i][j] = other.maze[i][j];
        }
    }
}

Maze &Maze::operator=(const Maze &other)
{
    if (this != &other)
    {
        if (maze != nullptr)
        {
            for (int i = 0; i < height; i++)
            {
                delete[] maze[i];
            }
            delete[] maze;
        }

        width = other.width;
        height = other.height;
        startPos = other.startPos;
        startY = other.startY;
        endPos = other.endPos;
        endY = other.endY;

        maze = new int *[height];
        for (int i = 0; i < height; i++)
        {
            maze[i] = new int[width];
        }

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                maze[i][j] = other.maze[i][j];
            }
        }
    }
    return *this;
}
