#include <gtest/gtest.h>

#include <Components/ExtraClasses/TreeObject.h>

TEST(TreeObject, Regular) {
    ExtraClasses::TreeObject<std::string> obj;

    auto pRoot = obj.getRootNode();
    ASSERT_TRUE(pRoot != nullptr);

    pRoot->setData("Test data");
    ASSERT_EQ("Test data", pRoot->getData());
}
