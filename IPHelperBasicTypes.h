#ifndef IPHELPERBASICTYPES_H
#define IPHELPERBASICTYPES_H

#include <vector>
#include <string>

namespace IPHelper {

class AddressIP {
    std::vector<unsigned char> data;
public:
    AddressIP();
    AddressIP(const std::string &stringIP);
    AddressIP(const char *cStrIP);
    AddressIP(unsigned long ulongIP);
    std::string toStdString() const;
    unsigned long toUlong() const;
    unsigned short size() const;
    unsigned char &operator [] (unsigned i);
    unsigned char operator [] (unsigned i) const;

    class GetFromStringError : public std::exception {
    public:
        const char * what() const noexcept override;
    };
};

std::istream &operator >> (std::istream &is, IPHelper::AddressIP &address);
std::ostream &operator << (std::ostream &os, const IPHelper::AddressIP &address);

class AddressMAC {
    std::vector<unsigned char> data;
public:
    AddressMAC();
    AddressMAC(const std::vector<unsigned char> &vectorMAC);
    AddressMAC(const unsigned char *cArrayMAC);
    unsigned short size() const;
    unsigned char &operator [] (unsigned i);
    unsigned char operator [] (unsigned i) const;
};

}

#endif // IPHELPERBASICTYPES_H
