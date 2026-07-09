#pragma once

#include <memory>
#include <stdexcept>

namespace ExtraClasses {

/**
 * @brief The HandlerBase class Base for pointer handlers
 */
template <typename ValueT>
class HandlerBase
{
    // Extra metaclasses
    template <typename T, typename = void>
    struct is_compareAvailable : std::false_type {};

    template <typename T>
    struct is_compareAvailable<T, std::void_t<decltype(std::declval<T>().operator<(T{}))>> : std::true_type {};

public:
    using value_t = ValueT;

    HandlerBase() = default;
    HandlerBase(HandlerBase&&) = default;
    HandlerBase(const HandlerBase&) = default;
    explicit HandlerBase(value_t* pTarget);
    explicit HandlerBase(value_t& pTarget);
    ~HandlerBase() = default;

    // Analog of std::static_pointer_cast
    template <typename DerivedT>
    std::enable_if_t<std::is_base_of_v<value_t, DerivedT> || std::is_base_of_v<DerivedT, value_t>,
                     DerivedT*>
    cast();
    template <typename DerivedT>
    std::enable_if_t<std::is_base_of_v<value_t, DerivedT> || std::is_base_of_v<DerivedT, value_t>,
                     const DerivedT*>
    cast() const;

    // Analog of std::dynamic_pointer_cast
    template <typename DerivedT>
    std::enable_if_t<std::is_base_of_v<value_t, DerivedT> || std::is_base_of_v<DerivedT, value_t>,
                     DerivedT*>
    cast_dynamic();
    template <typename DerivedT>
    std::enable_if_t<std::is_base_of_v<value_t, DerivedT> || std::is_base_of_v<DerivedT, value_t>,
                     const DerivedT*>
    cast_dynamic() const;

    // Pointer access operators
    value_t* operator->() noexcept(false);
    value_t& operator*() noexcept(false);
    const value_t* operator->() const noexcept(false);
    const value_t& operator*() const noexcept(false);

    // Copy of STL smart pointers logic
    value_t* get();
    const value_t* get() const;
    operator bool() const;

    bool isValid() const noexcept;

    // For std::set and others using
    bool operator <(const HandlerBase& _ohdl) const;

    // Assigment
    HandlerBase& operator =(const HandlerBase& _ohdl) = default;
    HandlerBase& operator =(HandlerBase&& _ohdl) = default;
    HandlerBase& operator =(value_t* _ohdl);

private:
    std::shared_ptr<bool> m_isValid { std::make_shared<bool>(false) };
    value_t* m_pTarget {nullptr}; // Pointer for synchronization

protected:
    // Add space for reimplement
    virtual void setPointer(value_t* pTarget);
    void invalidate();
};

// =============================== IMPLEMENTATION ===================================== //

template<typename ValueT>
inline HandlerBase<ValueT>::HandlerBase(ValueT *pTarget) { setPointer(pTarget); }
template<typename ValueT>
inline HandlerBase<ValueT>::HandlerBase(ValueT &pTarget) { setPointer(&pTarget); }

template <typename ValueT>
template <typename DerivedT>
std::enable_if_t<std::is_base_of_v<ValueT, DerivedT> || std::is_base_of_v<DerivedT, ValueT>, DerivedT*>
HandlerBase<ValueT>::cast()
{
    if (!isValid()) {
        throw std::runtime_error("Invalid pointer");
    }
    return static_cast<DerivedT*>(m_pTarget);
}

template <typename ValueT>
template <typename DerivedT>
std::enable_if_t<std::is_base_of_v<ValueT, DerivedT> || std::is_base_of_v<DerivedT, ValueT>, const DerivedT*>
HandlerBase<ValueT>::cast() const
{
    if (!isValid()) {
        throw std::runtime_error("Invalid pointer");
    }
    return static_cast<const DerivedT*>(m_pTarget);
}

template <typename ValueT>
template <typename DerivedT>
std::enable_if_t<std::is_base_of_v<ValueT, DerivedT> || std::is_base_of_v<DerivedT, ValueT>, DerivedT*>
HandlerBase<ValueT>::cast_dynamic()
{
    if (!isValid()) {
        throw std::runtime_error("Invalid pointer");
    }
    return dynamic_cast<DerivedT*>(m_pTarget);
}

template <typename ValueT>
template <typename DerivedT>
std::enable_if_t<std::is_base_of_v<ValueT, DerivedT> || std::is_base_of_v<DerivedT, ValueT>, const DerivedT*>
HandlerBase<ValueT>::cast_dynamic() const
{
    if (!isValid()) {
        throw std::runtime_error("Invalid pointer");
    }
    return dynamic_cast<const DerivedT*>(m_pTarget);
}

template<typename ValueT>
inline ValueT *HandlerBase<ValueT>::operator->() noexcept(false) {
    if (!isValid()) {
        throw std::runtime_error("Invalid pointer");
    }
    return m_pTarget;
}

template<typename ValueT>
inline ValueT &HandlerBase<ValueT>::operator*() noexcept(false) {
    if (!isValid()) {
        throw std::runtime_error("Invalid pointer");
    }
    return *m_pTarget;
}

template<typename ValueT>
inline const ValueT &HandlerBase<ValueT>::operator*() const noexcept(false) {
    if (!isValid()) {
        throw std::runtime_error("Invalid pointer");
    }
    return *m_pTarget;
}

template<typename ValueT>
inline const ValueT *HandlerBase<ValueT>::operator->() const noexcept(false) {
    if (!isValid()) {
        throw std::runtime_error("Invalid pointer");
    }
    return m_pTarget;
}

template<typename ValueT>
inline ValueT *HandlerBase<ValueT>::get() { return m_pTarget; }

template<typename ValueT>
inline const ValueT *HandlerBase<ValueT>::get() const { return m_pTarget; }

template<typename ValueT>
inline HandlerBase<ValueT>::operator bool() const { return isValid(); }

template<typename ValueT>
inline bool HandlerBase<ValueT>::isValid() const noexcept { return *m_isValid; }

template <typename ValueT>
inline HandlerBase<ValueT>& HandlerBase<ValueT>::operator =(value_t* _ovalue)
{
    m_pTarget = _ovalue;
    *m_isValid = true;
    return *this;
}

template<typename ValueT>
inline bool HandlerBase<ValueT>::operator<(const HandlerBase &_ohdl) const {
    if (!isValid()) {
        return _ohdl.isValid();
    }
    if constexpr (is_compareAvailable<ValueT>::value) {
        return *m_pTarget < *_ohdl.m_pTarget;
    } else {
        return m_pTarget < _ohdl.m_pTarget;
    }
}

template<typename ValueT>
inline void HandlerBase<ValueT>::setPointer(ValueT *pTarget) {
    // TODO: Set pointer is not thread-safe... Think about it
    *m_isValid = false;
    m_pTarget = pTarget;
    *m_isValid = (m_pTarget != nullptr);
}

template<typename ValueT>
inline void HandlerBase<ValueT>::invalidate()
{
    *m_isValid = false;
}

} // namespace Web
