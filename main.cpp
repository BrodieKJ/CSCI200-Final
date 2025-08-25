/*
 * CSCI 200: Final Project (_GIVE_BRIEF_DESCRIPTION_HERE_) A program that generates a random maze.
 *
 * Author: Brodie KJ (_INSERT_YOUR_NAME_HERE_)
 *
 * This program creates a GUI with several features. The program gives step by step intructions to the user for creating a maze across 3 layers
 * The user is able to select the start and end potion of reach layer. The next layers start potion is always based on the previous layer's end position.
 * The user can then press the generate button to generate the maze. The user can also save and load mazes.
 */

#include <SFML/Graphics.hpp>
#include "Maze.h"
#include <chrono>
#include <iostream>
#include <random>
#include <fstream>

using namespace sf;
using namespace std;

int main()
{
    const float BUTTON_HEIGHT = 40;
    const float BUTTON_SPACING = 20;
    const float TOP_MARGIN = 20;
    const float CELL_BORDER = 1;
    const float MIN_CELL_SIZE = 25;
    const float SIDE_MARGIN = 20;
    const float FIXED_CELL_SIZE = MIN_CELL_SIZE + CELL_BORDER * 2;
    const float INSTRUCTION_BOX_HEIGHT = 60;
    const float INSTRUCTION_BOX_MARGIN = 5;
    const float BUTTON_ROW_SPACING = 20;

    const int MAZE_WIDTH = 22;
    const int MAZE_HEIGHT = 22;
    const int WINDOW_WIDTH = (MAZE_WIDTH + 1) * FIXED_CELL_SIZE + (SIDE_MARGIN * 2);
    const int WINDOW_HEIGHT = MAZE_HEIGHT * FIXED_CELL_SIZE + (TOP_MARGIN + BUTTON_HEIGHT + TOP_MARGIN + BUTTON_HEIGHT + BUTTON_ROW_SPACING) + INSTRUCTION_BOX_HEIGHT + INSTRUCTION_BOX_MARGIN;

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Maze Generator");
    window.setFramerateLimit(60);

    Event event;
    Vector2u windowSize;

    Maze maze;
    Maze mazeCopy;
    Maze mazeLayer2;
    Maze mazeLayer3;

    int currentLayer = 1;
    bool mazeSetupComplete = false;

    maze.resizeMaze(MAZE_WIDTH, MAZE_HEIGHT);
    mazeLayer2.resizeMaze(MAZE_WIDTH, MAZE_HEIGHT);
    mazeLayer3.resizeMaze(MAZE_WIDTH, MAZE_HEIGHT);

    maze.setStart(-1);
    maze.setStartY(-1);
    maze.setEnd(-1);
    maze.setEndY(-1);

    mazeLayer2.setStart(-1);
    mazeLayer2.setStartY(-1);
    mazeLayer2.setEnd(-1);
    mazeLayer2.setEndY(-1);

    mazeLayer3.setStart(-1);
    mazeLayer3.setStartY(-1);
    mazeLayer3.setEnd(-1);
    mazeLayer3.setEndY(-1);

    Font font;
    font.loadFromFile("arial.ttf");

    Text instructionText("Select a start position (Must be on the edge)", font, 20);
    instructionText.setFillColor(Color::Black);

    while (window.isOpen())
    {
        window.clear();

        windowSize = window.getSize();

        float gridX = SIDE_MARGIN + FIXED_CELL_SIZE;
        float gridY = TOP_MARGIN + BUTTON_HEIGHT + TOP_MARGIN + BUTTON_HEIGHT + BUTTON_ROW_SPACING;

        float totalMazeWidth = maze.getWidth() * FIXED_CELL_SIZE;

        float totalButtonWidth = totalMazeWidth;
        float buttonWidth = (totalButtonWidth - (BUTTON_SPACING * 3)) / 4;
        float layerButtonWidth = (totalButtonWidth - BUTTON_SPACING) / 2;

        float buttonStartX = gridX;

        RectangleShape generateButton(Vector2f(buttonWidth, BUTTON_HEIGHT));
        generateButton.setPosition(buttonStartX, TOP_MARGIN);
        generateButton.setFillColor(Color::Cyan);

        RectangleShape clearButton(Vector2f(buttonWidth, BUTTON_HEIGHT));
        clearButton.setPosition(buttonStartX + buttonWidth + BUTTON_SPACING, TOP_MARGIN);
        clearButton.setFillColor(Color::Cyan);

        RectangleShape saveButton(Vector2f(buttonWidth, BUTTON_HEIGHT));
        saveButton.setPosition(buttonStartX + (buttonWidth + BUTTON_SPACING) * 2, TOP_MARGIN);
        saveButton.setFillColor(Color::Cyan);

        RectangleShape loadButton(Vector2f(buttonWidth, BUTTON_HEIGHT));
        loadButton.setPosition(buttonStartX + (buttonWidth + BUTTON_SPACING) * 3, TOP_MARGIN);
        loadButton.setFillColor(Color::Cyan);

        RectangleShape prevLayerButton(Vector2f(layerButtonWidth, BUTTON_HEIGHT));
        prevLayerButton.setPosition(buttonStartX, TOP_MARGIN + BUTTON_HEIGHT + BUTTON_ROW_SPACING);
        prevLayerButton.setFillColor(Color::Cyan);

        RectangleShape nextLayerButton(Vector2f(layerButtonWidth, BUTTON_HEIGHT));
        nextLayerButton.setPosition(buttonStartX + layerButtonWidth + BUTTON_SPACING, TOP_MARGIN + BUTTON_HEIGHT + BUTTON_ROW_SPACING);
        nextLayerButton.setFillColor(Color::Cyan);

        Text generateText("Generate", font, 20);
        generateText.setPosition(
            buttonStartX + (buttonWidth - generateText.getLocalBounds().width) / 2,
            TOP_MARGIN + (BUTTON_HEIGHT - generateText.getLocalBounds().height) / 2);
        generateText.setFillColor(Color::Black);

        Text clearText("Clear", font, 20);
        clearText.setPosition(
            buttonStartX + buttonWidth + BUTTON_SPACING + (buttonWidth - clearText.getLocalBounds().width) / 2,
            TOP_MARGIN + (BUTTON_HEIGHT - clearText.getLocalBounds().height) / 2);
        clearText.setFillColor(Color::Black);

        Text saveText("Save", font, 20);
        saveText.setPosition(
            buttonStartX + (buttonWidth + BUTTON_SPACING) * 2 + (buttonWidth - saveText.getLocalBounds().width) / 2,
            TOP_MARGIN + (BUTTON_HEIGHT - saveText.getLocalBounds().height) / 2);
        saveText.setFillColor(Color::Black);

        Text loadText("Load", font, 20);
        loadText.setPosition(
            buttonStartX + (buttonWidth + BUTTON_SPACING) * 3 + (buttonWidth - loadText.getLocalBounds().width) / 2,
            TOP_MARGIN + (BUTTON_HEIGHT - loadText.getLocalBounds().height) / 2);
        loadText.setFillColor(Color::Black);

        Text prevLayerText("Prev Layer", font, 20);
        prevLayerText.setPosition(
            buttonStartX + (layerButtonWidth - prevLayerText.getLocalBounds().width) / 2,
            TOP_MARGIN + BUTTON_HEIGHT + BUTTON_ROW_SPACING + (BUTTON_HEIGHT - prevLayerText.getLocalBounds().height) / 2);
        prevLayerText.setFillColor(Color::Black);

        Text nextLayerText("Next Layer", font, 20);
        nextLayerText.setPosition(
            buttonStartX + layerButtonWidth + BUTTON_SPACING + (layerButtonWidth - nextLayerText.getLocalBounds().width) / 2,
            TOP_MARGIN + BUTTON_HEIGHT + BUTTON_ROW_SPACING + (BUTTON_HEIGHT - nextLayerText.getLocalBounds().height) / 2);
        nextLayerText.setFillColor(Color::Black);

        RectangleShape instructionBox(Vector2f(totalMazeWidth, INSTRUCTION_BOX_HEIGHT));
        instructionBox.setPosition(gridX, gridY + maze.getHeight() * FIXED_CELL_SIZE + INSTRUCTION_BOX_MARGIN);
        instructionBox.setFillColor(Color::White);
        instructionBox.setOutlineColor(Color::Black);
        instructionBox.setOutlineThickness(1);

        instructionText.setPosition(
            gridX + (totalMazeWidth - instructionText.getLocalBounds().width) / 2,
            gridY + maze.getHeight() * FIXED_CELL_SIZE + INSTRUCTION_BOX_MARGIN + (INSTRUCTION_BOX_HEIGHT - instructionText.getLocalBounds().height) / 2);

        window.draw(generateButton);
        window.draw(clearButton);
        window.draw(saveButton);
        window.draw(loadButton);
        window.draw(generateText);
        window.draw(clearText);
        window.draw(saveText);
        window.draw(loadText);

        window.draw(prevLayerButton);
        window.draw(nextLayerButton);
        window.draw(prevLayerText);
        window.draw(nextLayerText);

        window.draw(instructionBox);
        window.draw(instructionText);

        Maze &currentMaze = (currentLayer == 1) ? maze : (currentLayer == 2 ? mazeLayer2 : mazeLayer3);
        for (int i = 0; i < currentMaze.getHeight(); i++)
        {
            for (int j = 0; j < currentMaze.getWidth(); j++)
            {
                int cellValue = currentMaze.getCell(i, j);

                RectangleShape cell(Vector2f(MIN_CELL_SIZE, MIN_CELL_SIZE));
                cell.setPosition(
                    gridX + j * FIXED_CELL_SIZE + CELL_BORDER,
                    gridY + i * FIXED_CELL_SIZE + CELL_BORDER);

                if (cellValue == 0)
                    cell.setFillColor(Color::White);
                else if (cellValue == 1)
                    cell.setFillColor(Color::Green);
                else if (cellValue == 2)
                {
                    cell.setFillColor(Color(128, 128, 128));
                    cell.setOutlineColor(Color::Black);
                    cell.setOutlineThickness(CELL_BORDER);
                    window.draw(cell);

                    const float stepHeight = MIN_CELL_SIZE / 3;
                    const Color darkGrey(64, 64, 64);

                    for (int step = 0; step < 3; step++)
                    {
                        RectangleShape stepRect(Vector2f(MIN_CELL_SIZE - 2, stepHeight - 2));
                        stepRect.setPosition(gridX + j * FIXED_CELL_SIZE + CELL_BORDER + 1, gridY + i * FIXED_CELL_SIZE + CELL_BORDER + (2 - step) * stepHeight + 1);
                        stepRect.setFillColor(darkGrey);

                        window.draw(stepRect);
                    }
                    continue;
                }
                else if (cellValue == 3)
                {
                    cell.setFillColor(Color(139, 69, 19));
                    cell.setOutlineColor(Color::Black);
                    cell.setOutlineThickness(CELL_BORDER);
                    window.draw(cell);

                    const float stepHeight = MIN_CELL_SIZE / 3;
                    const Color darkBrown(101, 67, 33);

                    for (int step = 0; step < 3; step++)
                    {
                        RectangleShape stepRect(Vector2f(MIN_CELL_SIZE - 2, stepHeight - 2));
                        stepRect.setPosition(gridX + j * FIXED_CELL_SIZE + CELL_BORDER + 1, gridY + i * FIXED_CELL_SIZE + CELL_BORDER + (2 - step) * stepHeight + 1);
                        stepRect.setFillColor(darkBrown);
                        window.draw(stepRect);
                    }
                    continue;
                }
                else if (cellValue == 4)
                    cell.setFillColor(Color(255, 128, 255));
                else
                    cell.setFillColor(Color::White);

                cell.setOutlineColor(Color::Black);
                cell.setOutlineThickness(CELL_BORDER);
                window.draw(cell);
            }
        }

        window.display();

        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }

            if (event.type == Event::Resized)
            {
                View view = window.getView();
                view.setSize(event.size.width, event.size.height);
                view.setCenter(event.size.width / 2, event.size.height / 2);
                window.setView(view);

                float newAvailableWidth = event.size.width - (SIDE_MARGIN * 2);
                float newAvailableHeight = event.size.height - (TOP_MARGIN + BUTTON_HEIGHT + TOP_MARGIN + BUTTON_HEIGHT + BUTTON_ROW_SPACING + INSTRUCTION_BOX_HEIGHT + INSTRUCTION_BOX_MARGIN);

                int newMazeWidth = (int)((newAvailableWidth - FIXED_CELL_SIZE * 2) / FIXED_CELL_SIZE);
                int newMazeHeight = (int)((newAvailableHeight - FIXED_CELL_SIZE) / FIXED_CELL_SIZE);

                newMazeWidth = max(newMazeWidth, 5) | 1;
                newMazeHeight = max(newMazeHeight, 5) | 1;

                cout << "Window size: " << event.size.width << "x" << event.size.height << endl;
                cout << "Available space: " << newAvailableWidth << "x" << newAvailableHeight << endl;
                cout << "Fixed cell size: " << FIXED_CELL_SIZE << endl;
                cout << "New maze dimensions: " << newMazeWidth << "x" << newMazeHeight << endl;

                if (newMazeWidth != maze.getWidth() || newMazeHeight != maze.getHeight())
                {
                    cout << "Resizing maze to " << newMazeWidth << "x" << newMazeHeight << endl;

                    maze.setStart(-1);
                    maze.setStartY(-1);
                    maze.setEnd(-1);
                    maze.setEndY(-1);

                    mazeLayer2.setStart(-1);
                    mazeLayer2.setStartY(-1);
                    mazeLayer2.setEnd(-1);
                    mazeLayer2.setEndY(-1);

                    mazeLayer3.setStart(-1);
                    mazeLayer3.setStartY(-1);
                    mazeLayer3.setEnd(-1);
                    mazeLayer3.setEndY(-1);

                    maze.resizeMaze(newMazeWidth, newMazeHeight);
                    mazeLayer2.resizeMaze(newMazeWidth, newMazeHeight);
                    mazeLayer3.resizeMaze(newMazeWidth, newMazeHeight);

                    currentLayer = 1;
                    mazeSetupComplete = false;
                    instructionText.setString("Select a start position");
                }

                windowSize = window.getSize();
            }

            if (Mouse::isButtonPressed(Mouse::Left))
            {
                Vector2i mp = Mouse::getPosition(window);

                float x = (mp.x - gridX) / FIXED_CELL_SIZE;
                float y = (mp.y - gridY) / FIXED_CELL_SIZE;

                // Debug output
                cout << "Mouse position: " << mp.x << "," << mp.y << endl;
                cout << "Grid coordinates: " << x << "," << y << endl;
                cout << "Grid X: " << gridX << " Grid Y: " << gridY << endl;
                cout << "Current layer: " << currentLayer << endl;

                if (x >= 0 && x < maze.getWidth() && y >= 0 && y < maze.getHeight())
                {
                    int mouseGridX = (int)x;
                    int mouseGridY = (int)y;

                    if (!mazeSetupComplete)
                    {
                        if (currentLayer == 1)
                        {
                            cout << "Layer 1: Start pos = " << maze.getStart() << ", End pos = " << maze.getEnd() << endl;

                            if (maze.getStart() == -1)
                            {
                                cout << "Attempting to set start position" << endl;

                                bool isOnEdge = (mouseGridY == 0 || mouseGridY == maze.getHeight() - 1 ||
                                                 mouseGridX == 0 || mouseGridX == maze.getWidth() - 1);
                                bool isCorner = ((mouseGridY == 0 || mouseGridY == maze.getHeight() - 1) &&
                                                 (mouseGridX == 0 || mouseGridX == maze.getWidth() - 1));

                                cout << "Is on edge: " << isOnEdge << ", Is corner: " << isCorner << endl;

                                if (isOnEdge && !isCorner)
                                {
                                    cout << "Setting start position at " << mouseGridX << "," << mouseGridY << endl;

                                    maze.setStart(mouseGridX);
                                    maze.setStartY(mouseGridY);
                                    maze.setCell(mouseGridY, mouseGridX, 2);

                                    instructionText.setString("Select the first layer's staircase position.");
                                }
                            }
                            else if (maze.getEnd() == -1)
                            {
                                cout << "Attempting to set first staircase" << endl;

                                bool isInside = (mouseGridY > 0 && mouseGridY < maze.getHeight() - 1 &&
                                                 mouseGridX > 0 && mouseGridX < maze.getWidth() - 1);

                                cout << "Is inside: " << isInside << endl;

                                if (isInside)
                                {
                                    cout << "Setting first staircase at " << mouseGridX << "," << mouseGridY << endl;

                                    maze.setEnd(mouseGridX);
                                    maze.setEndY(mouseGridY);
                                    maze.setCell(mouseGridY, mouseGridX, 3);

                                    mazeLayer2.setStart(mouseGridX);
                                    mazeLayer2.setStartY(mouseGridY);
                                    mazeLayer2.setCell(mouseGridY, mouseGridX, 2);

                                    currentLayer = 2;

                                    instructionText.setString("Select the second layer's staircase position.");
                                }
                            }
                        }
                        else if (currentLayer == 2)
                        {
                            cout << "Layer 2: Start pos = " << mazeLayer2.getStart() << ", End pos = " << mazeLayer2.getEnd() << endl;

                            if (mazeLayer2.getEnd() == -1)
                            {
                                cout << "Attempting to set second staircase" << endl;

                                bool isInside = (mouseGridY > 0 && mouseGridY < mazeLayer2.getHeight() - 1 &&
                                                 mouseGridX > 0 && mouseGridX < mazeLayer2.getWidth() - 1);

                                cout << "Is inside: " << isInside << endl;

                                if (isInside)
                                {
                                    cout << "Setting second staircase at " << mouseGridX << "," << mouseGridY << endl;

                                    mazeLayer2.setEnd(mouseGridX);
                                    mazeLayer2.setEndY(mouseGridY);
                                    mazeLayer2.setCell(mouseGridY, mouseGridX, 3);

                                    mazeLayer3.setStart(mouseGridX);
                                    mazeLayer3.setStartY(mouseGridY);
                                    mazeLayer3.setCell(mouseGridY, mouseGridX, 2);

                                    currentLayer = 3;

                                    instructionText.setString("Select the end position.");
                                }
                            }
                        }
                        else if (currentLayer == 3)
                        {
                            cout << "Layer 3: Start pos = " << mazeLayer3.getStart() << ", End pos = " << mazeLayer3.getEnd() << endl;

                            if (mazeLayer3.getEnd() == -1)
                            {
                                cout << "Attempting to set end position" << endl;

                                bool isOnEdge = (mouseGridY == 0 || mouseGridY == mazeLayer3.getHeight() - 1 || mouseGridX == 0 || mouseGridX == mazeLayer3.getWidth() - 1);
                                bool isCorner = ((mouseGridY == 0 || mouseGridY == mazeLayer3.getHeight() - 1) && (mouseGridX == 0 || mouseGridX == mazeLayer3.getWidth() - 1));

                                cout << "Is on edge: " << isOnEdge << ", Is corner: " << isCorner << endl;

                                if (isOnEdge && !isCorner)
                                {
                                    cout << "Setting end position at " << mouseGridX << "," << mouseGridY << endl;

                                    mazeLayer3.setEnd(mouseGridX);
                                    mazeLayer3.setEndY(mouseGridY);
                                    mazeLayer3.setCell(mouseGridY, mouseGridX, 3);

                                    cout << "Layer 3 end position set to: (" << mazeLayer3.getEnd() << "," << mazeLayer3.getEndY() << ")" << endl;

                                    mazeSetupComplete = true;

                                    instructionText.setString("Press Generate to create the maze");
                                }
                            }
                        }
                    }
                }
            }

            if (event.type == Event::MouseButtonPressed)
            {
                Vector2i mp = Mouse::getPosition(window);

                if (prevLayerButton.getGlobalBounds().contains(mp.x, mp.y))
                {
                    if (currentLayer > 1)
                    {
                        currentLayer--;
                        if (currentLayer == 1)
                        {
                            if (maze.getStart() == -1)
                                instructionText.setString("Select a start position");
                            else if (maze.getEnd() == -1)
                                instructionText.setString("Select the first layer'sstaircase position");
                        }
                        else if (currentLayer == 2)
                        {
                            if (mazeLayer2.getEnd() == -1)
                                instructionText.setString("Select the second layer'sstaircase position");
                        }
                    }
                }
                else if (nextLayerButton.getGlobalBounds().contains(mp.x, mp.y))
                {
                    if (currentLayer < 3)
                    {
                        currentLayer++;

                        if (currentLayer == 2)
                        {
                            if (mazeLayer2.getEnd() == -1)
                                instructionText.setString("Select the second layer's staircase position");
                        }
                        else if (currentLayer == 3)
                        {
                            if (mazeLayer3.getEnd() == -1)
                                instructionText.setString("Select the end position");
                        }
                    }
                }
                else if (generateButton.getGlobalBounds().contains(mp.x, mp.y))
                {
                    cout << "Generate button pressed" << endl;
                    cout << "Layer 3 positions - Start: (" << mazeLayer3.getStart() << "," << mazeLayer3.getStartY()
                         << ") End: (" << mazeLayer3.getEnd() << "," << mazeLayer3.getEndY() << ")" << endl;

                    maze.generateMaze();
                    mazeLayer2.generateMaze();
                    mazeLayer3.generateMaze();
                }
                else if (clearButton.getGlobalBounds().contains(mp.x, mp.y))
                {
                    cout << "Clear button pressed" << endl;
                    int currentWidth = maze.getWidth();
                    int currentHeight = maze.getHeight();

                    maze.setStart(-1);
                    maze.setStartY(-1);
                    maze.setEnd(-1);
                    maze.setEndY(-1);

                    mazeLayer2.setStart(-1);
                    mazeLayer2.setStartY(-1);
                    mazeLayer2.setEnd(-1);
                    mazeLayer2.setEndY(-1);

                    mazeLayer3.setStart(-1);
                    mazeLayer3.setStartY(-1);
                    mazeLayer3.setEnd(-1);
                    mazeLayer3.setEndY(-1);

                    maze.resizeMaze(currentWidth, currentHeight);
                    mazeLayer2.resizeMaze(currentWidth, currentHeight);
                    mazeLayer3.resizeMaze(currentWidth, currentHeight);

                    currentLayer = 1;
                    mazeSetupComplete = false;
                    instructionText.setString("Select a start position");
                }
                else if (saveButton.getGlobalBounds().contains(mp.x, mp.y))
                {
                    cout << "Save button pressed" << endl;

                    ofstream dimFile("maze_dimensions.txt");
                    if (dimFile.is_open())
                    {
                        dimFile << window.getSize().x << " " << window.getSize().y << endl;
                        dimFile.close();
                    }

                    maze.saveMaze("maze_layer1.txt");
                    mazeLayer2.saveMaze("maze_layer2.txt");
                    mazeLayer3.saveMaze("maze_layer3.txt");
                }
                else if (loadButton.getGlobalBounds().contains(mp.x, mp.y))
                {
                    cout << "Load button pressed" << endl;

                    ifstream dimFile("maze_dimensions.txt");
                    if (dimFile.is_open())
                    {
                        int width, height;
                        dimFile >> width >> height;
                        dimFile.close();

                        window.setSize(Vector2u(width, height));

                        View view = window.getView();
                        view.setSize(width, height);
                        view.setCenter(width / 2, height / 2);
                        window.setView(view);

                        float newAvailableWidth = width - (SIDE_MARGIN * 2);
                        float newAvailableHeight = height - (TOP_MARGIN + BUTTON_HEIGHT + TOP_MARGIN + BUTTON_HEIGHT + BUTTON_ROW_SPACING + INSTRUCTION_BOX_HEIGHT + INSTRUCTION_BOX_MARGIN);

                        int newMazeWidth = (int)((newAvailableWidth - FIXED_CELL_SIZE * 2) / FIXED_CELL_SIZE);
                        int newMazeHeight = (int)((newAvailableHeight - FIXED_CELL_SIZE) / FIXED_CELL_SIZE);

                        newMazeWidth = max(newMazeWidth, 5) | 1;
                        newMazeHeight = max(newMazeHeight, 5) | 1;

                        maze.resizeMaze(newMazeWidth, newMazeHeight);
                        mazeLayer2.resizeMaze(newMazeWidth, newMazeHeight);
                        mazeLayer3.resizeMaze(newMazeWidth, newMazeHeight);
                    }

                    maze.loadMaze("maze_layer1.txt");
                    mazeLayer2.loadMaze("maze_layer2.txt");
                    mazeLayer3.loadMaze("maze_layer3.txt");

                    mazeSetupComplete = true;

                    currentLayer = 1;
                    instructionText.setString("Press Generate to create the maze");
                }
            }
        }
    }

    return 0;
}
