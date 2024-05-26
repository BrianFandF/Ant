#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>
#include <random>

const int N = 10;
const double Q = 10.0;
const double alpha = 1.0;
const double beta = 5.0;
const double rho = 0.6;
const double initialPheromone = 0.2;
const int numAnts = 10;
const int maxIterations = 100;

std::vector<std::vector<double>> distanceMatrix = {
    {0, 18.03, 85.89, 72.11, 45.28, 77.78, 91.92, 79.06, 70.71, 109.66},
    {18.03, 0, 70, 54.08, 36.4, 60.21, 74.33, 62.65, 60.21, 91.92},
    {85.89, 70, 0, 39.05, 87.32, 54.08, 58.52, 68.01, 96.05, 65.19},
    {72.11, 54.08, 39.05, 0, 55.23, 15.81, 25.5, 58.31, 55.23, 40.31},
    {45.28, 36.4, 87.32, 55.23, 0, 50.99, 63.25, 44.72, 25.5, 80.78},
    {77.78, 60.21, 54.08, 15.81, 50.99, 0, 14.14, 14.14, 47.43, 32.02},
    {91.92, 74.33, 58.52, 25.5, 63.25, 14.14, 0, 20, 55.23, 18.03},
    {79.06, 62.65, 68.01, 58.31, 44.72, 14.14, 20, 0, 35.36, 36.4},
    {70.71, 60.21, 96.05, 55.23, 25.5, 47.43, 55.23, 35.36, 0, 70.18},
    {109.66, 91.92, 65.19, 40.31, 80.78, 32.02, 18.03, 36.4, 70.18, 0}
};

std::vector<std::vector<double>> pheromoneMatrix(N, std::vector<double>(N, initialPheromone));

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.0, 1.0);

double calculateProbability(int from, int to, const std::vector<bool>& visited) {
    if (visited[to]) return 0.0;

    double pheromone = std::pow(pheromoneMatrix[from][to], alpha);
    double inverseDistance = std::pow(1.0 / distanceMatrix[from][to], beta);

    return pheromone * inverseDistance;
}

int selectNextCity(int currentCity, const std::vector<bool>& visited) {
    double sumProbabilities = 0.0;
    for (int i = 0; i < N; ++i) {
        sumProbabilities += calculateProbability(currentCity, i, visited);
    }

    double randomValue = distribution(generator) * sumProbabilities;
    double cumulativeProbability = 0.0;

    for (int i = 0; i < N; ++i) {
        cumulativeProbability += calculateProbability(currentCity, i, visited);
        if (cumulativeProbability >= randomValue) {
            return i;
        }
    }

    return -1;
}

void updatePheromones(std::vector<std::vector<int>>& allRoutes, std::vector<double>& routeLengths) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            pheromoneMatrix[i][j] *= (1.0 - rho);
        }
    }

    for (int k = 0; k < numAnts; ++k) {
        double contribution = Q / routeLengths[k];
        for (size_t i = 0; i < allRoutes[k].size() - 1; ++i) {
            int from = allRoutes[k][i];
            int to = allRoutes[k][i + 1];
            pheromoneMatrix[from][to] += contribution;
        }
    }
}

double calculateRouteLength(const std::vector<int>& route) {
    double length = 0.0;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        length += distanceMatrix[route[i]][route[i + 1]];
    }
    return length;
}

int main() {
    std::vector<int> bestRoute;
    double bestRouteLength = std::numeric_limits<double>::max();
    std::vector<std::vector<int>> allRoutes(numAnts);
    std::vector<double> allRouteLengths(numAnts, 0.0);

    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        std::vector<double> routeLengths(numAnts, 0.0);

        for (int k = 0; k < numAnts; ++k) {
            std::vector<int> route;
            std::vector<bool> visited(N, false);
            int currentCity = k % N;
            route.push_back(currentCity);
            visited[currentCity] = true;

            for (int i = 1; i < N; ++i) {
                int nextCity = selectNextCity(currentCity, visited);
                route.push_back(nextCity);
                visited[nextCity] = true;
                currentCity = nextCity;
            }

            allRoutes[k] = route;
            routeLengths[k] = calculateRouteLength(route);
        }

        updatePheromones(allRoutes, routeLengths);

        for (int k = 0; k < numAnts; ++k) {
            allRouteLengths[k] = routeLengths[k];
            allRoutes[k] = allRoutes[k];
            std::cout << "Ant " << k << " Route Length: " << routeLengths[k] << "\n";
            std::cout << "Route: ";
            for (int city : allRoutes[k]) {
                std::cout << city << " ";
            }
            std::cout << "\n";

            if (routeLengths[k] < bestRouteLength) {
                bestRouteLength = routeLengths[k];
                bestRoute = allRoutes[k];
            }
        }
    }

    std::cout << "\nBest Route Length: " << bestRouteLength << "\n";
    std::cout << "Best Route: ";
    for (int city : bestRoute) {
        std::cout << city << " ";
    }
    std::cout << "\n";
}