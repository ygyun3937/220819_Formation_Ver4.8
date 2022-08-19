#pragma once

#include  <iostream>

using namespace std;

class CirculaQuque
{
private :
	int front;
	int rear;
	int maxQueueSize = 3;

public :
	CirculaQuque();
	float* queue;
	void CreateQueue(int nSize);
	void DestroyQueue();
	bool isEmpty();
	bool isFull();
	void enQueue(float item);
	float deQueue(int nIndex);
	void GetMinMaxValue(float& fMin, float& fMax);
};
