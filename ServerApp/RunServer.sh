#!/bin/bash

MACHINE_IP="192.168.94.1"   # gdzie ma byc kierowane polaczenie
PRIVATE_INTERFACE="eth0"    # nazwa interfejsu uzywanego do polaczenia z kontrolowanym urzadzeniem
PUBLIC_INTERFACE="ham0"     # nazwa interfejsu uzywanego do polaczenia ze swiatem
PORT="8888"                   # przekierowywany port

if [ $(cat /proc/sys/net/ipv4/ip_forward) -eq 1 ]; then # sprawdz, czy dozwolone sa przekierowania
    sudo iptables -F                    # resetowanie iptables
    sudo iptables -t nat -F             # ---------||---------
    sudo iptables -P FORWARD DROP
    sudo iptables -A FORWARD -s $MACHINE_IP -j ACCEPT
    sudo iptables -t nat -A PREROUTING -i $PUBLIC_INTERFACE -p tcp --dport $PORT -j DNAT --to $MACHINE_IP:$PORT   # kieruj wszystkie polaczenia z danego portu na port kontrolowanej maszyny
    sudo iptables -t nat -A POSTROUTING -o $PRIVATE_INTERFACE -j MASQUERADE # maskuj te polaczenia jako wychodzace z tej maszyny (potrzebne do response'ow)
    cd Debug
    sudo ./ServerApp                    # odpal serwer
    cd ..
    sudo iptables -F                    # \
    sudo iptables -t nat -F             # -zresetuj iptables po skonczeniu dzialania
    sudo iptables -P FORWARD DROP       # /
else
    echo "Packet forwarding disabled."
    echo "Enable it by changing net.ipv4.ip_forward to 1 in /etc/sysctl.conf."
    echo "Then restart the system"
fi
