
#include <iostream>
#include <thread>
#include <mutex>
#include <ctime>
#include <queue>

class Restaurant{
   std::queue<std::string> newOrderQueue;
   std::queue<std::string> readyOrderQueue;
   std::mutex newOrderMutex;
   std::mutex readyOrderMutex;
   std::mutex print;
   int readyOrderQuantity = 0;
   std::array<std::string,5> order{"pizza", "soup", "steak", "salad", "sushi"};

public:

    void Waiter(){

        std::srand(std::time(nullptr));
        int waitTime = std::rand() % 6 + 5;
        int index = std::rand() % 5;
        std::this_thread::sleep_for(std::chrono::seconds (waitTime));
        std::unique_lock<std::mutex> ulNew(newOrderMutex);
        newOrderQueue.emplace(order.at(index));
        std::unique_lock<std::mutex> ulPr(print);
        std::cout << "New order from visitor: " << order.at(index) << std::endl;
   }
   void Cook(){
        std::string order;
        std::unique_lock<std::mutex> ulNew(newOrderMutex);
        if(!newOrderQueue.empty()){
            order = newOrderQueue.front();
            newOrderQueue.pop();
            std::unique_lock<std::mutex> ulPr(print);
            std::cout << "The kitchen accepted the order: " << order << std::endl;
            ulPr.unlock();
            ulNew.unlock();
            std::srand(std::time(nullptr));
            int waitTime = std::rand() % 11 + 5;
            std::this_thread::sleep_for(std::chrono::seconds (waitTime));
            std::unique_lock<std::mutex> ulReady(readyOrderMutex);
            readyOrderQueue.emplace(order);
            ulPr.lock();
            std::cout << "The order: " << order << " is ready!" << std::endl;
        }
    }
    void Courier(){
        std::string order;
        std::this_thread::sleep_for(std::chrono::seconds (30));
        std::unique_lock<std::mutex> ulReady(readyOrderMutex);
        if(!readyOrderQueue.empty()){
            order = readyOrderQueue.front();
            readyOrderQueue.pop();
            readyOrderQuantity++;
            std::unique_lock<std::mutex> ulPr(print);
            std::cout << "The order " << order << " has been delivered!" << std::endl;
        }
    }
    int GetReadyOrderQuantity(){
        return readyOrderQuantity;
    }
};


int main() {
    Restaurant restaurant;
    std::thread waiter(&Restaurant::Waiter, std::ref(restaurant));
    std::thread cook(&Restaurant::Cook, std::ref(restaurant));
    std::thread courier(&Restaurant::Courier, std::ref(restaurant));
    waiter.join();
    cook.join();
    courier.join();
    while (restaurant.GetReadyOrderQuantity() < 10);
    return 0;
}