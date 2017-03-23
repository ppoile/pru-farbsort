#ifndef OBJECT_POOL
#define OBJECT_POOL

#endif // OBJECT_POOL

template <class T, int N>
class ObjectPool
{
private:
    T array[N];
    bool inUse[N];

public:
    ObjectPool()
    {
        for(int i = 0; i < N; i++)
        {
            inUse[i] = false;
        }
    }

    T* getObject()
    {
        for(int i = 0; i < N; i++)
        {
            if(inUse[i] == false)
            {
                inUse[i] = true;
                return &array[i];
            }
        }
        return 0;
    }

    void free(T* object)
    {
        for(int i = 0; i < N; i++)
        {
            if(object == &array[i])
            {
                inUse[i] =false;
                break;
            }
        }
    }

};
