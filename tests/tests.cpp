
#include <gtest/gtest.h>
#include "../common.h"
#include "../queue.h"



TEST(Queue, push_and_pull)
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

TEST(Queue, push_too_much)
{
    Queue<Color,3> queue;

    queue.push(RED);
    queue.push(WHITE);
    queue.push(BLUE);
    queue.push(WHITE);

    ASSERT_EQ(queue.pull(), RED);
    ASSERT_EQ(queue.pull(), WHITE);
    ASSERT_EQ(queue.pull(), BLUE);

    // push works after previous overflow
    queue.push(RED);
    ASSERT_EQ(queue.pull(), RED);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
