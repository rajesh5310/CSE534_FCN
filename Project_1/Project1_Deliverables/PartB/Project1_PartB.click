es  :: ListenEtherSwitch ;

FromDevice(eth0) ->  [0]es[0] -> Queue -> ToDevice(eth0);
FromDevice(eth1) ->  [1]es[1] -> Queue -> ToDevice(eth1);
FromDevice(eth2) ->  [2]es[2] -> Queue -> ToDevice(eth2);
es[3] -> Print("Listen") -> Discard;
