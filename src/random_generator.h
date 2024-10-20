#pragma once
#include <random>
#include <ctime>

class RandomGenerator {
private:
    std::mt19937 engine; // Static Mersenne Twister engine

public:
    RandomGenerator() {
        std::random_device rd;
        engine.seed(rd());
    }

    int get_random_int(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(this->engine);
    }

    double get_random_double(double min, double max) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(this->engine);
    }

    bool get_random_bool() {
        std::uniform_int_distribution<int> dist(0, 1);
        return dist(this->engine) != 0;
    }
};
