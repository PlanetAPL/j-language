NB. do not edit -- created by sym2ijs
cocurrent<'jdefs'
cocurrent <'jdefs'

FIONBIO =: -2147195266
FIONREAD =: 1074030207
FD_SETSIZE=: 1024

sockaddr_sz=: 16
sa_family_off=: 0
sa_family_sz=: 2
sa_data_off=: 2
sa_data_sz=: 14

sockaddr_in_sz=: 16
sin_family_off=: 0
sin_family_sz=: 2
sin_port_off=: 2
sin_port_sz=: 2
sin_addr_off=: 4
sin_addr_sz=: 4

in_addr_sz=: 4
s_addr_off=: 0
s_addr_sz=: 4

hostent_sz=: 20
h_name_off=: 0
h_name_sz=: 4
h_aliases_off=: 4
h_aliases_sz=: 4
h_addrtype_off=: 8
h_addrtype_sz=: 4
h_length_off=: 12
h_length_sz=: 4
h_addr_list_off=: 16
h_addr_list_sz=: 4




IPPROTO_IP=: 0
IPPROTO_ICMP=: 1
IPPROTO_IGMP=: 2
IPPROTO_TCP=: 6
IPPROTO_PUP=: 12
IPPROTO_UDP=: 17
IPPROTO_IDP=: 22
IPPROTO_RAW=: 255
IPPROTO_MAX=: 256


INADDR_ANY=: 0
INADDR_LOOPBACK=: 2130706433
INADDR_BROADCAST=: -1
INADDR_NONE=: _1

INADDR_UNSPEC_GROUP=: -536870912
INADDR_ALLHOSTS_GROUP=: -536870911
INADDR_MAX_LOCAL_GROUP=: -536870657
IN_LOOPBACKNET=: 127

SOCK_STREAM=: 2
SOCK_DGRAM=: 1
SOCK_RAW=: 4
SOCK_RDM=: 5
SOCK_SEQPACKET=: 6

SOL_SOCKET=: 65535
SO_DEBUG=: 1
SO_REUSEADDR=: 4
SO_KEEPALIVE=: 8
SO_DONTROUTE=: 16
SO_BROADCAST=: 32
SO_LINGER=: 128
SO_OOBINLINE=: 256

SO_SNDBUF=: 4097
SO_RCVBUF=: 4098
SO_SNDLOWAT=: 4099
SO_RCVLOWAT=: 4100
SO_SNDTIMEO=: 4101
SO_RCVTIMEO=: 4102
SO_ERROR=: 4103
SO_TYPE=: 4104

linger_sz=: 8
l_onoff_off=: 0
l_onoff_sz=: 4
l_linger_off=: 4
l_linger_sz=: 4

AF_UNSPEC=: 0
AF_UNIX=: 1
AF_INET=: 2
AF_SNA=: 11
AF_DECnet=: 12
AF_APPLETALK=: 16
AF_IPX=: 23
AF_MAX=: 25
PF_UNSPEC=: 0
PF_UNIX=: 1
PF_INET=: 2
PF_SNA=: 11
PF_DECnet=: 12
PF_APPLETALK=: 16
PF_IPX=: 23
PF_MAX=: 25

SOMAXCONN=: 5
MSG_OOB=: 1
MSG_PEEK=: 2
MSG_DONTROUTE=: 4

msghdr_sz=: 24
msg_name_off=: 0
msg_name_sz=: 4
msg_namelen_off=: 4
msg_namelen_sz=: 4
msg_iov_off=: 8
msg_iov_sz=: 4
msg_iovlen_off=: 12
msg_iovlen_sz=: 4
msg_accrights_off=: 16
msg_accrights_sz=: 4
msg_accrightslen_off=: 20
msg_accrightslen_sz=: 4

