#include "stdafx.h"
#include "CircularQueue.h"

CirculaQuque::CirculaQuque()
{
	front = 0;
	rear = 0;
}

bool CirculaQuque::isEmpty()
{
	if (front == rear)
		return true;
	else
		return false;
}

void  CirculaQuque::CreateQueue(int nSize)
{
	queue = (float*)malloc(sizeof(float) * nSize);
}

void  CirculaQuque::DestroyQueue()
{
	free(queue);
}

bool CirculaQuque::isFull()
{
	if (rear == (front + 1) % (_msize(queue) / sizeof(float)))
		return true;
	else
		return false;
}

void CirculaQuque::enQueue(float item)
{
	if (rear == (front + 1) % (_msize(queue) / sizeof(float)))
		rear = ++rear%(_msize(queue) / sizeof(float));

	front = ++front%(_msize(queue) / sizeof(float));

	if (front == (_msize(queue) / sizeof(float)))
		front = 0;
	if (rear == (_msize(queue) / sizeof(float)))
		rear = 0;

	queue[front] = item;
}

float CirculaQuque::deQueue(int nIndex)
{
	return queue[((_msize(queue) / sizeof(float))+front-nIndex)%(_msize(queue) / sizeof(float))];
}

void CirculaQuque::GetMinMaxValue(float& fMin, float& fMax)
{
	float fMinValue = 0;
	float fMaxValue = 0;
	for (int n = 0; n < (_msize(queue) / sizeof(float)); n++)
	{
		if (fMinValue == 0)
			fMinValue = queue[n];
		else
		{
			if (queue[n] < fMinValue)
				fMinValue = queue[n];
		}

		if (fMaxValue == 0)
			fMaxValue = queue[n];
		else
		{
			if (queue[n] > fMaxValue)
				fMaxValue = queue[n];
		}
	}

	fMin = fMinValue;
	fMax = fMaxValue;
}