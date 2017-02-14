#ifndef QUEUE
#define QUEUE

template <class T, int N>
class Queue
{
private:
    T array[N];
    int head;
    int tail;
    bool full;


public:
    Queue(): head(0), tail(0), full(false){}

    void push(T object)
    {
        if(full) return;
        array[head];
        head++;
        head %= N;
        if(head == tail)
        {
            full = true;
        }
    }

    T pull()
    {
        T result;

        if((tail != head) || full)
        {
            result = array[tail];
            tail++;
            tail %= N;
            full = false;
        }
        return result;
    }

    bool isEmpty()
    {
        return ((head == tail) && (!full));
    }

};


#endif // QUEUE

