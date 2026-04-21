#pragma once

#include <optional>
#include <nlohmann/json.hpp>

namespace ExtraClasses
{

template <typename ValueT>
class JOptional : public std::optional<ValueT>
{
    // Extra metaclasses
    template <typename T, typename = void>
    struct is_compareAvailable : std::false_type {};

    template <typename T>
    struct is_compareAvailable<T, std::void_t<decltype(std::declval<T>().operator<(T{}))>> : std::true_type {};

public:
    using type = ValueT;
    using std::optional<ValueT>::has_value;
    using std::optional<ValueT>::optional;
    using std::optional<ValueT>::value;
    operator bool() = delete;

    auto operator=(const ValueT& val) {
        return std::optional<ValueT>::operator=(val);
    }
    auto operator=(const nlohmann::json& iJson) {
        if (iJson.is_null()) {
            return std::optional<ValueT>::operator=(std::nullopt);
        }
        return std::optional<ValueT>::operator=(iJson);
    }

    ValueT& operator+(const ValueT& val) {
        if (!has_value()) {
            return ValueT{} + val;
        }
        return value() + val;
    }

    ValueT& operator+=(const ValueT& val) {
        if (!has_value()) {
            return ValueT{} += val;
        }
        return value() += val;
    }

    bool operator==(const ValueT& val) const {
        if (!has_value() || !has_value()) {
            return false;
        }
        return value() == val;
    }
    bool operator==(const JOptional<ValueT>& optVal) const {
        if (!has_value() || !optVal.has_value()) {
            return false;
        }
        return value() == optVal.value();
    }

    template <typename vT = ValueT, typename = std::void_t<is_compareAvailable<ValueT> > >
    bool operator<(const JOptional<ValueT>& _ohdl) const {
        if (!has_value() || !_ohdl.has_value()) {
            return false;
        }
        return value() < _ohdl.value();
    }

    operator nlohmann::json() const {
        if (std::optional<ValueT>::has_value()) {
            return std::optional<ValueT>::value();
        }
        return {};
    }

    operator ValueT() const {
        if (std::optional<ValueT>::has_value()) {
            return std::optional<ValueT>::value();
        }
        return {};
    }

    // Debug
    friend std::ostream& operator<<(std::ostream& os, const JOptional& jOptional) {
        if (jOptional.has_value()) {
            os << jOptional.value();
        } else {
            os << "NULL";
        }
        return os; // Return the stream to allow chaining
    }
};

}
