#include "IPHelperBasicTypes.h"

IPHelper::AddressIP::AddressIP() : data(4) {}

unsigned long getSymbolPosition(char symbol, const std::string &str)
{
    for (unsigned long i = 0; i < str.length(); i++)
        if (symbol == str[i]) return i;

    throw IPHelper::AddressIP::GetFromStringError();
}

IPHelper::AddressIP::AddressIP(const std::string &stringIP) : data(4)
{
    std::string str = stringIP;

    for (unsigned i = 0; i < 3; i++) {
        unsigned long endPosition;
        try {
            endPosition = getSymbolPosition('.', str);
        } catch (GetFromStringError &) {
            throw GetFromStringError();
        }

        this->data[i] = static_cast<unsigned char>(atoi(str.substr(0, endPosition).c_str()));
        str = str.substr(endPosition + 1);
    }

    this->data[3] = static_cast<unsigned char>(atoi(str.c_str()));
}

IPHelper::AddressIP::AddressIP(const char *cStrIP) : AddressIP(std::string(cStrIP)) {}

IPHelper::AddressIP::AddressIP(unsigned long ulongIP) : data(4)
{
    int offset = 0;
    unsigned long mask = 0x000000FF;

    this->data[0] = static_cast<unsigned char>((ulongIP & mask) >> offset);
    mask <<= 8;
    offset += 8;

    for (unsigned i = 1; i < 4; i++) {
        this->data[i] = static_cast<unsigned char>((ulongIP & mask) >> offset);
        mask <<= 8;
        offset += 8;
    }
}

std::string IPHelper::AddressIP::toStdString() const
{
    std::string stringIP = std::to_string(int(this->data[0]));

    for (unsigned i = 1; i < 4; i++) {
        stringIP += ".";
        stringIP += std::to_string(int(this->data[i]));
    }

    return stringIP;
}

unsigned long IPHelper::AddressIP::toUlong() const
{
    unsigned long ulongIP = this->data[3];

    for (int i = 2; i >= 0; i--) {
        ulongIP <<= 8;
        ulongIP += this->data[unsigned(i)];
    }

    return ulongIP;
}

unsigned short IPHelper::AddressIP::size() const
{
    return 4;
}

unsigned char &IPHelper::AddressIP::operator [](unsigned i)
{
    return this->data[i];
}

unsigned char IPHelper::AddressIP::operator [](unsigned i) const
{
    return this->data[i];
}

std::istream &IPHelper::operator >>(std::istream &is, IPHelper::AddressIP &address)
{
    std::string stringIP;

    is >> stringIP;
    address = stringIP;

    return is;
}

std::ostream &IPHelper::operator <<(std::ostream &os, const IPHelper::AddressIP &address)
{
    os << address.toStdString();

    return os;
}

const char *IPHelper::AddressIP::GetFromStringError::what() const noexcept
{
    return "IP Helper: invalid string to convert to IP address.";
}

IPHelper::AddressMAC::AddressMAC() : data(6) {}

IPHelper::AddressMAC::AddressMAC(const std::vector<unsigned char> &vectorMAC) : data(6)
{
    for (unsigned i = 0; i < 6; i++) this->data[i] = vectorMAC[i];
}

IPHelper::AddressMAC::AddressMAC(const unsigned char *cArrayMAC)
    : AddressMAC({ cArrayMAC[0], cArrayMAC[1], cArrayMAC[2], cArrayMAC[3], cArrayMAC[4], cArrayMAC[5] }) {}

unsigned short IPHelper::AddressMAC::size() const
{
    return 6;
}

unsigned char &IPHelper::AddressMAC::operator [](unsigned i)
{
    return this->data[i];
}

unsigned char IPHelper::AddressMAC::operator [](unsigned i) const
{
    return this->data[i];
}
