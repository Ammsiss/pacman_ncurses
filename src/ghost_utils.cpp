#include <vector>
#include <chrono>
#include <thread>
#include <queue>
#include <algorithm>
#include <utility>
#include <array>

#include "z_aggregate.h"
#include "ghost_utils.h"

std::vector<Vec> GhostUtils::createGhostPath(Vec start, Window& win, Vec target)
{
    auto rows{win.getWindowArea().size()};
    auto cols{win.getWindowArea()[0].size()};

    std::vector<std::vector<int>> visited(rows, std::vector<int>(cols, 0));
    std::vector<std::vector<Vec>> parentCell(rows, std::vector<Vec>(cols));

    std::queue<Vec> q{};
    q.push(start);                  // push in start point to q
    visited[start.y][start.x] = 1;  // assign start point visited

    while(!q.empty())
    {
        Vec current{ q.front() };  // assigns new variable "current" to element first out.
        q.pop();                          // removes element at front of q

        // Directions: up, down, left, right
        std::vector<Vec> directions = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};

        for(const auto& dir : directions)
        {
            Vec next{current.y + dir.y, current.x + dir.x};

            //if(next.y >= 0 && next.y < rows && next.x >= 0 && next.x < cols && !visited[next.y][next.x]) 
            if(next.y > 0 && next.y < (rows - 1) && next.x > 0 && next.x < (cols - 1) && win[{next.y, next.x}] != CellName::obstacleValue && 
                win[{next.y, next.x}] != CellName::perimeterValue && !visited[next.y][next.x])
            {
                q.push(next);
                visited[next.y][next.x] = 1;
                parentCell[next.y][next.x] = current;
            }
        }
    }

    std::vector<Vec> path{};
    Vec current{target};
    while(current.y != start.y || current.x != start.x)
    {
        path.push_back(current);
        current = parentCell[current.y][current.x];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());
    
    return path;
}
