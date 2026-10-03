// Зчитування та перевірка вхідного файлу.
#pragma once

#include <charconv>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

struct InputData {
    std::vector<long long> values;
    std::vector<std::uint64_t> freqs;
};

namespace detail {

    template <typename T>
    T parse_number(const std::string& token, const char* what) {
        T result{};
        const char* first = token.data();
        const char* last = first + token.size();
        const auto [ptr, ec] = std::from_chars(first, last, result);
        if (ec != std::errc{} || ptr != last) {
            throw std::runtime_error(std::string("invalid integer (") + what + "): '" + token + "'");
        }
        return result;
    }

}  // namespace detail

inline InputData read_input(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("Unable to open the file '" + path + "'");
    }
    std::string token;
    if (!(in >> token)) {
        throw std::runtime_error("The file is empty");
    }
    const auto n = detail::parse_number<long long>(token, "n");
    if (n <= 0) {
        throw std::runtime_error("n must be a natural number");
    }
    if (n > 1'000'000) {
        throw std::runtime_error("n too large (maximum 1,000,000)");
    }

    InputData data;
    for (long long i = 0; i < n; ++i) {
        if (!(in >> token)) {
            throw std::runtime_error("There are not enough numbers in the file");
        }
        data.values.push_back(detail::parse_number<long long>(token, "number"));
    }
    for (long long i = 0; i < n; ++i) {
        if (!(in >> token)) {
            throw std::runtime_error("There are not enough frequencies in the file");
        }
        const auto f = detail::parse_number<long long>(token, "frequency");
        if (f <= 0) {
            throw std::runtime_error("The frequency must be a natural number: '" + token + "'");
        }
        data.freqs.push_back(static_cast<std::uint64_t>(f));
    }
    if (in >> token) {
        throw std::runtime_error("Extra data at the end of the file: '" + token + "'");
    }
    return data;
}