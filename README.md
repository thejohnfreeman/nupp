netutils++
==========

This project contains:

- A library, `nupp`, exporting a C++ API layer over sockets.
- Executables, inspired by [inetutils][],
  implemented on top of `nupp` as an exercise for the writer.

For now, it only targets Linux.

If you want to run the `ping` or `traceroute` executables,
which open raw sockets for [ICMP][],
then you might need to [adjust permissions][1]:

```
sudo sysctl net.ipv4.ping_group_range='1000 1000'
for f in .build/output/*/bin/*; do
  if [ -f $f ]; then sudo setcap cap_net_admin,cap_net_raw=eip $f; fi
done
```

[inetutils]: https://www.gnu.org/software/inetutils/
[ICMP]: https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol

[1]: https://opennms.discourse.group/t/how-to-allow-unprivileged-users-to-use-icmp-ping/1573
