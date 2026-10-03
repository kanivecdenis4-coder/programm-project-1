// Клас функціональних об'єктів для генерації випадкових чисел із заданими частотами.
#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <vector>

class WeightedRandom {
public:
    // values[i] -- число, freqs[i] -- його частота (натуральне число).
    WeightedRandom(const std::vector<long long>& values,
        const std::vector<std::uint64_t>& freqs,
        std::uint32_t seed = std::random_device{}())
        : values_(values), engine_(seed) {
        if (values.empty()) {
            throw std::invalid_argument("The sequence of numbers is empty");
        }
        if (values.size() != freqs.size()) {
            throw std::invalid_argument("The number of numbers and the number of frequencies are different");
        }
        if (std::ranges::any_of(freqs, [](std::uint64_t f) { return f == 0; })) {
            throw std::invalid_argument("Frequencies must be positive");
        }
       
        auto sorted = values;
        std::ranges::sort(sorted);
        if (std::ranges::adjacent_find(sorted) != sorted.end()) {
            throw std::invalid_argument("There are identical numbers in the sequence");
        }
        std::uint64_t sum = 0;
        for (std::uint64_t f : freqs) {
            if (f > std::numeric_limits<std::uint64_t>::max() - sum) {
                throw std::invalid_argument("The sum of the frequencies is too large");
            }
            sum += f;
        }
        // Накопичені частоти: std::plus -- бібліотечний функціональний об'єкт.
        cumulative_.resize(freqs.size());
        std::partial_sum(freqs.begin(), freqs.end(), cumulative_.begin(), std::plus<>{});
        dist_ = std::uniform_int_distribution<std::uint64_t>(1, cumulative_.back());
    }

    
    long long operator()() {
        const std::uint64_t r = dist_(engine_);
        const auto it = std::ranges::lower_bound(cumulative_, r);
        return values_[static_cast<std::size_t>(it - cumulative_.begin())];
    }

private:
    std::vector<long long> values_;
    std::vector<std::uint64_t> cumulative_;
    std::mt19937_64 engine_;
    std::uniform_int_distribution<std::uint64_t> dist_;
};