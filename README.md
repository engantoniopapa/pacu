# pacu
Replicated Transactional File System for Distributed Systems

Applicative network file system with Client-Server architecture, that uses the replication protocol
primary-backup. The file system space is divided into domains and every server has to manage one
or more domains (primary) and has to be the replica of the other. The systems that use this file
system are tolerant regard: omission failure (client side), fail-stop failure(client and server side)
and Byzantine failure (client side). The entire project was developed in C language and uses the
SQLite DBMS to manage the configuration and the log of the system. Furthermore, the system
is tested on virtual network created by Netkit.
