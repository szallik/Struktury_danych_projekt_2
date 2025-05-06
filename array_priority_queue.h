#ifndef ARRAY_PRIORITY_QUEUE_H
#define ARRAY_PRIORITY_QUEUE_H

#include <unordered_map>
#include <stdexcept>
#include <utility> //do swapa

class ArrayPriorityQueue {
private:
    int* values;
    int* priorities;
    int size_;
    int capacity;
    std::unordered_map<int, int> value_to_index;
    //powieksza pojemnosc przez alokacje nowych wiekszych
    void resize() {
        int new_capacity = capacity * 2;
        int* new_values = new int[new_capacity];
        int* new_priorities = new int[new_capacity];

        for (int i = 0; i < size_; ++i) {
            new_values[i] = values[i];
            new_priorities[i] = priorities[i];
        }

        delete[] values;
        delete[] priorities;

        values = new_values;
        priorities = new_priorities;
        capacity = new_capacity;
    }

public:
    ArrayPriorityQueue(int initial_capacity = 16)
        : size_(0), capacity(initial_capacity) {
        values = new int[capacity];
        priorities = new int[capacity];
    }

    ~ArrayPriorityQueue() {
        delete[] values;
        delete[] priorities;
    }
    //wstawia nowy na koniec i dodaje do mappki
    void insert(int value, int priority) {
        if (value_to_index.count(value))
            throw std::invalid_argument("Value already exists");

        if (size_ == capacity)
            resize();

        values[size_] = value;
        priorities[size_] = priority;
        value_to_index[value] = size_;
        ++size_;
    }
    //znajduje element o najwyzszym prio i zmienia go z osotatnim
    int remove() {
        if (size_ == 0)
            throw std::out_of_range("Queue is empty");

        
        int max_index = 0;
        for (int i = 1; i < size_; ++i) {
            if (priorities[i] > priorities[max_index]) {
                max_index = i;
            }
        }

        int top_value = values[max_index];

       
        swap_entries(max_index, size_ - 1);
        --size_;
        value_to_index.erase(top_value);

        return top_value;
    }
    //przeszukuje zeby znalezc max prio
    int peek() const {
        if (size_ == 0)
            throw std::out_of_range("Queue is empty");

        int max_index = 0;
        for (int i = 1; i < size_; ++i) {
            if (priorities[i] > priorities[max_index]) {
                max_index = i;
            }
        }

        return values[max_index];
    }
    //zmiana prio indeks pobiera z mapy
    void change_priority(int value, int new_priority) {
        auto it = value_to_index.find(value);
        if (it == value_to_index.end())
            throw std::invalid_argument("Value not found");

        int index = it->second;
        priorities[index] = new_priority;
    }

    int size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

private:
    void swap_entries(int i, int j) {
        std::swap(values[i], values[j]);
        std::swap(priorities[i], priorities[j]);
        value_to_index[values[i]] = i;
        value_to_index[values[j]] = j;
    }
};

#endif // ARRAY_PRIORITY_QUEUE_H
