#include <iostream>
#include <cstring>
#include "IPHelper.h"

void displayHelp();
void outputARPTable(const IPHelper::TableARP &table);
void getMACByIPFromARPTable(IPHelper::TableARP *table, const char *cStrIP);
void appendToARPTable(IPHelper::TableARP *table, const char *cStrIndex, const char *cStrIP, const char *cStrMAC = nullptr);
void removeFromARPTable(IPHelper::TableARP *table, const char *cStrIndex, const char *cStrIP);

int main(int argc, char *argv[])
{
    IPHelper::TableARP table;

    if (argc == 1) displayHelp();
    else if (argc == 2 && !strcmp(argv[1], "-a")) outputARPTable(table);
    else if (argc == 3 && !strcmp(argv[1], "-f"))
        getMACByIPFromARPTable(&table, argv[2]);
    else if (argc == 4) {
        if (!strcmp(argv[1], "-s"))
            appendToARPTable(&table, argv[2], argv[3]);
        else if (!strcmp(argv[1], "-d"))
            removeFromARPTable(&table, argv[2], argv[3]);
    }
    else if (argc == 5 && !strcmp(argv[1], "-s"))
        appendToARPTable(&table, argv[2], argv[3], argv[4]);
    else std::cerr << "Error: unknown command." << std::endl;

    return 0;
}

void displayHelp()
{
    std::cout << "Displaying and changing IP-to-MAC conversion table used by" << std::endl;
    std::cout << "the Address Resolution Protocol (ARP)." << std::endl;
    std::cout << std::endl;
    std::cout << "-a  Display the ARP table." << std::endl;
    std::cout << "    Example: myARP -a" << std::endl;
    std::cout << "-s  Add an entry to ARP table." << std::endl;
    std::cout << "    Example: myARP -s <index> <IP> [<MAC>]" << std::endl;
    std::cout << "    where index - interface index in hex (example: 0x5);" << std::endl;
    std::cout << "          IP - an IP-address (example: 127.0.0.1);" << std::endl;
    std::cout << "          MAC - a physical address in hex (example: aa-aa-aa-aa-aa-aa)." << std::endl;
    std::cout << "-d  Remove an entry from ARP table." << std::endl;
    std::cout << "    Example: myARP -d <index> <IP>" << std::endl;
    std::cout << "-f  Find the MAC-address by the IP-address." << std::endl;
    std::cout << "    Example: myARP -f <IP>" << std::endl;
    std::cout << std::endl;
    std::cout << "IMPORTANT: If you want to change the ARP table, run the programm as" << std::endl;
    std::cout << "administrator." << std::endl;
}

void outputSpaces(unsigned long spacesNum);
void outputMAC(const IPHelper::AddressMAC &address);
void outputARPRowType(IPHelper::TableARPRow::Type type);

void outputARPTable(const IPHelper::TableARP &table)
{
    unsigned long index = table[0].index;

    std::cout << "Interface: 0x" << std::hex << index << std::endl;
    std::cout << "  Address in Internet     Physical address      Type" << std::endl;

    for (unsigned i = 0; i < table.size(); i++) {
        if (table[i].index != index) {
            index = table[i].index;
            std::cout << "Interface: 0x" << std::hex << index << std::endl;
            std::cout << "  Address in Internet     Physical address      Type" << std::endl;
        }

        std::cout << "  " << table[i].addressIP;
        outputSpaces(23 - table[i].addressIP.toStdString().length());
        if (table[i].isMACAddressExsists) {
            outputMAC(table[i].addressMAC);
            outputSpaces(6);
        }
        else outputSpaces(23);
        outputARPRowType(table[i].type);
        std::cout << std::endl;
   }
}

void outputSpaces(unsigned long spacesNum)
{
    for ( ; spacesNum; spacesNum--)
            std::cout << ' ';
}

void outputMAC(const IPHelper::AddressMAC &address)
{
    if (address[0] < 0x10) std::cout << '0';
    std::cout << std::hex << int(address[0]);

    for (unsigned i = 1; i < address.size(); i++) {
        std::cout << '-';
        if (address[i] < 0x10) std::cout << '0';
        std::cout << std::hex << int(address[i]);
    }
}

void outputARPRowType(IPHelper::TableARPRow::Type type)
{
    switch (type) {

    case IPHelper::TableARPRow::Unknown:
        std::cout << "unknown";
        break;
    case IPHelper::TableARPRow::Invalid:
        std::cout << "invalid";
        break;
    case IPHelper::TableARPRow::Dynamic:
        std::cout << "dynamic";
        break;
    case IPHelper::TableARPRow::Static:
        std::cout << "static";

    }
}

IPHelper::AddressMAC getMACFromCStr(const char *str);

void appendToARPTable(IPHelper::TableARP *table, const char *cStrIndex, const char *cStrIP, const char *cStrMAC)
{
    IPHelper::TableARPRow row;

    char *cStrIndexEnd;
    row.index = strtoul(cStrIndex, &cStrIndexEnd, 16);

    if (*cStrIndexEnd == '\0') {
        try {
            row.addressIP = cStrIP;
        } catch (IPHelper::AddressIP::GetFromStringError &exception) {
            std::cerr << exception.what() << std::endl;
            return;
        }

        row.isMACAddressExsists = cStrMAC ? true : false;

        if (row.isMACAddressExsists) row.addressMAC = getMACFromCStr(cStrMAC);

        row.type = IPHelper::TableARPRow::Static;

        try {
            table->append(row);
        } catch (IPHelper::TableARP::AppendError &exception) {
            std::cerr << exception.what() << std::endl;
            return;
        }

        std::cout << "Done." << std::endl;
    }
    else std::cerr << "Error: incorrect interface index." << std::endl;
}

IPHelper::AddressMAC getMACFromCStr(const char *str)
{
    IPHelper::AddressMAC address;
    int j = 0;

    for (unsigned i = 0; i < address.size(); i++) {
        std::string byteMAC = "0x";
        for (; str[j] && str[j] != '-'; j++) byteMAC += str[j];
        address[i] = static_cast<unsigned char>(strtoul(byteMAC.c_str(), nullptr, 16));
        j++;
    }

    return address;
}

void removeFromARPTable(IPHelper::TableARP *table, const char *cStrIndex, const char *cStrIP)
{
    char *cStrIndexEnd;
    unsigned long index = strtoul(cStrIndex, &cStrIndexEnd, 16);

    if (*cStrIndexEnd == '\0') {
        IPHelper::AddressIP ip;

        try {
            ip = cStrIP;
        } catch (IPHelper::AddressIP::GetFromStringError &exception) {
            std::cerr << exception.what() << std::endl;
            return;
        }

        try {
            table->remove(index, ip);
        } catch (IPHelper::TableARP::RemoveError &exception) {
            std::cerr << exception.what() << std::endl;
            return;
        }

        std::cout << "Done." << std::endl;
    }
    else std::cerr << "Error: incorrect interface index." << std::endl;
}

void getMACByIPFromARPTable(IPHelper::TableARP *table, const char *cStrIP)
{
    IPHelper::AddressIP ip;

    try {
        ip = cStrIP;
    } catch (IPHelper::AddressIP::GetFromStringError &exception) {
        std::cerr << exception.what() << std::endl;
        return;
    }

    IPHelper::AddressMAC addressMAC;

    try {
        addressMAC = table->getMACByIP(ip);
    } catch (IPHelper::TableARP::GetMACByIPError &exception) {
        std::cerr << exception.what() << std::endl;
        return;
    }

    outputMAC(addressMAC);
}
