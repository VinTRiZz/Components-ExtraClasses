#pragma once

#include "treeobject.hpp"

namespace ExtraClasses
{

template <typename DataT>
TreeNode<DataT>::TreeNode(const TreeNodePtr<DataT>& pParent) :
    m_parent {pParent} {
    
}

template <typename DataT>
bool TreeNode<DataT>::callRecursive(const recursiveCallback_t& func) {
    if (func(this->shared_from_this())) {
        return true;
    }
    
    for (auto& pChild : m_children) {
        if (pChild->callRecursive(func)) {
            return true;
        }
    }
    
    return false;
}

template <typename DataT>
bool TreeNode<DataT>::callBackwardRecursive(const recursiveCallback_t& func) {
    if (func(this->shared_from_this())) {
        return true;
    }
    
    auto pParent = m_parent.lock();
    if (pParent.use_count()) {
        return pParent->callBackwardRecursive(func);
    }
    
    return false;
}

template <typename DataT>
bool TreeNode<DataT>::callRecursive(const recursiveConstCallback_t& func) const {
    if (func(this->shared_from_this())) {
        return true;
    }
    
    for (const auto& pChild : m_children) {
        if (pChild->callRecursive(func)) {
            return true;
        }
    }
    
    return false;
}

template <typename DataT>
bool TreeNode<DataT>::callBackwardRecursive(const recursiveConstCallback_t& func) const {
    if (func(this->shared_from_this())) {
        return true;
    }
    
    const auto pParent = m_parent.lock();
    if (pParent.use_count()) {
        return pParent->callBackwardRecursive(func);
    }
    
    return false;
}

template <typename DataT>
void TreeNode<DataT>::setParent(const TreeNodePtr<DataT>& pParent) {
    m_parent = pParent;
}

template <typename DataT>
std::size_t TreeNode<DataT>::getNodeCount() const {
    return m_children.size();
}

template <typename DataT>
bool TreeNode<DataT>::addNode(std::size_t beforeIndex) {
    if (beforeIndex < m_children.size() ||
        beforeIndex == 0) {
        auto targetPos = m_children.begin();
        std::advance(targetPos, beforeIndex);
        m_children.insert(targetPos, std::make_shared<TreeNode<DataT> >(this->shared_from_this()));
        return true;
    }
    return false;
}

template <typename DataT>
void TreeNode<DataT>::addNode(const TreeNodePtr<DataT>& pNode) {
    m_children.push_back(pNode);
}

template <typename DataT>
bool TreeNode<DataT>::removeNode(std::size_t index) {
    if (index < m_children.size()) {
        auto targetPos = m_children.begin();
        std::advance(targetPos, index);
        m_children.insert(targetPos, std::make_shared<TreeNode<DataT> >(this->shared_from_this()));
        return true;
    }
    return false;
}

template <typename DataT>
void TreeNode<DataT>::clearNodes() {
    m_children.clear();
}

template <typename DataT>
void TreeNode<DataT>::clearData() {
    setData({});
}

template <typename DataT>
TreeNodePtr<DataT> TreeNode<DataT>::getNode(std::size_t index) const {
    if (index < m_children.size()) {
        auto targetPos = m_children.begin();
        std::advance(targetPos, index);
        return *targetPos;
    }
    return {};
}

template <typename DataT>
void TreeNode<DataT>::setData(const DataT& iData) {
    m_data = iData;
}

template <typename DataT>
DataT TreeNode<DataT>::getData() const {
    return m_data;
}


template <typename DataT>
TreeObject<DataT>::TreeObject() {
    m_rootNode = std::make_shared<TreeNode<DataT> >();
    m_rootNode->setData("ROOT");
}

template <typename DataT>
TreeNodePtr<DataT> TreeObject<DataT>::getRootNode() const {
    return m_rootNode;
}

}
