// std:c++latest
// programm project 1

#include <algorithm>
#include <cmath>
#include <compare>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <vector>

#include "reader.hpp"
#include "Random.hpp"

constexpr std::uint64_t kTrials = 1'000'000;

struct Row {
    long long value;
    double given;     // задана частота (відносна)
    double obtained;  // отримана частота (відносна)

    double deviation() const { return std::abs(given - obtained); }

    std::strong_ordering operator<=>(const Row& other) const { return value <=> other.value; }
    bool operator==(const Row& other) const { return value == other.value; }
};

std::string make_report(const InputData& data, const std::vector<std::uint64_t>& counts) {
    // Сума частот: std::plus -- бібліотечний функціональний об'єкт.
    const double total_freq = static_cast<double>(
        std::accumulate(data.freqs.begin(), data.freqs.end(), std::uint64_t{ 0 }, std::plus<>{}));

    std::vector<Row> rows;
    for (std::size_t i = 0; i < data.values.size(); ++i) {
        rows.push_back({ data.values[i],
                        static_cast<double>(data.freqs[i]) / total_freq,
                        static_cast<double>(counts[i]) / static_cast<double>(kTrials) });
    }
    
    std::ranges::sort(rows, std::less<>{});

   
    const auto max_row = std::ranges::max_element(
        rows, [](const Row& a, const Row& b) { return a.deviation() < b.deviation(); });

    std::ostringstream out;
    out << std::fixed << std::setprecision(6);
    out << "Numbers generated: " << kTrials << "\n\n";
    
    out << "       Number          Specified        Obtained     Discrepancy\n";
    for (const Row& r : rows) {
        out << std::setw(12) << r.value << std::setw(16) << r.given << std::setw(16) << r.obtained
            << std::setw(16) << r.deviation() << "\n";
    }
    out << "\nThe largest frequency discrepancy: " << max_row->deviation() << " (number " << max_row->value
        << ")\n";
    return out.str();
}

int main(int argc, char* argv[]) {
    const std::string input_path = argc > 1 ? argv[1] : "input.txt";
    const std::string output_path = argc > 2 ? argv[2] : "output.txt";

    try {
        const InputData data = read_input(input_path);
        WeightedRandom generator(data.values, data.freqs);

        // Лічильники кожного числа (індекс за значенням).
        std::map<long long, std::size_t> index;
        for (std::size_t i = 0; i < data.values.size(); ++i) {
            index[data.values[i]] = i;
        }
        std::vector<std::uint64_t> counts(data.values.size(), 0);
        for (std::uint64_t i = 0; i < kTrials; ++i) {
            ++counts[index.at(generator())];
        }

        const std::string report = make_report(data, counts);
        std::cout << report;

        std::ofstream out(output_path);
        if (!out) {
            std::cerr << "Error: Could not open the file '" << output_path << "' for recording\n";
            return 1;
        }
        out << report;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}