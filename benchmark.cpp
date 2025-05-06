#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <string>
#include <iomanip>
#include <filesystem>
#include <random>
#include <algorithm>

#include "heap_priority_queue.h"
#include "array_priority_queue.h"

namespace fs = std::filesystem;
using namespace std::chrono;

struct Pair {
    int value;
    int priority;
};
//wczytuje pary wartosc priorytet z wejsciowego
std::vector<Pair> load_input(const std::string& filename) {
    std::ifstream in(filename);
    std::vector<Pair> data;
    int value, priority;
    while (in >> value >> priority) {
        data.push_back({value, priority});
    }
    return data;
}
//mierzymy czasy samych operacji
//po jednej size i peek, wybor 10 do zmiany priorytetu i usuwanie 10 elementow 

template <typename Queue>
void run_benchmark(const std::string& struct_name,
                   const std::string& filename,
                   const std::vector<Pair>& all_data,
                   const std::vector<int>& sizes,
                   std::ofstream& csv_out) {
    std::cout << "  Running benchmarks for structure: " << struct_name << std::endl;

    for (int n : sizes) {
        if (n > all_data.size()) continue;

        std::cout << "    Size n = " << n << " ... " << std::flush;

        std::vector<Pair> data(all_data.begin(), all_data.begin() + n);
        Queue pq;

        // Insert
        auto start = high_resolution_clock::now();
        for (const auto& p : data)
            pq.insert(p.value, p.priority);
        auto end = high_resolution_clock::now();
        csv_out << filename << "," << struct_name << "," << n << ",insert,"
                << duration_cast<microseconds>(end - start).count() << "\n";

        // Size
        start = high_resolution_clock::now();
        volatile int s = pq.size();
        end = high_resolution_clock::now();
        csv_out << filename << "," << struct_name << "," << n << ",size,"
                << duration_cast<microseconds>(end - start).count() << "\n";

        // Peek
        start = high_resolution_clock::now();
        volatile int top = pq.peek();
        end = high_resolution_clock::now();
        csv_out << filename << "," << struct_name << "," << n << ",peek,"
                << duration_cast<microseconds>(end - start).count() << "\n";


        std::vector<int> targets;
        for (int i = 0; i < 10 && i < n; ++i) {
            targets.push_back(data[i * (n / 10)].value);
        }


        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(0, 499999);
        std::vector<int> new_priorities;
        for (int i = 0; i < targets.size(); ++i)
            new_priorities.push_back(dist(rng));


        start = high_resolution_clock::now();
        for (size_t i = 0; i < targets.size(); ++i)
            pq.change_priority(targets[i], new_priorities[i]);
        end = high_resolution_clock::now();
        csv_out << filename << "," << struct_name << "," << n << ",change_priority,"
                << duration_cast<microseconds>(end - start).count() << "\n";


        start = high_resolution_clock::now();
        for (int i = 0; i < 10 && !pq.empty(); ++i)
            pq.remove();
        end = high_resolution_clock::now();
        csv_out << filename << "," << struct_name << "," << n << ",remove,"
                << duration_cast<microseconds>(end - start).count() << "\n";

        std::cout << "done" << std::endl;
    }

    std::cout << "  Finished benchmarks for: " << struct_name << std::endl;
}
//tutaj iterujemy po wszystkich plikach tekstowych i uruchamiamy benchmarki ktore z kolei iteruja po rozmiarach
int main() {

    std::vector<int> sizes = {1000, 10000, 50000, 100000, 200000, 300000, 400000, 500000};

    std::string input_dir = "C:\\Users\\admin\\PycharmProjects\\struktury_danych_projekt_2_dane\\struktury_danych_projekt_2_dane";         // input data directory
    std::string output_file = "results1.csv";

    std::ofstream csv_out(output_file);
    csv_out << "file,structure,n,operation,time_microseconds\n";

    int file_counter = 0;


    std::vector<fs::directory_entry> files;
    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (entry.is_regular_file())
            files.push_back(entry);
    }
    std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
        return a.path().filename() < b.path().filename();
    });


    for (const auto& entry : files) {
        std::string filepath = entry.path().string();
        std::string filename = entry.path().filename().string();

        std::cout << "\nProcessing file: " << filename
                  << " (" << file_counter + 1 << "/" << files.size() << ")" << std::endl;

        std::vector<Pair> data = load_input(filepath);

        run_benchmark<HeapPriorityQueue>("Heap", filename, data, sizes, csv_out);
        run_benchmark<ArrayPriorityQueue>("Array", filename, data, sizes, csv_out);

        std::cout << "Completed file: " << filename << std::endl;
        ++file_counter;
    }

    std::cout << "\nAll benchmarks completed." << std::endl;
    std::cout << "Results saved to: " << output_file << std::endl;

    return 0;
}
