#include <gtest/gtest.h>

#include <Components/ExtraClasses/TreeObject.h>

TEST(TreeObject, Regular) {
    ExtraClasses::TreeObject<std::string> obj;

    auto pRoot = obj.getRootNode();
    ASSERT_TRUE(pRoot != nullptr);

    pRoot->setData("Test data");
    ASSERT_EQ("Test data", pRoot->getData());

    pRoot->addNode("1");
    pRoot->addNode("2");
    pRoot->addNode("3");
    ASSERT_EQ(3, pRoot->getNodeCount());

    ASSERT_TRUE(pRoot->getNode(0) != nullptr);
    ASSERT_EQ("1", pRoot->getNode(0)->getData());
    ASSERT_TRUE(pRoot->getNode(1) != nullptr);
    ASSERT_EQ("2", pRoot->getNode(1)->getData());
    ASSERT_TRUE(pRoot->getNode(2) != nullptr);
    ASSERT_EQ("3", pRoot->getNode(2)->getData());

    pRoot->clearNodes();
    ASSERT_EQ(0, pRoot->getNodeCount());

    pRoot->clearData();
    ASSERT_TRUE(pRoot->getData().empty());
}

TEST(TreeObject, RecursiveSameLayer) {
    ExtraClasses::TreeObject<std::string> obj;
    auto pRoot = obj.getRootNode();

    pRoot->setData("Test data");
    ASSERT_EQ("Test data", pRoot->getData());

    pRoot->addNode("1");
    pRoot->addNode("2");
    pRoot->addNode("3");

    std::string foundData;
    int callCount {0};
    pRoot->callRecursive([&foundData, &callCount](auto& pNode) -> bool {
        ++callCount;
        if (pNode->getData() == "2") {
            foundData = "2";
            return true;
        }
        return false;
    });
    ASSERT_EQ("2", foundData);
    ASSERT_EQ(3, callCount);

    foundData.clear();
    callCount = 0;
    pRoot->callRecursive([&foundData, &callCount](auto& pNode) -> bool {
        ++callCount;
        if (pNode->getData() == "Test data") {
            foundData = "Test data";
            return true;
        }
        return false;
    });
    ASSERT_EQ("Test data", foundData);
    ASSERT_EQ(1, callCount);
}

TEST(TreeObject, RecursiveDown) {
    ExtraClasses::TreeObject<int> obj;
    auto pRoot = obj.getRootNode();
    pRoot->setData(1);

    int curNodeNo {1}; // First node is root
    pRoot->addNode(++curNodeNo);
    pRoot->addNode(++curNodeNo);
    pRoot->getNode(1)->addNode(++curNodeNo);
    pRoot->getNode(1)->getNode(0)->addNode(++curNodeNo);
    pRoot->addNode(++curNodeNo);
    pRoot->getNode(1)->addNode(++curNodeNo);

    int counter {0};
    pRoot->callRecursive([&counter](auto& pNode) -> bool {
        ++counter;
        return false;
    });
    ASSERT_EQ(curNodeNo, counter);
}

TEST(TreeObject, RecursiveUp) {
    ExtraClasses::TreeObject<int> obj;
    auto pRoot = obj.getRootNode();
    pRoot->setData(1);

    int curNodeNo {1};
    pRoot->addNode(++curNodeNo);
    pRoot->getNode(0)->addNode(++curNodeNo);
    pRoot->getNode(0)->getNode(0)->addNode(++curNodeNo);

    auto pDepthNode = pRoot->getNode(0)->getNode(0)->getNode(0);
    pDepthNode->addNode(++curNodeNo);
    pDepthNode = pDepthNode->getNode(0);

    int depth {0};
    pDepthNode->callBackwardRecursive([&depth](auto& pNode) -> bool {
        ++depth;
        return false;
    });
    ASSERT_EQ(5, depth);
    ASSERT_EQ(0, pRoot->getDepth());
    ASSERT_EQ(4, pDepthNode->getDepth());
}
