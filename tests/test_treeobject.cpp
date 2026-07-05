#include "containers/treeobject.hpp"
#include <gtest/gtest.h>

#include <Components/ExtraClasses/Containers/TreeObject.h>

using Node_t = ExtraClasses::TreeNode<std::string>;
using NodePtr_t = ExtraClasses::TreeNodePtr<std::string>;

TEST(TreeNodePtr, RegularOperations) {
    NodePtr_t pRoot = Node_t::create("");

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
TEST(TreeNodePtr, AddingNodes) {
    NodePtr_t pRoot = Node_t::create("");

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
    auto pNode = Node_t::create("");
    pNode->setData("1");
    pRoot->addNode(pNode);
    pNode = Node_t::create("");
    pNode->setData("2");
    pRoot->addNode(pNode);
    pNode = Node_t::create("");
    pNode->setData("3");
    pRoot->addNode(pNode);
    ASSERT_EQ("1", pRoot->getNode(0)->getData());
    ASSERT_EQ("2", pRoot->getNode(1)->getData());
    ASSERT_EQ("3", pRoot->getNode(2)->getData());
}

TEST(TreeNodePtr, RemovingNodes) {
    NodePtr_t pRoot = Node_t::create("");

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
TEST(TreeNodePtr, BranchMoving) {
    ExtraClasses::TreeNodePtr<int> pRoot = ExtraClasses::TreeNode<int>::create(0);
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


TEST(TreeNodePtr, RecursiveSameLayer) {
    NodePtr_t pRoot = Node_t::create("");

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


TEST(TreeNodePtr, RecursiveDown) {
    ExtraClasses::TreeNodePtr<int> pRoot = ExtraClasses::TreeNode<int>::create(0);
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


TEST(TreeNodePtr, RecursiveUp) {
    ExtraClasses::TreeNodePtr<int> pRoot = ExtraClasses::TreeNode<int>::create(0);
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
