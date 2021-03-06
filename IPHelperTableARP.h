#ifndef IPHELPERTABLEARP_H
#define IPHELPERTABLEARP_H

#include "IPHelperBasicTypes.h"

namespace IPHelper {

struct TableARPRow {
    enum Type { Unknown = 1, Invalid, Dynamic, Static };
    unsigned long index;
    AddressIP addressIP;
    bool isMACAddressExsists;
    AddressMAC addressMAC;
    Type type;
};

class TableARP {
    std::vector<TableARPRow> table;
public:
    TableARP();
    void append(const TableARPRow &row);
    void remove(unsigned long index, const AddressIP &address);
    AddressMAC getMACByIP(const AddressIP &ip);
    TableARPRow operator [] (unsigned i) const;
    void update();
    size_t size() const;

    class GetTableError : public std::exception {
        unsigned long errorCode;
    public:
        GetTableError(unsigned long newErrorCode);
        const char *what() const noexcept override;
    };

    class AppendError : public std::exception {
        unsigned long errorCode;
    public:
        AppendError(unsigned long newErrorCode);
        const char * what() const noexcept override;
    };

    class RemoveError : public std::exception {
        unsigned long errorCode;
    public:
        RemoveError(unsigned long newErrorCode);
        const char *what() const noexcept override;
    };

    class GetMACByIPError : public std::exception {
        unsigned long errorCode;
    public:
        GetMACByIPError(unsigned long newErrorCode);
        const char *what() const noexcept override;
    };
};

}

#endif // IPHELPERTABLEARP_H
