#pragma once
#include <string>

class Maze
{
public:
    Maze();
    Maze(const Maze &other);
    Maze &operator=(const Maze &other);
    ~Maze();

    void setCell(int, int, int);

    void setStart(int);

    void setStartY(int);

    void setEnd(int);

    void setEndY(int);

    int getCell(int, int);

    int getStart();

    int getStartY();

    int getEnd();

    int getEndY();

    void generateMaze();

    bool solveMaze();

    void drawMaze();

    void saveMaze(const std::string &);

    void loadMaze(const std::string &);

    void resizeMaze(int width, int height);

    int getWidth() const { return width; }

    int getHeight() const { return height; }

private:
    int **maze;

    int width;

    int height;

    int startPos;

    int startY;

    int endPos;

    int endY;
};