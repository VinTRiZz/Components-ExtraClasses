#pragma once

#include <memory>
#include <vector>
#include <list>
#include <vector>
#include <variant>
#include <functional>

namespace ExtraClasses
{
template <typename DataT> class TreeNode;
template <typename DataT> using TreeNodePtr = std::shared_ptr<TreeNode<DataT> >;
template <typename DataT> using TreeNodeConstPtr = std::shared_ptr<const TreeNode<DataT> >;
template <typename DataT> using TreeParentNodePtr = std::weak_ptr<TreeNode<DataT> >;

template <typename DataT>
class TreeNode : public std::enable_shared_from_this<TreeNode<DataT> >
{
public:
    explicit TreeNode(const TreeNodePtr<DataT>& pParent = {});

    using recursiveCallback_t = std::function<bool(const TreeNodePtr<DataT>&)>;
    using recursiveConstCallback_t = std::function<bool(const TreeNodeConstPtr<DataT>&)>;
    
    // Function meturn true if must stop calling after processing node
    bool callRecursive(const recursiveCallback_t& func);
    bool callBackwardRecursive(const recursiveCallback_t& func);
    bool callRecursive(const recursiveConstCallback_t& func) const;
    bool callBackwardRecursive(const recursiveConstCallback_t& func) const;
    
    void setParent(const TreeNodePtr<DataT>& pParent);
    
    std::size_t getNodeCount() const;
    
    bool addNode(std::size_t beforeIndex);
    void addNode(const TreeNodePtr<DataT>& pNode);
    void addNode(const DataT& nodeData);
    bool removeNode(std::size_t index);
    void clearNodes();
    void clearData();
    TreeNodePtr<DataT> getNode(std::size_t index) const;
    std::size_t getDepth() const;
    
    void setData(const DataT& iData);
    DataT getData() const;

    /**
     * @brief setupAsList Convert internal data structure into list holding. Added for performance needs (many children or equal)
     */
    void setupAsList();

    /**
     * @brief setupAsList Convert internal data structure into vector holding. Added for performance needs (many children or equal)
     */
    void setupAsVector();
    
private:
    TreeParentNodePtr<DataT> m_parent;
    std::variant< std::list<TreeNodePtr<DataT> >, std::vector<TreeNodePtr<DataT> > > m_children;
    DataT m_data {};
};

/**
 * @brief The TreeObject class Tree object for any data type
 */
template <typename DataT>
class TreeObject
{
public:
    TreeObject();

    TreeNodePtr<DataT> getRootNode() const;
    
private:
    TreeNodePtr<DataT> m_rootNode;
};
}

#include "treeobject.inl"
