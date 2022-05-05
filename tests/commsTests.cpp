//
// Created by kr63454 on 1/31/2022.
//
#include "gtest/gtest.h"
#include "machinehandle.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);
}

TEST(MachHandle, XmlInput_model_serial_number) {
    // Expect two strings not to be equal.
    auto m = MachineHandle("templatetest.xml");
    EXPECT_STREQ("ADT", m.model.c_str());
    // Expect equality.
    EXPECT_EQ(123, m.serial_number);
}

TEST(MachHandle, XmlInput_messages) {
    // Expect two strings not to be equal.
    auto m = MachineHandle("templatetest.xml");
    EXPECT_EQ(5, m.publish_messages.size());
    EXPECT_EQ(5, m.subscription_messages.size());
    EXPECT_TRUE(std::find(m.publish_messages.begin(), m.publish_messages.end(), "Model") != m.publish_messages.end());
    EXPECT_TRUE(std::find(m.subscription_messages.begin(), m.subscription_messages.end(), "Model") != m.subscription_messages.end());
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

