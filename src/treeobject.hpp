#pragma once

#include <memory>
#include <vector>
#include <list>
#include <string>
#include <functional>

namespace ExtraClasses
{
    
class TreeNode;
using TreeNodePtr = std::shared_ptr<TreeNode>;
using TreeNodeConstPtr = std::shared_ptr<const TreeNode>;
using TreeParentNodePtr = std::weak_ptr<TreeNode>;

// TODO: Задавать тип контейнера (вектор / лист)
class TreeNode : public std::enable_shared_from_this<TreeNode>
{
public:
    explicit TreeNode(const TreeNodePtr& pParent = {});
    ~TreeNode();
    
    using recursiveCallback_t = std::function<bool(const TreeNodePtr&)>;
    using recursiveConstCallback_t = std::function<bool(const TreeNodeConstPtr&)>;
    
    // Function meturn true if must stop calling after processing node
    bool callRecursive(const recursiveCallback_t& func);
    bool callBackwardRecursive(const recursiveCallback_t& func);
    bool callRecursive(const recursiveConstCallback_t& func) const;
    bool callBackwardRecursive(const recursiveConstCallback_t& func) const;
    
    void setParent(const TreeNodePtr& pParent);
    
    std::size_t getNodeCount() const;
    
    bool addNode(std::size_t beforeIndex);
    void addNode(const TreeNodePtr& pNode);
    bool removeNode(std::size_t index);
    void clearNodes();
    void clearData();
    TreeNodePtr getNode(std::size_t index) const;
    
    void setData(const std::string& iData);
    std::string getData() const;
    
private:
    TreeParentNodePtr m_parent;
    std::list<TreeNodePtr> m_children;
    
    std::string m_data;
};
    
    
class TreeObject
{
public:
    TreeObject();

    TreeNodePtr getRootNode() const;
    
private:
    TreeNodePtr m_rootNode;
};
}
