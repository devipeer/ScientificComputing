#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <mutex>
#include <array>
#include <atomic>
#include <vector>
#include <chrono>

// Piotr Pawe³ko SC meeting 1

using namespace std;
mutex mutex1;
static int CounterValue;
static std::atomic<int> AtomicCounterValue;

void GlobalFunction(string str) {
	cout << str << " - " << this_thread::get_id() << "\n";
}


class FunctionObject {
public:
	void operator()(string str) {
		cout << str << " - " << this_thread::get_id() << "\n";
	}
};

class ClassMethod {
public:
	void Method(string str) {
		cout << str << " - " << this_thread::get_id() << "\n";
	}
};

void FirstTask() {
	auto LambdaThread = [](string str) {
		cout << str << " - " << this_thread::get_id() << "\n";
	};

	thread th1(GlobalFunction, "Global Function");
	thread th2(FunctionObject(), "Function Object");
	thread th3(&ClassMethod::Method, ClassMethod{}, "Class Method");
	thread th4(LambdaThread, "Lambda thread");

	th1.join();
	th2.join();
	th3.join();
	th4.join();
	cout << "\n";
}

void TextPrint(string str) {
	mutex1.lock();
	for (int i = 0; i < 50; i++){
		cout << str;
	}
	cout << " ID: " << this_thread::get_id() << "\n";
	mutex1.unlock();
}

void SecondTask() {
	array<thread, 20> threads;
	for (int i = 0; i < 20; i++) {
		threads[i] = thread{ TextPrint, "$" };
	}

	for (int i = 0; i < 20; i++)
		threads[i].join();
	cout << "\n";
}

void SimpleCounter() {
	for (CounterValue = 0; CounterValue < 10000000; CounterValue++) {}
}

void MutexCounter() {
	for (CounterValue = 0; CounterValue < 10000000; CounterValue++) 
		std::lock_guard<std::mutex> lock(mutex1);
}

void AtomicCounter() {
	for (AtomicCounterValue = 0; CounterValue < 10000000; CounterValue++) {}
}

void CreateUnsynchronized() {
	vector<thread> UnsynchronizedThreads;
	UnsynchronizedThreads.reserve(10);
	for (int i = 0; i < 10; i++)
		UnsynchronizedThreads.emplace_back(SimpleCounter);
	for (auto& thread : UnsynchronizedThreads) {
		thread.join();
	}
}

void CreateMutexThreads() {
	vector<thread> MutexThreads;
	MutexThreads.reserve(10);
	for (int i = 0; i < 10; i++)
		MutexThreads.emplace_back(MutexCounter);
	for (auto& thread : MutexThreads) {
		thread.join();
	}
}

void CreateAtomicThreads() {
	vector<thread> AtomicThreads;
	AtomicThreads.reserve(10);
	for (int i = 0; i < 10; i++)
		AtomicThreads.emplace_back(AtomicCounter);
	for (auto& thread : AtomicThreads) {
		thread.join();
	}
}

void ThirdTask() {
	auto start = chrono::steady_clock::now();
	CreateUnsynchronized();
	auto end = std::chrono::steady_clock::now();
	chrono::duration<double> elapsed_seconds = end - start;
	cout << "elapsed time for unsynchronized threads: " << elapsed_seconds.count() << "s\n";
	CounterValue = 0;
	start = chrono::steady_clock::now();
	CreateMutexThreads();
	end = chrono::steady_clock::now();
	elapsed_seconds = end - start;
	cout << "elapsed time for mutex threads: " << elapsed_seconds.count() << "s\n";
	CounterValue = 0;
	start = chrono::steady_clock::now();
	CreateAtomicThreads();
	end = chrono::steady_clock::now();
	elapsed_seconds = end - start;
	cout << "elapsed time for atomic threads: " << elapsed_seconds.count() << "s\n";
}

int main()
{
	cout << "--------- FIRST TASK ---------" << "\n";
	FirstTask();
	cout << "--------- SECOND TASK ---------" << "\n";
	SecondTask();
	cout << "--------- THIRD TASK ---------" << "\n";
	ThirdTask();
	return 0;
}