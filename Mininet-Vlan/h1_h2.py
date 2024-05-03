from mininet.net import Mininet
from mininet.cli import CLI
from mininet.link import Link

if '__main__' == __name__:

    net = Mininet() #Creating network instances

    #Add Hosts
    h1 = net.addHost('h1')
    h2 = net.addHost('h2')
    h3 = net.addHost('h3')
    h4 = net.addHost('h4')

    #Add L2 Swtich s5
    s5 = net.addHost('s5')


    #link names are auto generated in the order eth0 eth1 eth2
    #as they are added on the device
    Link(h1,s5)
    Link(h2,s5)
    Link(h3,s5)
    Link(h4,s5)
    net.build()

    #Remove default IP addreses from host's interface
    h1.cmd("ifconfig h1-eth0 0")
    h2.cmd("ifconfig h2-eth0 0")
    h3.cmd("ifconfig h3-eth0 0")
    h4.cmd("ifconfig h4-eth0 0")

    #Create a vlan 10 on Switch s5
    s5.cmd("brct1 addbr vlan10")
    #Bring up the vlan
    s5.cmd("ifconfig vlan10 up")
    s5.cmd("ifconfig vlan20 up")

    #Add s5-eth0 to vlan10 on L2 switch in Access Mode
    #Add s5
    s5.cmd("brct1 addif vlan10 s5-eth0")
    s5.cmd("brct1 addif vlan10 s5-eth1")
    s5.cmd("brct1 addif vlan20 s5-eth2")
    s5.cmd("brct1 addif vlan20 s5-eth3")


    #Assign IP address to Hosts as usual
    h1.cmd("ifconfig h1-eth0 10.0.10.1 netmask 255.255.255.0")
    h2.cmd("ifconfig h2-eth0 10.0.10.2 netmask 255.255.255.0")
    h3.cmd("ifconfig h3-eth0 10.0.20.1 netmask 255.255.255.0")
    h4.cmd("ifconfig h4-eth0 10.0.20.2 netmask 255.255.255.0")

    #Add default routes
    h1.cmd("ip route add default via 10.0.10.254 dev h1-eth0")
    h2.cmd("ip route add default via 10.0.10.254 dev h2-eth0")
    h3.cmd("ip route add default via 10.0.20.254 dev h3-eth0")
    h4.cmd("ip route add default via 10.0.20.254 dev h4-eth0")

    #Start Mininet prompt
    CLI(net)
    net.stop()
