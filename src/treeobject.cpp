#include "treeobject.hpp"

namespace ExtraClasses
{
TreeNode::TreeNode(const TreeNodePtr& pParent) :
    m_parent {pParent} {
    
}

TreeNode::~TreeNode() {
    
}

bool TreeNode::callRecursive(const recursiveCallback_t& func) {
    if (func(shared_from_this())) {
        return true;
    }
    
    for (auto& pChild : m_children) {
        if (pChild->callRecursive(func)) {
            return true;
        }
    }
    
    return false;
}

bool TreeNode::callBackwardRecursive(const recursiveCallback_t& func) {
    if (func(shared_from_this())) {
        return true;
    }
    
    auto pParent = m_parent.lock();
    if (pParent.use_count()) {
        return pParent->callBackwardRecursive(func);
    }
    
    return false;
}

bool TreeNode::callRecursive(const recursiveConstCallback_t& func) const {
    if (func(shared_from_this())) {
        return true;
    }
    
    for (const TreeNodeConstPtr& pChild : m_children) {
        if (pChild->callRecursive(func)) {
            return true;
        }
    }
    
    return false;
}

bool TreeNode::callBackwardRecursive(const recursiveConstCallback_t& func) const {
    if (func(shared_from_this())) {
        return true;
    }
    
    const TreeNodeConstPtr pParent = m_parent.lock();
    if (pParent.use_count()) {
        return pParent->callBackwardRecursive(func);
    }
    
    return false;
}

void TreeNode::setParent(const TreeNodePtr& pParent) {
    m_parent = pParent;
}

std::size_t TreeNode::getNodeCount() const {
    return m_children.size();
}

bool TreeNode::addNode(std::size_t beforeIndex) {
    if (beforeIndex < m_children.size() ||
        beforeIndex == 0) {
        auto targetPos = m_children.begin();
        std::advance(targetPos, beforeIndex);
        m_children.insert(targetPos, std::make_shared<TreeNode>(shared_from_this()));
        return true;
    }
    return false;
}

void TreeNode::addNode(const TreeNodePtr& pNode) {
    m_children.push_back(pNode);
}

bool TreeNode::removeNode(std::size_t index = -1) {
    if (index < m_children.size()) {
        auto targetPos = m_children.begin();
        std::advance(targetPos, index);
        m_children.insert(targetPos, std::make_shared<TreeNode>(shared_from_this()));
        return true;
    }
    return false;
}

void TreeNode::clearNodes() {
    m_children.clear();
}

void TreeNode::clearData() {
    setData({});
}

TreeNodePtr TreeNode::getNode(std::size_t index) const {
    if (index < m_children.size()) {
        auto targetPos = m_children.begin();
        std::advance(targetPos, index);
        return *targetPos;
    }
    return {};
}

void TreeNode::setData(const std::string& iData) {
    m_data = iData;
}

std::string TreeNode::getData() const {
    return m_data;
}


TreeObject::TreeObject() {
    m_rootNode = std::make_shared<TreeNode>();
    m_rootNode->setData("ROOT");
}

TreeNodePtr TreeObject::getRootNode() const {
    return m_rootNode;
}

}