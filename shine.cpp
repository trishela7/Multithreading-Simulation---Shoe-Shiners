#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
using namespace std;

// shared array of bools that tell you if seat is taken or not
bool* isSeatTaken;

// mutex to stop people from printing at the same time
mutex printLock;

void shiner_func(int num_chairs) 
{
	while(true) 
	{
		//i is current location of chair
		int i;
		// look for a open chair
		for (i = 0; i < num_chairs; i++) 
		{
			// found a person to service
			if(isSeatTaken[i]) 
			{
				printLock.lock();
				cout << "Serviced person sitting at chair " << i << endl;
				//seat is freed up
				isSeatTaken[i] = false;
				printLock.unlock();
				break;
			}
		}

		// all seats were empty since for loop didnt break early
		if(i == num_chairs)
		{
			printLock.lock();
			cout << "All chairs were empty" << endl;
			printLock.unlock();
		}

		this_thread::sleep_for (std::chrono::seconds(5));
	}
}

void chair_func(int thread_num) 
{
	while(true) 
	{
		// "take the ticket"
		printLock.lock();

		//each chair will either say I am taken or not taken 
		if(!isSeatTaken[thread_num]) 
		{
			cout << "Seat " << thread_num << " is not taken" << endl;
		} 
		else 
		{
			cout << "Seat " << thread_num << " is taken" << endl;
		}

		// "give the ticket back"
		printLock.unlock();

		//sleep_for function is inside this_thread namespace
		//seconds is in chrono namespace
		this_thread::sleep_for (std::chrono::seconds(5));
	}

	return;
}

int main()
{
	int num_chairs; 
	// Make an array of ints of size num_chairs & change type from array to thread
	cout << "How many chairs do you want this place to have?" << endl;
	cin >> num_chairs;

	thread* chairs = new thread [num_chairs];
	isSeatTaken = new bool [num_chairs];

	printLock.lock();
	cout << "Enter \"yes\" if you want to take a seat" << endl;
	printLock.unlock();


	// for every thread
	for(int i = 0; i < num_chairs; i++) 
	{ 	//initialized all values to seats not taken
		isSeatTaken[i] = false;
		chairs[i] = thread(chair_func, i);
	}

	// created a thread to represent the shoeShiner
	thread shoeShiner = thread(shiner_func, num_chairs);

	while (true) 
	{
		string answer;
		cin >> answer;

		// if they want to take a seat
		if (answer == "yes") 
		{
			int i;
			// look for an open seat
			for(i = 0; i < num_chairs; i++) 
			{	//if the seat is not taken
				if(!isSeatTaken[i]) 
				{	//set seat to being taken and loop goes back to cin 
					isSeatTaken[i] = true;
					break;
				}
			}

			// we could not find a seat since loop terminated early
			if (i == num_chairs) 
			{
				printLock.lock();
				cout << "You were not able to find a chair and you left" << endl;
				printLock.unlock();
			}
		}
	}

	// clean up all threads after they are done executing
	for(int i = 0; i < num_chairs; i++) 
	{
		chairs[i].join();
	}

	shoeShiner.join();

	return 0;
}