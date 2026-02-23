#include <gtest/gtest.h>

#include <Components/ExtraClasses/TreeObject.h>

TEST(TreeObject, RegularOperations) {
    ExtraClasses::TreeObject<std::string> obj;

    // Check root creating and getting
    auto pRoot = obj.getRootNode();
    ASSERT_TRUE(pRoot != nullptr);
    ASSERT_EQ(pRoot, obj.getRootNode());

    // Check data setting
    pRoot->setData("Test data");
    ASSERT_EQ("Test data", pRoot->getData());

    // Check node count
    pRoot->addNode("1");
    pRoot->addNode("2");
    pRoot->addNode("3");
    ASSERT_EQ(3, pRoot->getNodeCount());
    ASSERT_EQ(pRoot->getNode(1), pRoot->findNode("2"));

    // Check nodes data
    ASSERT_TRUE(pRoot->getNode(0) != nullptr);
    ASSERT_EQ("1", pRoot->getNode(0)->getData());
    ASSERT_TRUE(pRoot->getNode(1) != nullptr);
    ASSERT_EQ("2", pRoot->getNode(1)->getData());
    ASSERT_TRUE(pRoot->getNode(2) != nullptr);
    ASSERT_EQ("3", pRoot->getNode(2)->getData());

    // Check out of bounds
    ASSERT_TRUE(pRoot->getNode(2000) == nullptr);

    // Check node clearing
    pRoot->clearNodes();
    ASSERT_EQ(0, pRoot->getNodeCount());
    ASSERT_TRUE(pRoot->getNode(0) == nullptr);
}



// ============================ BASIC OPERATIONS //
TEST(TreeObject, AddingNodes) {
    ExtraClasses::TreeObject<std::string> obj;
    auto pRoot = obj.getRootNode();

    // Add by value
    pRoot->addNode("1");
    pRoot->addNode("2");
    pRoot->addNode("3");
    ASSERT_EQ("2", pRoot->getNode(1)->getData());

    // Add by index
    pRoot->clearNodes();
    pRoot->addNode(0); pRoot->getNode(0)->setData("1");
    pRoot->addNode(0); pRoot->getNode(0)->setData("2");
    pRoot->addNode(0); pRoot->getNode(0)->setData("3");
    ASSERT_EQ("1", pRoot->getNode(2)->getData());
    ASSERT_EQ("3", pRoot->getNode(0)->getData());

    // Add as object
    pRoot->clearNodes();
    auto pNode = std::make_shared<ExtraClasses::TreeNode<std::string> >();
    pNode->setData("1");
    pRoot->addNode(pNode);
    pNode = std::make_shared<ExtraClasses::TreeNode<std::string> >();
    pNode->setData("2");
    pRoot->addNode(pNode);
    pNode = std::make_shared<ExtraClasses::TreeNode<std::string> >();
    pNode->setData("3");
    pRoot->addNode(pNode);
    ASSERT_EQ("1", pRoot->getNode(0)->getData());
    ASSERT_EQ("2", pRoot->getNode(1)->getData());
    ASSERT_EQ("3", pRoot->getNode(2)->getData());
}

TEST(TreeObject, RemovingNodes) {
    ExtraClasses::TreeObject<std::string> obj;
    auto pRoot = obj.getRootNode();

    // Remove by value
    pRoot->addNode("1");
    pRoot->addNode("2");
    pRoot->addNode("3");
    ASSERT_TRUE(pRoot->removeNode("2"));
    ASSERT_EQ("3", pRoot->getNode(1)->getData());

    // Remove by index
    pRoot->clearNodes();
    pRoot->addNode("1");
    pRoot->addNode("2");
    pRoot->addNode("3");
    ASSERT_TRUE(pRoot->removeNode(1));
    ASSERT_EQ("3", pRoot->getNode(1)->getData());

    // Remove as object
    pRoot->clearNodes();
    pRoot->addNode("1");
    pRoot->addNode("2");
    pRoot->addNode("3");
    auto pSecondNode = pRoot->getNode(1);
    ASSERT_TRUE(pRoot->removeNode(pSecondNode));
    ASSERT_EQ("3", pRoot->getNode(1)->getData());
}



// ============================ RECURSION //
TEST(TreeObject, BranchMoving) {
    ExtraClasses::TreeObject<int> obj;
    auto pRoot = obj.getRootNode();
    pRoot->setData(1);

    // Setup tree base
    int curNodeNo {1}; // First node is root
    pRoot->addNode(++curNodeNo);
    pRoot->addNode(++curNodeNo);
    pRoot->getNode(1)->addNode(++curNodeNo);
    pRoot->getNode(1)->getNode(0)->addNode(++curNodeNo);

    // Move branch
    ASSERT_EQ(2, pRoot->getNodeCount());
    pRoot->getNode(1)->getNode(0)->setParent(pRoot);
    ASSERT_TRUE(pRoot->getNode(1)->getNode(0) == nullptr);
    ASSERT_EQ(3, pRoot->getNodeCount());

    // Setup other branch
    pRoot->addNode(++curNodeNo);
    pRoot->getNode(1)->addNode(++curNodeNo);

    // Check count of items
    int counter {0};
    pRoot->callRecursive([&counter](auto& pNode) -> bool {
        ++counter;
        return false;
    });
    ASSERT_EQ(curNodeNo, counter);
}


TEST(TreeObject, RecursiveSameLayer) {
    ExtraClasses::TreeObject<std::string> obj;
    auto pRoot = obj.getRootNode();

    pRoot->setData("Test data");
    ASSERT_EQ("Test data", pRoot->getData());

    pRoot->addNode("1");
    pRoot->addNode("2");
    pRoot->addNode("3");

    // Search in 0 depth
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

    // Search for root node from root node
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

    // Setup tree
    int curNodeNo {1}; // First node is root
    pRoot->addNode(++curNodeNo);
    pRoot->addNode(++curNodeNo);
    pRoot->getNode(1)->addNode(++curNodeNo);
    pRoot->getNode(1)->getNode(0)->addNode(++curNodeNo);
    pRoot->addNode(++curNodeNo);
    pRoot->getNode(1)->addNode(++curNodeNo);

    // Get total item count
    int counter {0};
    pRoot->callRecursive([&counter](auto& pNode) -> bool {
        ++counter;
        return false;
    });
    ASSERT_EQ(curNodeNo, counter);

    // Warning here is normal (it's comparison between pointers inside).
    // Otherwise does not even compile
    ASSERT_EQ(std::size_t(curNodeNo - 1), pRoot->getTotalChildCount());
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
