#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
struct Node
{
	int value;
	Node* next;
	std::mutex* node_mutex;
    Node(int val) : value(val), next(nullptr), node_mutex(new std::mutex()) {}
};
class FineGrainedQueue
{
public:
    FineGrainedQueue(Node* node) : head(node), queue_mutex(new std::mutex()) {}
    void insertIntoMiddle(int value, int pos)
    {
        Node* newNode = new Node(value);
        std::lock_guard<std::mutex> lock(*queue_mutex);
        Node* current = head;
        int currentPos = 0;
        while (current->next != nullptr && currentPos < pos - 1)
        {
            current = current->next;
            currentPos++;
        }
        std::unique_lock<std::mutex> currentLock(*current->node_mutex, std::defer_lock);
        if (current->next != nullptr)
        {
            std::unique_lock<std::mutex> nextLock(*current->next->node_mutex, std::defer_lock);
        }
        if (pos == 0)
        {
            newNode->next =head;
            head=newNode;
        }
        else 
        {
            newNode->next = current->next;
            current->next = newNode;
        }
    }

    void printQueue()
    {
        std::lock_guard<std::mutex> lock(*queue_mutex);
        Node* current = head;
        while (current != nullptr)
        {
            std::cout << current->value << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
	private:
	Node* head;
	std::mutex* queue_mutex;
};
auto main() -> int {
    Node* node = new Node(15);
    FineGrainedQueue fgq(node);
    std::thread th1(&FineGrainedQueue::insertIntoMiddle, &fgq, 1, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::thread th2(&FineGrainedQueue::insertIntoMiddle, &fgq, 2, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::thread th3(&FineGrainedQueue::insertIntoMiddle, &fgq, 3, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::thread th4(&FineGrainedQueue::insertIntoMiddle, &fgq, 4, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::thread th5(&FineGrainedQueue::insertIntoMiddle, &fgq, 5, 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::thread th6(&FineGrainedQueue::insertIntoMiddle, &fgq, 9, 10);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::thread th7(&FineGrainedQueue::insertIntoMiddle, &fgq, 11, 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();
    th6.join();
    th7.join();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::thread th8(&FineGrainedQueue::printQueue, &fgq);
    th8.join();
	return 0;
}