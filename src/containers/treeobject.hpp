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

/**
 * @brief The TreeNode class Node of a tree with common features
 * @note Tree node is not safe fo recursions, so handle it by yourself!
 */
template <typename DataT>
class TreeNode : public std::enable_shared_from_this<TreeNode<DataT> >
{
public:
    explicit TreeNode(const TreeNodePtr<DataT>& pParent = {});

    // Function meturn true if must stop calling after processing node
    using recursiveCallback_t = std::function<bool(const TreeNodePtr<DataT>&)>;
    using recursiveConstCallback_t = std::function<bool(const TreeNodeConstPtr<DataT>&)>;
    
    /**
     * @brief callRecursive Call operation recursively, including this object
     * @param func          Function to call
     * @return              Result of @param func
     */
    bool callRecursive(const recursiveCallback_t& func);

    /**
     * @brief callBackwardRecursive Call operation recursively on parents, including this object
     * @param func                  Function to call
     * @return                      Result of @param func
     */
    bool callBackwardRecursive(const recursiveCallback_t& func);

    /**
     * @brief callRecursive Call operation recursively, including this object
     * @param func          Function to call
     * @return              Result of @param func
     */
    bool callRecursive(const recursiveConstCallback_t& func) const;

    /**
     * @brief callBackwardRecursive Call operation recursively on parents, including this object
     * @param func                  Function to call
     * @return                      Result of @param func
     */
    bool callBackwardRecursive(const recursiveConstCallback_t& func) const;
    
    /**
     * @brief setParent Set parent of a node (actually moves tree)
     * @param pParent
     */
    void setParent(const TreeNodePtr<DataT>& pParent);
    
    /**
     * @brief getNodeCount  Get count of this node child nodes
     * @return
     */
    std::size_t getNodeCount() const;
    
    /**
     * @brief addNode       Add a new node before index
     * @param beforeIndex
     * @return
     */
    bool addNode(std::size_t beforeIndex);

    /**
     * @brief addNode   Add node to end of internal container
     * @param pNode
     */
    void addNode(const TreeNodePtr<DataT>& pNode);

    /**
     * @brief addNode   Add node handling value to the end of internal container
     * @param nodeData
     */
    void addNode(const DataT& nodeData);

    /**
     * @brief removeNode    Remove node
     * @param pNode         node pointer
     * @return              true if node removed
     */
    bool removeNode(TreeNodePtr<DataT> pNode);

    /**
     * @brief removeNode    Remove node by index
     * @param index
     * @return              true if node removed
     */
    bool removeNode(std::size_t index);

    /**
     * @brief removeNode    Remove node by it's value. Linear complexity
     * @param value
     * @return              true if node removed
     */
    bool removeNode(const DataT& value);

    /**
     * @brief clearNodes Remove all child nodes
     */
    void clearNodes();

    /**
     * @brief getNode   Get node by it's index
     * @param index
     * @return
     */
    TreeNodePtr<DataT> getNode(std::size_t index) const;

    /**
     * @brief findNode  Find node by it's value. Linear complexity
     * @param value
     * @return
     */
    TreeNodePtr<DataT> findNode(const DataT &value) const;

    /**
     * @brief getDepth  Get node depth. Calculates up to root node
     * @return          Value from 0 (root index)
     */
    std::size_t getDepth() const;

    /**
     * @brief getTotalChildCount    Get all children count (calculates recursively)
     * @return
     */
    std::size_t getTotalChildCount() const;
    
    /**
     * @brief setData   Set internal data
     * @param iData
     */
    void setData(const DataT& iData);

    /**
     * @brief getData   Get internal data
     * @return
     */
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
 * @brief The TreeObject class Tree object for any data type. Added just to be base class
 */
template <typename DataT>
class TreeObject
{
public:
    TreeObject();
    virtual ~TreeObject() = default;

    /**
     * @brief getRootNode   Get internal root node to handle first depth level
     * @return
     */
    TreeNodePtr<DataT> getRootNode() const;
    
private:
    TreeNodePtr<DataT> m_rootNode;
};

}

#include "treeobject.inl"
