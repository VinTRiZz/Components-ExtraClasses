#pragma once

#include <memory>
#include <stdexcept>

namespace ExtraClasses {

/**
 * @brief The Handler class Base for pointer handlers
 */
template <typename ValueT>
class Handler
{
    // Extra metaclasses
    template <typename T, typename = void>
    struct is_lessComparable : std::false_type {};

    template <typename T>
    struct is_lessComparable<T, std::void_t<decltype(std::declval<const T&>() < std::declval<const T&>())>> : std::true_type {};

public:
    using value_t = ValueT;

    Handler() = default;
    Handler(Handler&&) = default;
    Handler(const Handler&) = default;
    explicit Handler(value_t* pTarget) :
        m_pTarget { std::make_shared<value_t*>(nullptr) }
    {
        *m_pTarget = pTarget;
    }
    explicit Handler(value_t& pTarget) :
        m_pTarget { std::make_shared<value_t*>(nullptr) }
    {
        *m_pTarget = pTarget;
    }
    ~Handler() = default;

    // Analog of std::static_pointer_cast
    template <typename DerivedT>
    DerivedT* cast() noexcept(false) {
        static_assert(std::is_base_of_v<value_t, DerivedT> || std::is_base_of_v<DerivedT, value_t>,
                      "Derived class must be related to handler type");
        checkValid();
        return static_cast<DerivedT*>(*m_pTarget);
    }
    template <typename DerivedT>
    const DerivedT* cast() const noexcept(false) {
        static_assert(std::is_base_of_v<value_t, DerivedT> || std::is_base_of_v<DerivedT, value_t>,
                      "Derived class must be related to handler type");
        checkValid();
        return static_cast<const DerivedT*>(*m_pTarget);
    }

    // Analog of std::dynamic_pointer_cast
    template <typename DerivedT>
    DerivedT* cast_dynamic() noexcept(false) {
        checkValid();
        return dynamic_cast<DerivedT*>(*m_pTarget);
    }
    template <typename DerivedT>
    const DerivedT* cast_dynamic() const noexcept(false) {
        checkValid();
        return dynamic_cast<const DerivedT*>(*m_pTarget);
    }

    // Pointer access operators
    value_t* operator->() noexcept(false) {
        checkValid();
        return *m_pTarget;
    }
    const value_t* operator->() const noexcept(false) {
        checkValid();
        return *m_pTarget;
    }

    value_t& operator*() noexcept(false) {
        checkValid();
        return *(*m_pTarget);
    }
    const value_t& operator*() const noexcept(false) {
        checkValid();
        return *(*m_pTarget);
    }

    // Copy of STL smart pointers logic
    value_t* get() noexcept { return *m_pTarget; }
    const value_t* get() const noexcept { return *m_pTarget; }
    explicit operator bool() const { return isValid(); }

    // Handler source management
    bool isValid() const noexcept { return (nullptr != *m_pTarget); }
    void invalidate() { *m_pTarget = nullptr; }

    // For std::set and others using. Default is pointer address sort
    bool operator <(const Handler& _ohdl) const noexcept {
        if (!isValid() || !_ohdl.isValid()) {
            return isValid();
        }
        if constexpr (is_lessComparable<ValueT>::value) {
            return (*m_pTarget < *_ohdl.m_pTarget);
        } else {
            return (m_pTarget < _ohdl.m_pTarget);
        }
    }

    // Comparing
    bool operator ==(value_t* _oval) const noexcept {
        return isValid() && (*m_pTarget == _oval);
    }
    bool operator ==(const Handler& _ohdl) const noexcept {
        return (m_pTarget == _ohdl.m_pTarget);
    }

    // Assigment
    Handler& operator =(const Handler& _ohdl) noexcept = default;
    Handler& operator =(Handler&& _ohdl) noexcept = default;
    Handler& operator =(value_t* _ovalue) noexcept { *m_pTarget = _ovalue; }

private:
    std::shared_ptr<value_t*> m_pTarget { std::make_shared<value_t*>(nullptr) };

    void checkValid() const noexcept(false) {
        if (!isValid()) {
            throw std::runtime_error("Handler invalidated");
        }
    }
};

} // namespace Web
