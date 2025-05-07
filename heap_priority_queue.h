#ifndef HEAP_PRIORITY_QUEUE_H
#define HEAP_PRIORITY_QUEUE_H

#include <unordered_map>
#include <stdexcept>
#include <utility> // do swapa

class HeapPriorityQueue {
private:
    int* values;
    int* priorities;
    int size_;
    int capacity;
    std::unordered_map<int, int> value_to_index;

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

    void swap(int i, int j) {
        std::swap(values[i], values[j]);
        std::swap(priorities[i], priorities[j]);
        value_to_index[values[i]] = i;
        value_to_index[values[j]] = j;
    }

    void heapify_up(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (priorities[index] > priorities[parent]) {
                swap(index, parent);
                index = parent;
            } else {
                break;
            }
        }
    }

    void heapify_down(int index) {
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < size_ && priorities[left] > priorities[largest])
                largest = left;
            if (right < size_ && priorities[right] > priorities[largest])
                largest = right;

            if (largest != index) {
                swap(index, largest);
                index = largest;
            } else {
                break;
            }
        }
    }

public:
    HeapPriorityQueue(const HeapPriorityQueue& other)
    : size_(other.size_), capacity(other.capacity), value_to_index(other.value_to_index) {
        values = new int[capacity];
        priorities = new int[capacity];
        for (int i = 0; i < size_; ++i) {
            values[i] = other.values[i];
            priorities[i] = other.priorities[i];
        }
    }
    HeapPriorityQueue(int initial_capacity = 16)
        : size_(0), capacity(initial_capacity) {
        values = new int[capacity];
        priorities = new int[capacity];
    }

    ~HeapPriorityQueue() {
        delete[] values;
        delete[] priorities;
    }

    void insert(int value, int priority) {
        if (value_to_index.count(value))
            throw std::invalid_argument("Value already exists");

        if (size_ == capacity)
            resize();

        values[size_] = value;
        priorities[size_] = priority;
        value_to_index[value] = size_;
        heapify_up(size_);
        ++size_;
    }

    int remove() {
        if (size_ == 0)
            throw std::out_of_range("Queue is empty");

        int top_value = values[0];
        swap(0, size_ - 1);
        --size_;
        value_to_index.erase(top_value);
        heapify_down(0);
        return top_value;
    }

    int peek() const {
        if (size_ == 0)
            throw std::out_of_range("Queue is empty");
        return values[0];
    }

    void change_priority(int value, int new_priority) {
        auto it = value_to_index.find(value);
        if (it == value_to_index.end())
            throw std::invalid_argument("Value not found");

        int index = it->second;
        int old_priority = priorities[index];
        priorities[index] = new_priority;

        if (new_priority > old_priority)
            heapify_up(index);
        else
            heapify_down(index);
    }

    int size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }
};

#endif // HEAP_PRIORITY_QUEUE_H
