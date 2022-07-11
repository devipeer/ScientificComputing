#include <iostream>
#include <algorithm>
#include <thread>
#include <vector>
#include <array>
#include <mutex>
#include <chrono>
#include <iterator>
#include <queue>
#include <numeric>


using namespace std;

#define ARRSIZE 100000

class Queue
{
public:
	vector<vector<int>> queue;
	int q_size;
	bool isProducing = 0;
	mutex mtx;
	
		Queue(int size) {
			q_size = size;
	}
		void push(vector<int> array) {
			if(!isFull() || isEmpty()) {
			//unique_lock<mutex> lck(mtx);
			queue.push_back(array);
			}
	}
		vector<int> pop() {
			if(!isEmpty()){
				//unique_lock<mutex> lck(mtx);
				vector<int> element;
				element = queue.at(0);
				queue.erase(queue.begin());
				return element;
			}
		}

		bool isEmpty() {
			return queue.empty();
	}
		bool isFull() {
			if (queue.size() >= q_size)
				return true;
			else
				return false;
		}
};

class Producer
{
	Queue& queue;
	int arrays = 0;
	int counter = 0;
public:
	Producer(Queue& queue, int no_arrays) : queue(queue) {
		arrays = no_arrays;
		queue.isProducing = 1;
	}
	vector<int> create_arr(int size) {
		vector<int> array;
		for (int i = 0; i < size; i++) {
			array.push_back(rand());
		}
		//cout << "(Producer) Checksum: " << accumulate(array.begin(), array.end(), 0) << endl;
		return array;
	}
	void push_array() {
		while (counter < arrays) {
			if (!queue.isFull()) {
				unique_lock<mutex> lck(queue.mtx);
				queue.push(create_arr(ARRSIZE));
				counter++;
				//cout << "(Producer) Counter: " << counter << endl;
			}
			queue.isProducing = 1;
		}
		queue.isProducing = 0;
		if (queue.isFull())
			cout << "Producer: Yield(Queue Full!)" << endl;
		if (queue.isEmpty())
			cout << "Producer: Yield(Queue Empty!)" << endl;
	}
};

class Consumer
{
public:
	Queue& queue;
	int counter = 0;
	int cons;
	Consumer(Queue& queue, int consumer_n) : queue(queue) {
		cons = consumer_n;
	}
	void sort_array() {
		vector<int> popped;
		while (!queue.isEmpty() || queue.isProducing) {
			if (!queue.isEmpty()) {
				unique_lock<mutex> lck(queue.mtx);
				popped = queue.pop();
			}
			if (!popped.empty()) {
				sort(popped.begin(), popped.end());
				counter++;
				//cout << "(Consumer " << cons << ")" << " Counter: " << counter << endl;
				//cout << "(Consumer " << cons << ")" << " Checksum: " << accumulate(popped.begin(), popped.end(), 0) << " Counter: " << counter << endl;
			}
		}
		unique_lock<mutex> lck(queue.mtx);
		if (queue.isFull())
			cout << "(Consumer " << cons << ")" << ": Yield(Queue Full!)" << endl;
		if (queue.isEmpty())
			cout << "(Consumer " << cons << ")" << ": Yield(Queue Empty!)" << endl;
		cout << "(Consumer " << cons << ")" << " Counter: " << counter << endl;
	}
};


void print(std::vector<int> const& input)
{
	for (int i = 0; i < input.size(); i++) {
		std::cout << input.at(i) << ' ';
	}
}


int main()
{
	int size_of_queue = 1000;
	int no_arrays;

	cout << "Arrays to produce: ";
	cin >> no_arrays;

	Queue queue(size_of_queue);
	auto start = chrono::system_clock::now();
	thread Producer1(&Producer::push_array, Producer(ref(queue), no_arrays));

	vector<thread> Consumers;
	thread Consumer1(&Consumer::sort_array, Consumer(ref(queue), 1));
	thread Consumer2(&Consumer::sort_array, Consumer(ref(queue), 2));
	thread Consumer3(&Consumer::sort_array, Consumer(ref(queue), 3));
	thread Consumer4(&Consumer::sort_array, Consumer(ref(queue), 4));
	thread Consumer5(&Consumer::sort_array, Consumer(ref(queue), 5));
	thread Consumer6(&Consumer::sort_array, Consumer(ref(queue), 6));
	thread Consumer7(&Consumer::sort_array, Consumer(ref(queue), 7));
	thread Consumer8(&Consumer::sort_array, Consumer(ref(queue), 8));

	Producer1.join();
	Consumer1.join();
	Consumer2.join();
	Consumer3.join();
	Consumer4.join();
	Consumer5.join();
	Consumer6.join();
	Consumer7.join();
	Consumer8.join();

	auto end = chrono::system_clock::now();
	auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
	cout << "Time: " << elapsed.count() / 1000 << " seconds" << endl;

	cout <<"Elements in q: " << queue.queue.size() << endl;

	return 0;
}