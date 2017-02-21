#include <gtest/gtest.h>
#include "../common.h"
#include "../queue.h"

TEST(QueueTest, StoreItems_RetrieveThem)
{
    Queue<Color,3> queue;
    queue.push(BLUE);

    ASSERT_EQ(queue.pull(), BLUE);

    queue.push(RED);
    queue.push(WHITE);
    queue.push(BLUE);

    ASSERT_EQ(queue.pull(), RED);
    ASSERT_EQ(queue.pull(), WHITE);
    ASSERT_EQ(queue.pull(), BLUE);

}

TEST(QueueTest, StoreTooMuch_LastItemLost)
{
    Queue<Color,3> queue;

    queue.push(RED);
    queue.push(WHITE);
    queue.push(BLUE);
    queue.push(WHITE); // 4.th item can not be stored

    ASSERT_EQ(queue.pull(), RED);
    ASSERT_EQ(queue.pull(), WHITE);
    ASSERT_EQ(queue.pull(), BLUE);

    // push works after previous overflow
    queue.push(RED);
    ASSERT_EQ(queue.pull(), RED);
}
