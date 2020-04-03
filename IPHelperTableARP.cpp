#include "IPHelperTableARP.h"
#include <iphlpapi.h>

std::vector<IPHelper::TableARPRow> getTable()
{
    unsigned long bufferSize = 0;

    unsigned long errorCode;
    if ((errorCode = GetIpNetTable(nullptr, &bufferSize, true)) != ERROR_INSUFFICIENT_BUFFER)
        throw IPHelper::TableARP::GetTableError(errorCode);

    MIB_IPNETTABLE *buffer = (MIB_IPNETTABLE *)malloc(bufferSize);

    if ((errorCode = GetIpNetTable(buffer, &bufferSize, true)) != NO_ERROR)
        throw IPHelper::TableARP::GetTableError(errorCode);

    std::vector<IPHelper::TableARPRow> table;

    for (unsigned long i = 0; i < buffer->dwNumEntries; i++) {
        IPHelper::TableARPRow row;
        row.index = buffer->table[i].dwIndex;
        row.addressIP = buffer->table[i].dwAddr;
        if (buffer->table[i].dwPhysAddrLen) {
            row.isMACAddressExsists = true;
            row.addressMAC = buffer->table[i].bPhysAddr;
        }
        else row.isMACAddressExsists = false;
        row.type = IPHelper::TableARPRow::Type(buffer->table[i].dwType);
        table.push_back(row);
    }

    free(buffer);

    return table;
}

IPHelper::TableARP::TableARP()
{
    try {
        this->table = getTable();
    } catch (GetTableError &exception) {
        throw exception;
    }
}

void IPHelper::TableARP::append(const IPHelper::TableARPRow &row)
{
    MIB_IPNETROW buffer;

    buffer.dwIndex = row.index;
    buffer.dwAddr = row.addressIP.toUlong();
    buffer.dwPhysAddrLen = row.isMACAddressExsists ? row.addressMAC.size() : 0;

    if (row.isMACAddressExsists)
        for (unsigned i = 0; i < row.addressMAC.size(); i++)
            buffer.bPhysAddr[i] = row.addressMAC[i];

    buffer.dwType = row.type;

    unsigned long errorCode;
    if ((errorCode = CreateIpNetEntry(&buffer)) != NO_ERROR) throw AppendError(errorCode);

    this->update();
}

void IPHelper::TableARP::remove(unsigned long index, const IPHelper::AddressIP &address)
{
    MIB_IPNETROW buffer;

    buffer.dwIndex = index;
    buffer.dwAddr = address.toUlong();

    unsigned long errorCode;
    if ((errorCode = DeleteIpNetEntry(&buffer)) != NO_ERROR) throw RemoveError(errorCode);

    this->update();
}

IPHelper::AddressMAC IPHelper::TableARP::getMACByIP(const IPHelper::AddressIP &ip)
{
    unsigned long bufferSize = 8;
    unsigned char buffer[8];

    unsigned long errorCode;
    if ((errorCode = SendARP(ip.toUlong(), 0, buffer, &bufferSize)) != NO_ERROR)
        throw GetMACByIPError(errorCode);

    return AddressMAC(buffer);
}

IPHelper::TableARPRow IPHelper::TableARP::operator [](unsigned i) const
{
    return this->table[i];
}

void IPHelper::TableARP::update()
{
    this->table.clear();

    try {
        this->table = getTable();
    } catch (GetTableError &exception) {
        throw exception;
    }
}

size_t IPHelper::TableARP::size() const
{
    return this->table.size();
}

IPHelper::TableARP::GetTableError::GetTableError(unsigned long newErrorCode) : errorCode(newErrorCode) {}

const char *IPHelper::TableARP::GetTableError::what() const noexcept
{
    switch (this->errorCode) {

    case ERROR_INSUFFICIENT_BUFFER:
        return "IP Helper: cannot get ARP table. Buffer is not large enough.";
    case ERROR_INVALID_PARAMETER:
        return "IP Helper: cannot get ARP table. Invalid parameter.";
    case ERROR_NO_DATA:
        return "IP Helper: cannot get ARP table. ARP table is empty.";
    case ERROR_NOT_SUPPORTED:
        return "IP Helper: cannot get ARP table. The IPv4 transport is not configured on the local computer.";
    default:
        return "IP Helper: cannot get ARP table.";

    }
}



IPHelper::TableARP::AppendError::AppendError(unsigned long newErrorCode) : errorCode(newErrorCode) {}

const char *IPHelper::TableARP::AppendError::what() const noexcept
{
    switch (this->errorCode) {

    case ERROR_ACCESS_DENIED:
        return "IP Helper: cannot append row to ARP table. Access is denied.";
    case ERROR_INVALID_PARAMETER:
        return "IP Helper: cannot append row to ARP table. Invalid parameter.";
    case ERROR_NOT_SUPPORTED:
        return "IP Helper: cannot append row to ARP table. The IPv4 transport is not configured on the local computer.";
    default:
        return "IP Helper: cannot append row to ARP table.";

    }
}

IPHelper::TableARP::RemoveError::RemoveError(unsigned long newErrorCode) : errorCode(newErrorCode) {}

const char *IPHelper::TableARP::RemoveError::what() const noexcept
{
    switch (this->errorCode) {

    case ERROR_ACCESS_DENIED:
        return "IP Helper: cannot remove row from ARP table. Access is denied.";
    case ERROR_INVALID_PARAMETER:
        return "IP Helper: cannot remove row from ARP table. Invalid parameter.";
    case ERROR_NOT_SUPPORTED:
        return "IP Helper: cannot remove row from ARP table. The IPv4 transport is not configured on the local computer.";
    default:
        return "IP Helper: cannot remove row from ARP table.";

    }
}

IPHelper::TableARP::GetMACByIPError::GetMACByIPError(unsigned long newErrorCode) : errorCode(newErrorCode) {}

const char *IPHelper::TableARP::GetMACByIPError::what() const noexcept
{
    switch (this->errorCode) {

    case ERROR_BAD_NET_NAME:
        return "IP Helper: cannot find MAC by IP. The network name cannot be found.";
    case ERROR_BUFFER_OVERFLOW:
        return "IP Helper: cannot find MAC by IP. Buffer overflow.";
    case ERROR_GEN_FAILURE:
        return "IP Helper: cannot find MAC by IP. A device attached to the system is not functioning.";
    case ERROR_INVALID_PARAMETER:
        return "IP Helper: cannot find MAC by IP. Invalid parameter.";
    case ERROR_INVALID_USER_BUFFER:
        return "IP Helper: cannot find MAC by IP. The supplied user buffer is not valid for the requested operation.";
    case ERROR_NOT_FOUND:
        return "IP Helper: cannot find MAC by IP. MAC not found.";
    case ERROR_NOT_SUPPORTED:
        return "IP Helper: cannot find MAC by IP. The IPv4 transport is not configured on the local computer.";
    default:
        return "IP Helper: cannot find MAC by IP.";

    }
}
