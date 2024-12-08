#include "ipconfig.h"

void socketAPI::printUnicastAddress(PIP_ADAPTER_UNICAST_ADDRESS &ua) {
    char ipAddress[INET6_ADDRSTRLEN];
    char subnetMask[INET6_ADDRSTRLEN];
    
    printf("\t\tLength: %lu\n", ua->Length);
    printf("\t\tFlags: %lu\n", ua->Flags);

    if (ua->Address.lpSockaddr->sa_family == AF_INET) {
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)ua->Address.lpSockaddr;
        inet_ntop(AF_INET, &(ipv4->sin_addr), ipAddress, INET6_ADDRSTRLEN);
        printf("\t\tIPv4 Address: %s\n", ipAddress);

        // Calculate subnet mask from prefix length 
        ULONG prefixLength = ua->OnLinkPrefixLength; 
        ULONG mask = (0xFFFFFFFF << (32 - prefixLength)) & 0xFFFFFFFF; 
        struct in_addr mask_addr; 
        mask_addr.s_addr = htonl(mask); 
        inet_ntop(AF_INET, &mask_addr, subnetMask, INET_ADDRSTRLEN);
        printf("\t\tSubnet mask: %s\n", subnetMask);
    } else if (ua->Address.lpSockaddr->sa_family == AF_INET6) {
        struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)ua->Address.lpSockaddr;
        inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipAddress, INET6_ADDRSTRLEN);
        printf("\t\tIPv6 Address: %s\n", ipAddress);
    }

    printf("\t\tPrefix Origin: %u\n", ua->PrefixOrigin); 
    printf("\t\tSuffix Origin: %u\n", ua->SuffixOrigin); 
    printf("\t\tDad State: %u\n", ua->DadState); 
    printf("\t\tValid Lifetime: %lu\n", ua->ValidLifetime);
    printf("\t\tPreferred Lifetime: %lu\n", ua->PreferredLifetime);
    printf("\t\tLease Lifetime: %lu\n", ua->LeaseLifetime);
    printf("\t\tOn Link Prefix Length: %u\n", (int)ua->OnLinkPrefixLength);
}

void socketAPI::printAnycastAddress(PIP_ADAPTER_ANYCAST_ADDRESS aa) {
    char ipAddress[INET6_ADDRSTRLEN];
    
    if (aa->Address.lpSockaddr->sa_family == AF_INET) {
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)aa->Address.lpSockaddr;
        inet_ntop(AF_INET, &(ipv4->sin_addr), ipAddress, INET_ADDRSTRLEN);
        printf("\t\tAnycast IPv4 Address: %s\n", ipAddress);
    } else if (aa->Address.lpSockaddr->sa_family == AF_INET6) {
        struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)aa->Address.lpSockaddr;
        inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipAddress, INET6_ADDRSTRLEN);
        printf("\t\tAnycast IPv6 Address: %s\n", ipAddress);
    }
}

void socketAPI::printMulticastAddress(PIP_ADAPTER_MULTICAST_ADDRESS ma) {
    char ipAddress[INET6_ADDRSTRLEN];
    
    if (ma->Address.lpSockaddr->sa_family == AF_INET) {
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)ma->Address.lpSockaddr;
        inet_ntop(AF_INET, &(ipv4->sin_addr), ipAddress, INET_ADDRSTRLEN);
        printf("\t\tMulticast IPv4 Address: %s\n", ipAddress);
    } else if (ma->Address.lpSockaddr->sa_family == AF_INET6) {
        struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)ma->Address.lpSockaddr;
        inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipAddress, INET6_ADDRSTRLEN);
        printf("\t\tMulticast IPv6 Address: %s\n", ipAddress);
    }

    ma = ma->Next;
}

void socketAPI::printDnsServerAddress(PIP_ADAPTER_DNS_SERVER_ADDRESS da) {
    char ipAddress[INET6_ADDRSTRLEN];
    
    if (da->Address.lpSockaddr->sa_family == AF_INET) {
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)da->Address.lpSockaddr;
        inet_ntop(AF_INET, &(ipv4->sin_addr), ipAddress, INET_ADDRSTRLEN);
        printf("\t\tDNS Server IPv4 Address: %s\n", ipAddress);
    } else if (da->Address.lpSockaddr->sa_family == AF_INET6) {
        struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)da->Address.lpSockaddr;
        inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipAddress, INET6_ADDRSTRLEN);
        printf("\t\tDNS Server IPv6 Address: %s\n", ipAddress);
    }

    da = da->Next;
}

void socketAPI::printPrefix(PIP_ADAPTER_PREFIX prefix) {
    char ipAddress[INET6_ADDRSTRLEN];
    
    if (prefix->Address.lpSockaddr->sa_family == AF_INET) {
        struct sockaddr_in* ipv4 = (struct sockaddr_in*)prefix->Address.lpSockaddr;
        inet_ntop(AF_INET, &(ipv4->sin_addr), ipAddress, INET_ADDRSTRLEN);
        printf("\t\tPrefix IPv4 Address: %s\n", ipAddress);
    } else if (prefix->Address.lpSockaddr->sa_family == AF_INET6) {
        struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)prefix->Address.lpSockaddr;
        inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipAddress, INET6_ADDRSTRLEN);
        printf("\t\tPrefix IPv6 Address: %s\n", ipAddress);
    }
    
    printf("\t\tPrefix Length: %lu\n", prefix->PrefixLength);
}

// default to unspecified address family (both)
// 4: AF_INET, 6: AF_INET6, A: AF_UNSPEC
void socketAPI::outputAdapterInfo(ULONG family) {
    /* Declare and initialize variables */

    DWORD dwRetVal = 0;

    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

    LPVOID lpMsgBuf = NULL;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    ULONG Iterations = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
    IP_ADAPTER_DNS_SERVER_ADDRESS *pDnServer = NULL;
    IP_ADAPTER_PREFIX *pPrefix = NULL;

    printf("Calling GetAdaptersAddresses function with family = ");
    if (family == AF_INET)
        printf("AF_INET\n");
    if (family == AF_INET6)
        printf("AF_INET6\n");
    if (family == AF_UNSPEC)
        printf("AF_UNSPEC\n\n");

    // Allocate a 15 KB buffer to start with.
    outBufLen = WORKING_BUFFER_SIZE;

    do {

        pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);
        if (pAddresses == NULL) {
            printf
                ("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
            exit(1);
        }

        dwRetVal =
            GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            FREE(pAddresses);
            pAddresses = NULL;
        } else {
            break;
        }

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    if (dwRetVal == NO_ERROR) {
        // If successful, output some information from the data we received
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            printf("\tLength of the IP_ADAPTER_ADDRESS struct: %ld\n",
                   pCurrAddresses->Length);
            printf("\tIfIndex (IPv4 interface): %lu\n", pCurrAddresses->IfIndex);
            printf("\tAdapter name: %s\n", pCurrAddresses->AdapterName);

            pUnicast = pCurrAddresses->FirstUnicastAddress;
            if (pUnicast != NULL) {
                for (i = 0; pUnicast != NULL; i++) {
                    // printUnicastAddress(pUnicast);
                    pUnicast = pUnicast->Next;
                }
                printf("\tNumber of Unicast Addresses: %d\n", i);
            } else
                printf("\tNo Unicast Addresses\n");

            pAnycast = pCurrAddresses->FirstAnycastAddress;
            if (pAnycast) {
                for (i = 0; pAnycast != NULL; i++) {
                    // printAnycastAddress(pAnycast);
                    pAnycast = pAnycast->Next;
                }
                printf("\tNumber of Anycast Addresses: %d\n", i);
            } else
                printf("\tNo Anycast Addresses\n");

            pMulticast = pCurrAddresses->FirstMulticastAddress;
            if (pMulticast) {
                for (i = 0; pMulticast != NULL; i++) {
                    // printMulticastAddress(pMulticast);
                    pMulticast = pMulticast->Next;
                }
                printf("\tNumber of Multicast Addresses: %d\n", i);
            } else
                printf("\tNo Multicast Addresses\n");

            pDnServer = pCurrAddresses->FirstDnsServerAddress;
            if (pDnServer) {
                for (i = 0; pDnServer != NULL; i++) {
                    // printDnsServerAddress(pDnServer);
                    pDnServer = pDnServer->Next;
                }
                printf("\tNumber of DNS Server Addresses: %d\n", i);
            } else
                printf("\tNo DNS Server Addresses\n");

            printf("\tDNS Suffix: %S\n", pCurrAddresses->DnsSuffix);
            printf("\tDescription: %S\n", pCurrAddresses->Description);
            printf("\tFriendly name: %S\n", pCurrAddresses->FriendlyName);

            if (pCurrAddresses->PhysicalAddressLength != 0) {
                printf("\tPhysical address: ");
                for (i = 0; i < (int) pCurrAddresses->PhysicalAddressLength;
                     i++) {
                    if (i == (pCurrAddresses->PhysicalAddressLength - 1))
                        printf("%.2X\n",
                               (int) pCurrAddresses->PhysicalAddress[i]);
                    else
                        printf("%.2X-",
                               (int) pCurrAddresses->PhysicalAddress[i]);
                }
            }
            printf("\tFlags: %ld\n", pCurrAddresses->Flags);
            printf("\tMtu: %lu\n", pCurrAddresses->Mtu);
            printf("\tIfType: %ld\n", pCurrAddresses->IfType);
            printf("\tOperStatus: %ld\n", pCurrAddresses->OperStatus);
            printf("\tIpv6IfIndex (IPv6 interface): %u\n",
                   pCurrAddresses->Ipv6IfIndex);
            printf("\tZoneIndices (hex): ");
            for (i = 0; i < 16; i++)
                printf("%lx ", pCurrAddresses->ZoneIndices[i]);
            printf("\n");

            printf("\tTransmit link speed: %I64u\n", pCurrAddresses->TransmitLinkSpeed);
            printf("\tReceive link speed: %I64u\n", pCurrAddresses->ReceiveLinkSpeed);

            pPrefix = pCurrAddresses->FirstPrefix;
            if (pPrefix) {
                for (i = 0; pPrefix != NULL; i++)
                    pPrefix = pPrefix->Next;
                printf("\tNumber of IP Adapter Prefix entries: %d\n", i);
            } else
                printf("\tNumber of IP Adapter Prefix entries: 0\n");

            printf("\n");

            pCurrAddresses = pCurrAddresses->Next;
        }
    } else {
        printf("Call to GetAdaptersAddresses failed with error: %d\n",
               dwRetVal);
        if (dwRetVal == ERROR_NO_DATA)
            printf("\tNo addresses were found for the requested parameters\n");
        else {

            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                    NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),   
                    // Default language
                    (LPTSTR) & lpMsgBuf, 0, NULL)) {
                printf("\tError: %s", lpMsgBuf);
                LocalFree(lpMsgBuf);
                if (pAddresses)
                    FREE(pAddresses);
                exit(1);
            }
        }
    }

    if (pAddresses) {
        FREE(pAddresses);
    }
}

std::pair<std::string, std::string> socketAPI::getUnicastAddress(PIP_ADAPTER_UNICAST_ADDRESS &ua) {
    if (ua->Address.lpSockaddr->sa_family == AF_INET) {
        char ipAddress[INET_ADDRSTRLEN];
        char subnetMask[INET_ADDRSTRLEN];

        struct sockaddr_in* ipv4 = (struct sockaddr_in*)ua->Address.lpSockaddr;
        inet_ntop(AF_INET, &(ipv4->sin_addr), ipAddress, INET_ADDRSTRLEN);

        ULONG prefixLength = ua->OnLinkPrefixLength; 
        ULONG mask = (0xFFFFFFFF << (32 - prefixLength)) & 0xFFFFFFFF; 
        struct in_addr mask_addr; 
        mask_addr.s_addr = htonl(mask); 
        inet_ntop(AF_INET, &mask_addr, subnetMask, INET_ADDRSTRLEN);

        return std::pair<std::string, std::string>(ipAddress, subnetMask);
    } else {
        return std::pair<std::string, std::string>();
    }
}

std::vector<std::pair<std::string, std::string>> socketAPI::getAllAddress(ULONG family)
{
    DWORD dwRetVal = 0;
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    LPVOID lpMsgBuf = NULL;
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    ULONG Iterations = 0;
    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;

    outBufLen = WORKING_BUFFER_SIZE;
    do {
        pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);
        if (pAddresses == NULL) {
            return std::vector<std::pair<std::string, std::string>>();
        }

        dwRetVal =
            GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            FREE(pAddresses);
            pAddresses = NULL;
        } else {
            break;
        }

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    std::vector<std::pair<std::string, std::string>> result;
    if (dwRetVal == NO_ERROR) {
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            pUnicast = pCurrAddresses->FirstUnicastAddress;
            while (pUnicast != NULL) {
                result.push_back(getUnicastAddress(pUnicast));
                pUnicast = pUnicast->Next;
            } 
            pCurrAddresses = pCurrAddresses->Next;
        }
    } else {
        if (dwRetVal != ERROR_NO_DATA) {
            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                    NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),   
                    (LPTSTR) & lpMsgBuf, 0, NULL)) {
                printf("\tError: %s", lpMsgBuf);
                LocalFree(lpMsgBuf);
                if (pAddresses)
                    FREE(pAddresses);
            }
        }
    }

    if (pAddresses) {
        FREE(pAddresses);
    }
    return result;
}

std::string socketAPI::getAvailableIP() {
    std::vector<std::pair<std::string, std::string>> listIP = getAllAddress(AF_INET);
    return listIP[rand() % (int)listIP.size()].first;
}

uint32_t socketAPI::getBinaryAvailableIP() {
    std::vector<std::pair<std::string, std::string>> listIP = getAllAddress(AF_INET);
    return getBinaryIP((char*)listIP[rand() % (int)listIP.size()].first.c_str());
}

std::string socketAPI::findSuitableIP(std::string IP_sender, std::string subnetMask) {
    std::vector<std::pair<std::string, std::string>> listIP = getAllAddress(AF_INET);
    for (auto &[hostIP, host_subnetMask] : listIP) {
        // std::cout << hostIP << ' ' << host_subnetMask << '\n';
        if (host_subnetMask == subnetMask && socketAPI::sameSubnet((char*)IP_sender.c_str(), (char*)hostIP.c_str(), (char*)subnetMask.c_str())) {
            return hostIP;
        }
    }
    return std::string();
}

