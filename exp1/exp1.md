Tux123
if_e1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 127.16.120.0  netmask 255.255.255.0  broadcast 127.16.120.255
        ether ec:75:0c:c2:3c:ac  txqueuelen 1000  (Ethernet)
        RX packets 16  bytes 2394 (2.3 KB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 97  bytes 14206 (14.2 KB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
 
netedu@Tux123:~$ route -n
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
0.0.0.0         10.227.20.254   0.0.0.0         UG    100    0        0 if_mng
10.227.20.0     0.0.0.0         255.255.255.0   U     100    0        0 if_mng
172.16.120.0    0.0.0.0         255.255.255.0   U     0      0        0 if_e1
172.17.0.0      0.0.0.0         255.255.0.0     U     0      0        0 docker0
 
 
netedu@Tux123:~$ arp -a
? (10.227.20.8) at bc:24:11:ef:33:8e [ether] on if_mng
? (10.227.20.4) at bc:24:11:79:82:65 [ether] on if_mng
? (10.227.20.121) at <incomplete> on if_mng
? (10.227.20.213) at bc:24:11:6f:7d:9d [ether] on if_mng
? (10.227.20.124) at 5c:f9:dd:70:a5:b0 [ether] on if_mng
_gateway (10.227.20.254) at e4:8d:8c:20:25:c8 [ether] on if_mng
? (10.227.20.3) at bc:24:11:e7:5e:5b [ether] on if_mng
? (10.227.20.173) at 04:92:26:66:b5:64 [ether] on if_mng
? (10.227.20.2) at bc:24:11:f5:a2:54 [ether] on if_mng
? (10.227.20.122) at 5c:f9:dd:70:85:71 [ether] on if_mng
? (172.16.120.254) at ec:75:0c:c2:3c:75 [ether] on if_e1
 

Tux124
if_e1: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 172.16.120.254  netmask 255.255.255.0  broadcast 172.16.120.255
        ether ec:75:0c:c2:3c:75  txqueuelen 1000  (Ethernet)
        RX packets 3689  bytes 430491 (430.4 KB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 959  bytes 109776 (109.7 KB)
        TX errors 0  dropped 34 overruns 0  carrier 0  collisions 0

netedu@Tux124:~$ route -n
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
0.0.0.0         10.227.20.254   0.0.0.0         UG    100    0        0 if_mng
10.227.20.0     0.0.0.0         255.255.255.0   U     100    0        0 if_mng
172.16.120.0    0.0.0.0         255.255.255.0   U     0      0        0 if_e1
172.17.0.0      0.0.0.0         255.255.0.0     U     0      0        0 docker0
 
netedu@Tux124:~$ arp -a
? (10.227.20.121) at <incomplete> on if_mng
? (10.227.20.213) at bc:24:11:6f:7d:9d [ether] on if_mng
? (172.16.120.1) at ec:75:0c:c2:3c:ac [ether] on if_e1
? (10.227.20.173) at 04:92:26:66:b5:64 [ether] on if_mng
_gateway (10.227.20.254) at e4:8d:8c:20:25:c8 [ether] on if_mng
? (10.227.20.4) at bc:24:11:79:82:65 [ether] on if_mng
? (10.227.20.8) at bc:24:11:ef:33:8e [ether] on if_mng
? (10.227.20.123) at 5c:f9:dd:70:99:89 [ether] on if_mng
? (10.227.20.3) at bc:24:11:e7:5e:5b [ether] on if_mng
? (10.227.20.122) at 5c:f9:dd:70:85:71 [ether] on if_mng
? (10.227.20.2) at bc:24:11:f5:a2:54 [ether] on if_mng
 