#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <deque>

//Произодитель 
//Заблокирует мьютекс
//Если буффер заполнен перейдёт в режим сна.
//Добавит элемент в буффер
//Разблокирует мьютекс
//Уведомит потребителя


//Потребитель
//Заблокирует мьютекс
//Если буффер пуст перейдёт в режим сна.
//Возьмёт элемент из буффера
//Разблокирует мьютекс
//Уведомит производителя

std::mutex mtx;
std::deque<int> buffer;
std::condition_variable cond;
const unsigned int maxBufferSize = 10;
bool work = true;

void Producer(int val)
{
	while (val)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::unique_lock<std::mutex> locker(mtx);
		cond.wait(locker, []() {return buffer.size() < maxBufferSize; });
		buffer.push_back(val);
		std::cout << "Producer поместил в буффер: " << val << '\n';
		val--;
		locker.unlock();//Сняли блокировку с буффера
		cond.notify_one();//Освободили поток потребителя
	}
	work = false;
}

void Consumer()
{
	while (work)
	{
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::unique_lock<std::mutex> locker(mtx);
		cond.wait(locker, []() {return buffer.size() > 0; });
		int val = buffer.back();
		buffer.pop_back();
		std::cout << "Consumer взял из буффера: " << val << '\n';
		locker.unlock();//Сняли блокировку с буффера
		cond.notify_one();//Освободили поток производителя
	}
	std::cout << "Работа закончилась!\n";
}

int main()
{
	setlocale(LC_ALL, "Russian");
	std::thread t1(Producer, 100);
	std::thread t2(Consumer);

	t1.join();
	t2.join();
	
}