#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <random>
#include <algorithm>
#include "heap_priority_queue.h"
#include "array_priority_queue.h"

// Struktura pary danych wejściowych (value, priority)
struct Pair {
    int value;
    int priority;
};

// Wczytuje dane z pliku
std::vector<Pair> load_input(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<Pair> data;
    int value, priority;
    while (file >> value >> priority) {
        data.push_back({value, priority});
    }
    return data;
}

// Mierzy czas wykonania operacji i zwraca czas w mikrosekundach
template<typename Func>
long long measure_time(Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

// Benchmark dla jednej struktury, jednego rozmiaru i jednego pliku
template<typename Queue>
void benchmark_single_structure(const std::string& structure_name,
                                const std::vector<Pair>& all_data,
                                int n,
                                int file_id,
                                std::ofstream& output,
                                std::mt19937& rng) {
    const int REPEATS = 100;

    // Budujemy bazową strukturę z n elementów
    Queue pq;
    for (int i = 0; i < n; ++i)
        pq.insert(all_data[i].value, all_data[i].priority);

    // 1. INSERT (na kopii, 100 razy)
    std::uniform_int_distribution<int> priority_dist(0, 499999);
    int new_priority = priority_dist(rng);
    int new_value = 1'000'000; // Bezpieczna, unikalna wartość spoza danych

    long long total_insert_time = 0;
    for (int i = 0; i < REPEATS; ++i) {
        Queue copy = pq;
        total_insert_time += measure_time([&]() {
            copy.insert(new_value, new_priority);
        });
    }
    output << structure_name << "," << file_id << "," << n << ",insert," << REPEATS << "," << total_insert_time / REPEATS << "\n";

    // 2. SIZE (na kopii, 100 razy)
    long long total_size_time = 0;
    for (int i = 0; i < REPEATS; ++i) {
        Queue copy = pq;
        total_size_time += measure_time([&]() {
            volatile int s = copy.size();
            (void)s;
        });
    }
    output << structure_name << "," << file_id << "," << n << ",size," << REPEATS << "," << total_size_time / REPEATS << "\n";

    // 3. PEEK (na kopii, 100 razy)
    long long total_peek_time = 0;
    for (int i = 0; i < REPEATS; ++i) {
        Queue copy = pq;
        total_peek_time += measure_time([&]() {
            volatile int top = copy.peek();
            (void)top;
        });
    }
    output << structure_name << "," << file_id << "," << n << ",peek," << REPEATS << "," << total_peek_time / REPEATS << "\n";

    // 4. CHANGE_PRIORITY (na kopii, 100 razy)
    std::uniform_int_distribution<int> index_dist(0, n - 1);
    int index = index_dist(rng);
    int value_to_change = all_data[index].value;
    int new_priority_change = priority_dist(rng);

    long long total_change_time = 0;
    for (int i = 0; i < REPEATS; ++i) {
        Queue copy = pq;
        total_change_time += measure_time([&]() {
            copy.change_priority(value_to_change, new_priority_change);
        });
    }
    output << structure_name << "," << file_id << "," << n << ",change_priority," << REPEATS << "," << total_change_time / REPEATS << "\n";

    // 5. REMOVE (na kopii, 100 razy)
    long long total_remove_time = 0;
    for (int i = 0; i < REPEATS; ++i) {
        Queue copy = pq;
        total_remove_time += measure_time([&]() {
            copy.remove();
        });
    }
    output << structure_name << "," << file_id << "," << n << ",remove," << REPEATS << "," << total_remove_time / REPEATS << "\n";
}

int main() {
    std::vector<int> sizes = {1000, 10000, 50000, 100000, 200000, 300000, 400000, 500000};
    std::ofstream output("results5.csv");
    output << "structure,file_id,n,operation,repeats,time_microseconds\n";

    std::mt19937 rng(12345); // Stały seed dla powtarzalności

    for (int file_id = 0; file_id < 10; ++file_id) {
        std::ostringstream filename;
        filename << "struktury_danych_projekt_2_dane/dane_" << file_id << ".txt";

        std::cout << "Processing: " << filename.str() << std::endl;

        std::vector<Pair> all_data = load_input(filename.str());

        for (int n : sizes) {
            std::cout << "  Benchmarking n = " << n << "..." << std::endl;

            benchmark_single_structure<HeapPriorityQueue>("Heap", all_data, n, file_id, output, rng);
            benchmark_single_structure<ArrayPriorityQueue>("Array", all_data, n, file_id, output, rng);
        }
    }

    output.close();
    std::cout << "Benchmark complete. Results saved to results.csv\n";
    return 0;
}
