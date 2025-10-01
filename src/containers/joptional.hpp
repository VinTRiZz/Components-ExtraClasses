#pragma once

#include <optional>
#include <nlohmann/json.hpp>

namespace ExtraClasses
{

template <typename T>
class JOptional final : public std::optional<T>
{
    T butaforValue {}; // Used to return in tryGetValue
public:
    using std::optional<T>::has_value;
    using std::optional<T>::operator=;
    using std::optional<T>::optional;
    using std::optional<T>::value;
    operator bool() = delete;

    const T& tryGetValue() const {
        if (std::optional<T>::has_value()) {
            return std::optional<T>::value();
        }
        return butaforValue;
    }

    auto operator=(const T& val) {
        return std::optional<T>::operator=(val);
    }

    auto operator+(const T& val) {
        if (!std::optional<T>::has_value()) {
            std::optional<T>::operator=(T());
            std::optional<T>::value() +(val);
        } else {
            std::optional<T>::operator=(std::optional<T>::value() + val);
        }
        return *this;
    }

    auto operator+=(const T& val) {
        if (!std::optional<T>::has_value()) {
            std::optional<T>::operator=(T{});
            std::optional<T>::value() +=val;
        } else {
            std::optional<T>::operator=(std::optional<T>::value() += val);
        }
        return *this;
    }

    operator nlohmann::json() {
        if (std::optional<T>::has_value()) {
            return std::optional<T>::value();
        }
        return {};
    }

    operator T() {
        if (std::optional<T>::has_value()) {
            return std::optional<T>::value();
        }
        return {};
    }

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
