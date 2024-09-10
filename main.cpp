#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <ctime>
#include <cstdlib>
#include <csignal>
#include <future>
#include <thread>
#include <mutex>
#include <vector>

using namespace std;

// ANSI color codes for console text
const string colorGreen = "\e[0;32m";
const string colorRed = "\e[0;31m";
const string colorYellow = "\e[0;33m";
const string colorBlue = "\e[0;34m";
const string colorReset = "\033[0m";
const string colors[] = {
    colorGreen,
    colorRed,
    colorYellow,
    colorBlue};

// Animation delays
const int typingDelay = 100000;
const int deleteDelay = 70000;
const int finalDelay = 1500000;
const int treeChangeDelay = 500000;

// Global flag to stop animation
bool stopAnimation = false;

// Function to handle Ctrl+C signal
void handleCtrlC(int signum)
{
    stopAnimation = true;
    exit(0);
}

// Function to get terminal size
void getTerminalSize(int &height, int &width)
{
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    height = size.ws_row;
    width = size.ws_col;
}

// Function to get the current year
int getCurrentYear()
{
    time_t now = time(nullptr);
    struct tm *tm_now = localtime(&now);
    return (tm_now->tm_mon + 1 == 12) ? 1900 + tm_now->tm_year + 1 : 1900 + tm_now->tm_year;
}

// Function to check if the current month is December or January
bool getHolidayMonth()
{
    time_t now = time(nullptr);
    struct tm *tm_now = localtime(&now);
    return (tm_now->tm_mon + 1 == 12 || tm_now->tm_mon + 1 == 1);
}

// Function to get a random color from predefined colors
string getRandomColor()
{
    int index = rand() % (sizeof(colors) / sizeof(colors[0]));
    return colors[index];
}

// Function to clear the console screen
void clearScreen()
{
    cout << "\033c";
}

// Function to print spaces
void printSpaces(int count)
{
    for (int i = 0; i < count; ++i)
    {
        cout << " ";
    }
}

// Function to print spaces at the top
void printTopSpace(int height)
{
    for (int i = 0; i < height; i++)
    {
        cout << "" << endl;
    }
}

// Function to print spaces on the left
void printLeftSpace(int width)
{
    for (int i = 0; i < width; i++)
    {
        cout << " ";
    }
}

// Function to print a tree
void printTree(int height, int width)
{
    int maxTreeWidth = 2 * height - 1;
    int startPosition = (width - maxTreeWidth) / 2;
    printTopSpace(height);
    for (int i = 1; i <= height; ++i)
    {
        printLeftSpace(width);
        printSpaces(startPosition);
        printSpaces(height - i);
        for (int k = 0; k < 2 * i - 1; ++k)
        {
            cout << colorGreen << "*" << colorReset;
        }
        cout << endl;
    }
    printLeftSpace(width);
    printSpaces(startPosition);
    printSpaces(height - 1);
    cout << colorRed << "*" << colorReset << endl;
    cout << endl;
}

// Function to animate a tree
void animateTree(int height, int width, int blinkInterval, int year)
{
    int maxTreeWidth = 2 * height - 1;
    int startPosition = (width - maxTreeWidth) / 2;
    while (true)
    {
        clearScreen();
        printTopSpace(height);
        for (int i = 1; i <= height; ++i)
        {
            printLeftSpace(width);
            printSpaces(startPosition);
            printSpaces(height - i);
            for (int k = 0; k < 2 * i - 1; ++k)
            {
                if (rand() % 5 == 0)
                {
                    cout << getRandomColor() << "0" << colorReset;
                }
                else
                {
                    cout << colorGreen << "*" << colorReset;
                }
            }
            cout << endl;
        }
        printLeftSpace(width);
        printSpaces(startPosition);
        printSpaces(height - 1);
        cout << colorRed << "*" << colorReset << endl;
        cout << endl;
        srand(static_cast<unsigned int>(time(nullptr)));
        string text = "Happy new " + to_string(year) + " year!";
        int spacesBefore = (width - text.length()) / 2;
        printLeftSpace(width);
        printSpaces(spacesBefore);
        for (char c : text)
        {
            string textColor = getRandomColor();
            cout << textColor << c << colorReset;
            cout.flush();
        }
        sleep(blinkInterval);
    }
}

// Mutex for tree animation
mutex treeMutex;

// Mutex for text animation
mutex textMutex;

// Wrapper function to animate the tree with a mutex
void animateTreeWrapper(int height, int width, int blinkInterval, int year)
{
    while (true)
    {
        lock_guard<mutex> lock(treeMutex);
        animateTree(height, width, blinkInterval, year);
    }
}

int main()
{
    // Register Ctrl+C signal handler
    signal(SIGINT, handleCtrlC);

    // Get terminal size
    int terminalHeight, terminalWidth;
    getTerminalSize(terminalHeight, terminalWidth);

    int blinkInterval = 1;

    // Get the current year and check if it's a holiday month
    int year = getCurrentYear();
    bool isHolidayMonth = getHolidayMonth();

    if (isHolidayMonth)
    {
        // Start tree animation thread for holiday month
        clearScreen();
        thread treeThread(animateTreeWrapper, terminalHeight / 3, terminalWidth / 3, blinkInterval, year);
        while (!stopAnimation)
        {
            usleep(treeChangeDelay);
        }
        treeThread.join();
    }
    else
    {
        // Start tree printing thread for non-holiday month
        clearScreen();
        thread treeThread(printTree, terminalHeight / 3, terminalWidth / 3);
        while (!stopAnimation)
        {
            usleep(treeChangeDelay);
        }
        treeThread.join();
    }

    return 0;
}
