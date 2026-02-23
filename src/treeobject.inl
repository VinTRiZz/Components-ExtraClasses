#pragma once

#include "treeobject.hpp"

namespace ExtraClasses
{

template <typename DataT>
TreeNode<DataT>::TreeNode(const TreeNodePtr<DataT>& pParent) :
    m_parent {pParent} {
    setupAsList();
}

template <typename DataT>
bool TreeNode<DataT>::callRecursive(const recursiveCallback_t& func) {
    if (func(this->shared_from_this())) {
        return true;
    }
    return std::visit([&func](auto& cont){
        for (auto& pChild : cont) {
            if (pChild->callRecursive(func)) {
                return true;
            }
        }
        return false;
    }, m_children);
}

template <typename DataT>
bool TreeNode<DataT>::callBackwardRecursive(const recursiveCallback_t& func) {
    if (func(this->shared_from_this())) {
        return true;
    }
    return std::visit([&func, this](auto& cont){
        auto pParent = m_parent.lock();
        if (pParent.use_count()) {
            return pParent->callBackwardRecursive(func);
        }

        return false;
    }, m_children);
}

template <typename DataT>
bool TreeNode<DataT>::callRecursive(const recursiveConstCallback_t& func) const {
    if (func(this->shared_from_this())) {
        return true;
    }
    return std::visit([&func](auto& cont){
        for (auto& pChild : cont) {
            if (pChild->callRecursive(func)) {
                return true;
            }
        }
        return false;
    }, m_children);
}

template <typename DataT>
bool TreeNode<DataT>::callBackwardRecursive(const recursiveConstCallback_t& func) const {
    if (func(this->shared_from_this())) {
        return true;
    }
    return std::visit([&func, this](auto& cont){
        auto pParent = m_parent.lock();
        if (pParent.use_count()) {
            return pParent->callBackwardRecursive(func);
        }

        return false;
    }, m_children);
}

template <typename DataT>
void TreeNode<DataT>::setParent(const TreeNodePtr<DataT>& pParent) {
    auto pSelfParent = m_parent.lock();
    if (pSelfParent) {
        pSelfParent->removeNode(this->shared_from_this());
    }
    m_parent = pParent;
    pParent->addNode(this->shared_from_this());
}

template <typename DataT>
std::size_t TreeNode<DataT>::getNodeCount() const {
    return std::visit([](auto& cont){
        return cont.size();
    }, m_children);
}

template <typename DataT>
bool TreeNode<DataT>::addNode(std::size_t beforeIndex) {
    return std::visit([beforeIndex, this](auto& cont){
        if (beforeIndex < cont.size() ||
                beforeIndex == 0) {
            auto targetPos = cont.begin();
            std::advance(targetPos, beforeIndex);
            cont.insert(targetPos, std::make_shared<TreeNode<DataT> >(this->shared_from_this()));
            return true;
        }
        return false;
    }, m_children);
}

template <typename DataT>
void TreeNode<DataT>::addNode(const TreeNodePtr<DataT>& pNode) {
    std::visit([pNode](auto& cont){
        cont.push_back(pNode);
    }, m_children);
}

template <typename DataT>
void TreeNode<DataT>::addNode(const DataT& nodeData) {
    auto pNode = std::make_shared<TreeNode<DataT> >(this->shared_from_this());
    pNode->setData(nodeData);
    return addNode(pNode);
}

template <typename DataT>
bool TreeNode<DataT>::removeNode(TreeNodePtr<DataT> pNode) {
    return std::visit([pNode, this](auto& cont){
        auto targetPos = std::find(cont.begin(), cont.end(), pNode);
        if (targetPos != cont.end()) {
            cont.erase(targetPos);
            return true;
        }
        return false;
    }, m_children);
}

template <typename DataT>
bool TreeNode<DataT>::removeNode(std::size_t index) {
    return std::visit([index, this](auto& cont){
        if (index < cont.size()) {
            auto targetPos = cont.begin();
            std::advance(targetPos, index);
            cont.erase(targetPos);
            return true;
        }
        return false;
    }, m_children);
}

template <typename DataT>
bool TreeNode<DataT>::removeNode(const DataT &value) {
    return removeNode(findNode(value));
}

template <typename DataT>
void TreeNode<DataT>::clearNodes() {
    return std::visit([this](auto& cont){
        cont.clear();
    }, m_children);
}

template <typename DataT>
TreeNodePtr<DataT> TreeNode<DataT>::getNode(std::size_t index) const {
    return std::visit([index, this](auto& cont) -> TreeNodePtr<DataT> {
        if (index < cont.size()) {
            auto targetPos = cont.begin();
            std::advance(targetPos, index);
            return *targetPos;
        }
        return {};
    }, m_children);
}

template<typename DataT>
TreeNodePtr<DataT> TreeNode<DataT>::findNode(const DataT& value) const
{
    return std::visit([&value, this](auto& cont) -> TreeNodePtr<DataT> {
        auto targetPos = std::find_if(cont.begin(), cont.end(), [&value](auto& pNode){
            return (pNode->getData() == value);
        });
        if (targetPos != cont.end()) {
            return *targetPos;
        }
        return {};
    }, m_children);
}

template<typename DataT>
std::size_t TreeNode<DataT>::getDepth() const
{
    std::size_t depth {0};
    auto pParent = m_parent.lock();
    if (pParent) {
        pParent->callBackwardRecursive([&depth](auto& pNode) -> bool {
            ++depth;
            return false;
        });
    }
    return depth;
}

template<typename DataT>
std::size_t TreeNode<DataT>::getTotalChildCount() const
{
    std::size_t count {0};
    callRecursive([&count](auto& pNode) -> bool {
        ++count;
        return false;
    });
    --count; // Remove self from count
    return count;
}


template <typename DataT>
void TreeNode<DataT>::setData(const DataT& iData) {
    m_data = iData;
}

template <typename DataT>
DataT TreeNode<DataT>::getData() const {
    return m_data;
}

template<typename DataT>
void TreeNode<DataT>::setupAsList()
{
    if (std::holds_alternative< std::vector<TreeNodePtr<DataT> > >(m_children)) {
        auto childrenV = std::get< std::vector<TreeNodePtr<DataT> > >(m_children);
        std::list<TreeNodePtr<DataT> > children;
        std::copy(childrenV.begin(), childrenV.end(), std::back_inserter(children));
        m_children = std::move(children);
    }
}

template<typename DataT>
void TreeNode<DataT>::setupAsVector()
{
    if (std::holds_alternative< std::list<TreeNodePtr<DataT> > >(m_children)) {
        auto childrenL = std::get< std::list<TreeNodePtr<DataT> > >(m_children);
        std::vector<TreeNodePtr<DataT> > children;
        children.reserve(childrenL.size());
        std::copy(childrenL.begin(), childrenL.end(), std::back_inserter(children));
        m_children = std::move(children);
    }
}


template <typename DataT>
TreeObject<DataT>::TreeObject() {
    m_rootNode = std::make_shared<TreeNode<DataT> >();
}

template <typename DataT>
TreeNodePtr<DataT> TreeObject<DataT>::getRootNode() const {
    return m_rootNode;
}

}
