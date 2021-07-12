/*
 * This file is part of ABCDK.
 * 
 * MIT License
 * 
 */
#include "socket.h"

int abcdk_gethostbyname(const char *name, sa_family_t family, abcdk_sockaddr_t *addrs, int max, char canonname[1000])
{
    struct addrinfo *results = NULL;
    struct addrinfo *it = NULL;
    struct addrinfo hint = {0};
    int chk;
    int count = 0;

    assert(name != NULL && (family == ABCDK_IPV4 || family == ABCDK_IPV6) && addrs != NULL && max > 0);

    hint.ai_flags = AI_ADDRCONFIG | AI_CANONNAME;
    hint.ai_family = family;

    chk = getaddrinfo(name, NULL, &hint, &results);
    if (chk != 0 || results == NULL)
        return -1;

    for (it = results; it != NULL && count < max; it = it->ai_next)
    {
        if (it->ai_socktype != SOCK_STREAM || it->ai_protocol != IPPROTO_TCP)
            continue;

        if (it->ai_addr->sa_family != ABCDK_IPV4 && it->ai_addr->sa_family != ABCDK_IPV6)
            continue;

        memcpy(&addrs[count++], it->ai_addr, it->ai_addrlen);
    }

    if (canonname && results->ai_canonname)
        strncpy(canonname,results->ai_canonname, 1000);

    freeaddrinfo(results);

    return count;
}

int abcdk_inet_pton(const char *name, sa_family_t family, abcdk_sockaddr_t *addr)
{
    int chk = -1;

    assert(name != NULL && (family == ABCDK_IPV4 || family == ABCDK_IPV6) && addr != NULL);

    /*bind family*/
    addr->family = family;

    if (addr->family == ABCDK_IPV4)
        chk = (inet_pton(family, name, &addr->addr4.sin_addr) == 1 ? 0 : -1);
    if (addr->family == ABCDK_IPV6)
        chk = (inet_pton(family, name, &addr->addr6.sin6_addr) == 1 ? 0 : -1);

    return chk;
}

char *abcdk_inet_ntop(const abcdk_sockaddr_t *addr, char *name, size_t max)
{
    assert(addr != NULL && name != NULL && max > 0);
    assert(addr->family == ABCDK_IPV4 || addr->family == ABCDK_IPV6);
    assert((addr->family == ABCDK_IPV4) ? (max >= INET_ADDRSTRLEN) : 1);
    assert((addr->family == ABCDK_IPV6) ? (max >= INET6_ADDRSTRLEN) : 1);

    if (addr->family == ABCDK_IPV4)
        return (char *)inet_ntop(addr->family, &addr->addr4.sin_addr, name, max);
    if (addr->family == ABCDK_IPV6)
        return (char *)inet_ntop(addr->family, &addr->addr6.sin6_addr, name, max);

    return NULL;
}

int abcdk_ifname_fetch(abcdk_ifaddrs_t *addrs, int max, int ex_loopback)
{
    struct ifaddrs *results = NULL;
    struct ifaddrs *it = NULL;
    abcdk_ifaddrs_t *p = NULL;
    int chk;
    int count = 0;

    assert(addrs != NULL && max > 0);

    chk = getifaddrs(&results);
    if (chk != 0 || results == NULL)
        return -1;

    for (it = results; it != NULL && count < max; it = it->ifa_next)
    {
        if (it->ifa_addr == NULL)
            continue;

        if (it->ifa_addr->sa_family != ABCDK_IPV4 && it->ifa_addr->sa_family != ABCDK_IPV6)
            continue;

        if ((it->ifa_flags & IFF_LOOPBACK) && ex_loopback)
            continue;

        p = &addrs[count++];

        strncpy(p->name, it->ifa_name, IFNAMSIZ);

        if (ABCDK_IPV4 == it->ifa_addr->sa_family)
        {
            memcpy(&p->addr, it->ifa_addr, sizeof(struct sockaddr_in));
            memcpy(&p->mark, it->ifa_netmask, sizeof(struct sockaddr_in));

            if (it->ifa_flags & IFF_BROADCAST)
                memcpy(&p->broa, it->ifa_broadaddr, sizeof(struct sockaddr_in));
            else
                p->broa.family = PF_UNSPEC;
        }
        else if (ABCDK_IPV6 == it->ifa_addr->sa_family)
        {
            memcpy(&p->addr, it->ifa_addr, sizeof(struct sockaddr_in6));
            memcpy(&p->mark, it->ifa_netmask, sizeof(struct sockaddr_in6));

            /*IPv6 not support. */
            p->broa.family = PF_UNSPEC;
        }
    }

    freeifaddrs(results);

    return count;
}

int abcdk_socket_ioctl(uint32_t cmd, void *args)
{
    int sock = -1;
    int chk;

    assert(cmd != 0 && args != NULL);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1)
        return -1;

    chk = ioctl(sock, cmd, args);

final:

    abcdk_closep(&sock);

    return chk;
}

char *abcdk_mac_fetch(const char *ifname, char addr[12])
{
    struct ifreq args = {0};
    int chk;

    assert(ifname != NULL && addr != NULL);

    strncpy(args.ifr_ifrn.ifrn_name, ifname, IFNAMSIZ);

    chk = abcdk_socket_ioctl(SIOCGIFHWADDR, &args);
    if (chk == -1)
        return NULL;

    for (int i = 0; i < 6; i++)
        sprintf(addr + 2 * i, "%02X", (uint8_t)args.ifr_hwaddr.sa_data[i]);

    return addr;
}

int abcdk_socket_option(int fd,int level, int name,void *data,int *len,int direction)
{
    assert(fd >= 0 && level >= 0 && name > 0 && data != NULL && len != NULL && (direction == 1 || direction == 2));

    if(direction == 1)
        return getsockopt(fd,level,name,data,len);
    
    return setsockopt(fd,level,name,data,*len);
}

int abcdk_sockopt_option_int(int fd,int level, int name,int *flag,int direction)
{
    socklen_t len = sizeof(int);

    assert(fd >= 0 && level >= 0 && name > 0 && flag != NULL && (direction == 1 || direction == 2));

    return abcdk_socket_option(fd,level,name,flag,&len,direction);
}

int abcdk_sockopt_option_timeout(int fd,int name, struct timeval *tv,int direction)
{
    socklen_t len = sizeof(struct timeval);

    assert(fd >= 0 && name > 0 && tv != NULL && (direction == 1 || direction == 2));

    return abcdk_socket_option(fd,SOL_SOCKET,name,tv,&len,direction);
}

int abcdk_socket_option_linger(int fd,struct linger *lg,int direction)
{
    socklen_t len = sizeof(struct linger);

    assert(fd >= 0 && lg != NULL && (direction == 1 || direction == 2));

    return abcdk_socket_option(fd,SOL_SOCKET,SO_LINGER,lg,&len,direction);  
}

int abcdk_socket_option_multicast(int fd,abcdk_sockaddr_t *multiaddr, const char *ifaddr,int enable)
{
    socklen_t len = sizeof(struct ip_mreq);
    socklen_t len6 = sizeof(struct ipv6_mreq);
    struct ip_mreq st_mreq = {0};
    struct ipv6_mreq st_mreq6 = {0};
    int name;
    int chk;

    assert(fd >= 0 && multiaddr != NULL);

    assert(multiaddr->family == ABCDK_IPV4 || multiaddr->family == ABCDK_IPV6);

    if(multiaddr->family == ABCDK_IPV4)
    {
        st_mreq.imr_multiaddr = multiaddr->addr4.sin_addr;
        st_mreq.imr_interface.s_addr = (ifaddr ? inet_addr(ifaddr) : INADDR_ANY);

        name = (enable ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP);

        chk = abcdk_socket_option(fd,IPPROTO_IP,name,&st_mreq,&len,2);  
    }
    else if(multiaddr->family == ABCDK_IPV6)
    {
        st_mreq6.ipv6mr_multiaddr = multiaddr->addr6.sin6_addr;
        st_mreq6.ipv6mr_interface = (ifaddr ? if_nametoindex(ifaddr) : 0);

        name = (enable ? IPV6_JOIN_GROUP : IPV6_LEAVE_GROUP);

        chk = abcdk_socket_option(fd,IPPROTO_IP,name,&st_mreq,&len,2); 
    }

    return chk;
}

int abcdk_socket(sa_family_t family, int udp)
{
    int type = SOCK_CLOEXEC;

    assert(family == ABCDK_IPV4 || family == ABCDK_IPV6);

    type |= (udp ? SOCK_DGRAM : SOCK_STREAM);

    return socket(family, type, 0);
}

int abcdk_bind(int fd, const abcdk_sockaddr_t *addr)
{
    assert(fd >= 0 && addr != NULL);

    return bind(fd, &addr->addr, sizeof(abcdk_sockaddr_t));
}

int abcdk_accept(int fd, abcdk_sockaddr_t *addr)
{
    int sub_fd = -1;
    socklen_t addrlen = sizeof(abcdk_sockaddr_t);

    assert(fd >= 0);

    if (addr)
        sub_fd = accept(fd, &addr->addr, &addrlen);
    else
        sub_fd = accept(fd, NULL, NULL);

    if (sub_fd < 0)
        return -1;

    /* 添加个非必要标志，忽略可能的出错信息。 */
    abcdk_fflag_add(sub_fd, O_CLOEXEC);

    return sub_fd;
}

int abcdk_connect(int fd, abcdk_sockaddr_t *addr, time_t timeout)
{
    int flags = 0;
    int eno = 0;
    int chk;

    assert(fd >= 0 && addr != NULL);

    flags = abcdk_fflag_get(fd);
    if (flags == -1)
        return -1;

    /* 添加非阻塞标志，用于异步连接。*/
    chk = 0;
    if (!(flags & O_NONBLOCK))
        chk = abcdk_fflag_add(fd, O_NONBLOCK);

    if (chk != 0)
        return -1;

    chk = connect(fd, &addr->addr, sizeof(abcdk_sockaddr_t));
    if(chk == 0)
        goto final;

    if (errno != EINPROGRESS && errno != EWOULDBLOCK && errno != EAGAIN)
        goto final;

    /* 等待写事件(允许)。 */
    chk = (abcdk_poll(fd, 0x02, timeout) > 0 ? 0 : -1);
    if(chk != 0)
        goto final;

    /* 获取SOCKET句柄的出错码。 */
    chk = abcdk_sockopt_option_int(fd, SOL_SOCKET, SO_ERROR, &eno, 1);
    chk = (eno == 0 ? 0 : -1);

final:
    
    /* 恢复原有的标志，忽略可能的出错信息。*/
    if (!(flags & O_NONBLOCK))
        abcdk_fflag_del(fd, O_NONBLOCK);

    return chk;
}

int abcdk_sockaddr_from_string(abcdk_sockaddr_t *dst, const char *src, int try_lookup)
{
    char name[68] = {0};
    uint16_t port = 0;
    int chk;

    assert(dst != NULL && src != NULL);

    if (strchr(src, '['))
    {
        dst->family = ABCDK_IPV6;
        sscanf(src, "%*[[ ]%[^] ]%*[] :,]%hu", name, &port);
    }
    else if (strchr(src, ','))
    {
        dst->family = ABCDK_IPV6;
        sscanf(src, "%[^, ]%*[, ]%hu", name, &port);
    }
    else if (strchr(src, ':'))
    {
        dst->family = ABCDK_IPV4;
        sscanf(src, "%[^: ]%*[: ]%hu", name, &port);
    }
    else
    {
        strncpy(name,src,62);
    }

    /*尝试直接转换。*/
    chk = abcdk_inet_pton(name, dst->family, dst);
    if (chk != 0 && try_lookup)
    {
        /*可能是域名。*/
        chk = (abcdk_gethostbyname(name, dst->family, dst, 1, NULL) == 1 ? 0 : -1);
    }

    /*地址转换成功后，再转换端口号。*/
    if(chk==0)
    {
        if(dst->family == ABCDK_IPV6)
            dst->addr6.sin6_port = abcdk_endian_h_to_b16(port);
        if(dst->family == ABCDK_IPV4)
            dst->addr4.sin_port = abcdk_endian_h_to_b16(port);
    }

    return chk;
}

char *abcdk_sockaddr_to_string(char dst[68],const abcdk_sockaddr_t *src)
{
    char buf[INET6_ADDRSTRLEN] = {0};

    assert(dst != NULL && src != NULL);

    assert(src->family == ABCDK_IPV4 || src->family == ABCDK_IPV6);

    if (abcdk_inet_ntop(src, buf, INET6_ADDRSTRLEN) == NULL)
        return NULL;

    if (src->family == ABCDK_IPV6)
    {
        if(src->addr6.sin6_port)
            sprintf(dst,"[%s]:%hu",buf,abcdk_endian_b_to_h16(src->addr6.sin6_port));
        else
            strcpy(dst,buf);
    }
    if (src->family == ABCDK_IPV4)
    {
        if(src->addr4.sin_port)
            sprintf(dst,"%s:%hu",buf,abcdk_endian_b_to_h16(src->addr4.sin_port));
        else
            strcpy(dst,buf);
    }

    return dst;
}

int abcdk_sockaddr_where(const abcdk_sockaddr_t *test,int where)
{
    int addr_num = 0;
    int addr_max = 100;
    abcdk_ifaddrs_t *addrs = NULL;
    abcdk_ifaddrs_t *addr_p = NULL;
    int match_num = 0;
    int chk;

    assert(test != NULL && (where ==1 || where ==2));

    addrs = (abcdk_ifaddrs_t *)abcdk_heap_alloc(sizeof(abcdk_ifaddrs_t)*addr_max);
    if(!addrs)
        ABCDK_ERRNO_AND_RETURN1(errno,0);

    addr_num = abcdk_ifname_fetch(addrs,addr_max,1);

    for (int i = 0; i < addr_num; i++)
    {
        addr_p = addrs+i;// &addrs[i]

        /*只比较同类型的地址。*/
        if(addr_p->addr.family != test->family)
            continue;

        if (addr_p->addr.family == ABCDK_IPV6)
            chk = memcmp(&addr_p->addr.addr6.sin6_addr, &test->addr6.sin6_addr, sizeof(struct in6_addr));
        if (addr_p->addr.family == ABCDK_IPV4)
            chk = memcmp(&addr_p->addr.addr4.sin_addr, &test->addr4.sin_addr, sizeof(struct in_addr));

        /*地址相同则计数。*/
        if (chk == 0)
            match_num += 1;
    }

    abcdk_heap_free2((void**)&addrs);

    if (where == 1)
        return ((match_num > 0) ? 1 : 0);
    if (where == 2)
        return ((match_num <= 0) ? 1 : 0);
}