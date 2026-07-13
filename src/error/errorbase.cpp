#include "errorbase.hpp"

#include <Components/Logger/Logger.h>

namespace ExtraClasses
{

ErrorBase::ErrorBase(int errCode, const std::string &detailText) :
    std::exception(),
    m_errorCode {errCode},
    m_detailText {detailText}
{

}

const char *ErrorBase::what() const noexcept
{
    m_dumpText = getErrorText() + (m_detailText.empty() ? std::string() : std::string(" (") + m_detailText + ")");
    return m_dumpText.c_str();
}

void ErrorBase::printSelfStd() const
{
    std::cerr << what() << std::endl; // Logger can not be stated or invalid
}

void ErrorBase::printSelf()
{
    COMPLOG_ERROR(what());
}

void ErrorBase::printSelf(int errCode, const std::string &detailText)
{
    COMPLOG_ERROR(ErrorBase{}.errorCodeToText(errCode), detailText);
}

void ErrorBase::reset() {
    setCode(ErrorCode_OK);
}

void ErrorBase::setCode(int errCode)
{
    m_errorCode = errCode;
    if (m_errorCode == ErrorCode_OK) {
        m_detailText.clear();
    }
}

int ErrorBase::getCode() const
{
    return m_errorCode;
}

bool ErrorBase::isOk() const
{
    return (m_errorCode == ErrorCode_OK);
}

ErrorBase::operator bool() const
{
    return isOk();
}

std::string ErrorBase::getErrorText() const
{
    return errorCodeToText(m_errorCode);
}

std::string ErrorBase::errorCodeToText(int errc) const
{
    return {};
}

void ErrorBase::setDetailText(const std::string &errText)
{
    m_detailText = errText;
}

std::string ErrorBase::getDetailText() const
{
    return m_detailText;
}

}