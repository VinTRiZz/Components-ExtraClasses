#include "datafragmentator.hpp"

#include <algorithm>

namespace ExtraClasses
{

bool DataFragment::operator <(const DataFragment &odf) const {
    return (id < odf.id);
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
    m_actualSize.fetch_add(data.data.size(), std::memory_order_release);
    m_fragmentMx.lock();
    m_fragments.emplace(std::move(data));
    m_fragmentMx.unlock();
}

const std::set<DataFragment> &DataInfo::getFragments() const
{
    return m_fragments;
}

bool DataInfo::isValid() const
{
    return (getSize() == getActualSize());
}

bool DataInfo::compile()
{
    if (!isValid()) {
        return false;
    }

    // TODO: Connect all data
    return false;
}

bool DataInfo::split(uint64_t splitDataSize)
{
    if (!isValid()) {
        return false;
    }

    // TODO: Split all data into parts
    return false;
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
