#include <iostream>
#include <windows.h>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

int arr_size;
int count;
int threads_number;
bool* st_arr;
int end_n;

std::vector<int>arr;

std::mutex mute;
std::condition_variable cond;

bool Paused(bool arr[], int n) {
    for (int i = 0; i < n; i++) {
        if (arr[i])
            return false;
    }
    return true;
}

void Thread_func(int x) {
    srand(x);
    int r = rand() % arr_size;
    std::vector<int> m;
    while (true) {
        std::unique_lock<std::mutex> ul(mute);
        if ((end_n) == x) {
            st_arr[x] = false;
            for (int i : m) {
                arr[i] = 0;
            }
            threads_number--;
            cond.notify_all();
            ul.unlock();
            return;
        }
        if (arr[r] == 0) {
            Sleep(5);
            arr[r] = x + 1;
            for (int i : m) if (i == r)
            {
                m.push_back(r);
                break;
            }
            Sleep(5);
        }
        else {
            st_arr[x] = false;
            for (int i : m) {
                std::cout << i << " ";
            }
            std::cout << std::endl << arr_size << std::endl;
            cond.notify_all();
            cond.wait(ul, [=]() {return st_arr[x]; });
        }
        ul.unlock();
    }
}
int main() {
    std::cout << "Enter size of array ";
    std::cin >> arr_size;
    arr.resize(arr_size);
    for (int i = 0; i < arr_size; i++) {
        arr[i] = 0;
    }
    std::cout << "Enter number marker ";
    std::cin >> threads_number;
    count = threads_number;
    std::vector<std::thread>th;
    end_n = -1;
    st_arr = new bool[threads_number];
    for (int i = 0; i < threads_number; i++) {
        //func in thread
        th.emplace_back(&Thread_func, i);
        st_arr[i] = true;
    }
    for (int i = 0; i < threads_number; i++) {
        th[i].detach();
    }
    std::vector<int>endedThreads;
    while (threads_number > 0) {
        std::unique_lock<std::mutex> ul(mute);
        cond.wait(ul, [=]() {return Paused(st_arr, count); });
        std::cout << "array: ";
        for (int i : arr) {
            std::cout << i << " ";
        }
        std::cout << std::endl << "Enter number of thread" << std::endl;
        int number;
        std::cin >> number;
        if (number >= th.size())
            number = 0;
        endedThreads.push_back(number);
        st_arr[number] = true;
        end_n = number;
        cond.notify_all();
        cond.wait(ul, [=]() {return Paused(st_arr, count); });
        for (int i = 0; i < threads_number; i++) {
            for (int b : endedThreads) if (b == i)
            {
                st_arr[i] = true;
                break;
            }
        }
        cond.notify_all();
        ul.unlock();
    }
    return 0;
}