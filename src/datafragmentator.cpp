#include "datafragmentator.hpp"

#include <algorithm>

namespace ExtraClasses
{

bool DataFragment::operator <(const DataFragment &odf) const {
    return (beginBytePos < odf.beginBytePos);
}

DataInfo::DataInfo(DataId_t id, uint64_t dataSize) :
    m_id {id},
    m_size {dataSize}
{

}

DataId_t DataInfo::getId() const
{
    return m_id;
}

uint64_t DataInfo::getSize() const
{
    return m_size;
}

uint64_t DataInfo::getActualSize() const
{
    return m_actualSize.load(std::memory_order_acquire);
}

void DataInfo::addFragment(DataFragment &&data)
{
    m_fragmentMx.lock();
    m_actualSize.fetch_add(data.data.size(), std::memory_order_release);
    m_fragments.emplace(std::move(data));
    m_fragmentMx.unlock();
}

const std::set<DataFragment> &DataInfo::getFragments() const
{
    return m_fragments;
}

bool DataInfo::isValid() const
{
    return (m_size != 0) && (m_size == getActualSize());
}

bool DataInfo::compile()
{
    if (!isValid()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(m_fragmentMx);
    if (m_fragments.size() == 1) {
        return true;
    }
    auto firstPart = std::move(*m_fragments.begin());
    m_fragments.erase(firstPart);

    for (auto& fragment :  m_fragments) {
        firstPart.data.reserve(fragment.data.size());
        std::copy_n(fragment.data.data(), fragment.data.size(), std::back_inserter(firstPart.data));
    }
    m_fragments.clear();

    auto isCompilationSucceed = (firstPart.data.size() == getActualSize());
    m_fragments.emplace(std::move(firstPart));
    return isCompilationSucceed;
}

bool DataInfo::split(uint64_t splitDataSize)
{
    if (!isValid()) {
        return false;
    }
    if (!compile()) {
        return false;
    }

    auto firstPart = std::move(*m_fragments.begin());
    m_fragments.erase(firstPart);

    uint64_t curpos {};
    const auto dataSize = firstPart.data.size();
    while (curpos < dataSize) {
        DataFragment splittedDataPart;
        auto deltaSize = std::min(splitDataSize, dataSize - curpos);
        splittedDataPart.data.reserve(deltaSize);
        std::copy_n(firstPart.data.data() + curpos, deltaSize, std::back_inserter(splittedDataPart.data));
        splittedDataPart.beginBytePos = curpos;
        curpos += deltaSize;
        m_fragments.emplace(std::move(splittedDataPart));
    }

    std::size_t totalDataSize {0};
    for (const auto& fragment : m_fragments) {
        totalDataSize += fragment.data.size();
    }
    return (totalDataSize == getActualSize());
}

bool DataInfo::operator <(const DataInfo &odi) const {
    return (m_id < odi.m_id);
}

bool DataInfo::operator ==(DataId_t id) const
{
    return (m_id == id);
}

void DataFragmentator::registerData(DataId_t dataId, uint64_t dataSize)
{
    m_dataRegisterMx.lock();
    m_data.push_front(std::make_shared<DataInfo>(dataId, dataSize));
    m_dataRegisterMx.unlock();
}

bool DataFragmentator::addFragment(DataId_t dataId, DataFragment &&data)
{
    decltype(m_data)::iterator targetData;
    {
        std::lock_guard<std::mutex> lock(m_dataRegisterMx);
        targetData = std::find_if(m_data.begin(), m_data.end(), [dataId](auto& dataInfo){
            return (dataInfo->getId() == dataId);
        });
        if (targetData == m_data.end()) {
            return false;
        }
    }
    (*targetData)->addFragment(std::move(data));
    return true;
}

DataInfoPtr DataFragmentator::getData(uint64_t dataId)
{
    decltype(m_data)::iterator targetData;
    {
        std::lock_guard<std::mutex> lock(m_dataRegisterMx);
        targetData = std::find_if(m_data.begin(), m_data.end(), [dataId](auto& dataInfo){
            return (dataInfo->getId() == dataId);
        });
        if (targetData == m_data.end()) {
            return {};
        }
    }
    return *targetData;
}

}
