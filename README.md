# File-System-Sim

Working with FUSE to build my own file systems.

Implement in C an in-memory file system simulator with a flat linear directory structure and a direct and indexed space allocation. Need a storage that is an array of 2^16 blocks of 256 bytes each; a block is just a plain sequence of bytes until it is overlaid with a structure (use a union, of course): directory or file meta-data, an index node, or a data node; there is also a type of the node.
Integrate all my work on the file system with FUSE. 
Using source code by Don Ownes for hash functions. 
http://viewsvn1.zanavi.cc/viewvc.cgi/zanavi_public1/navit/navit/maptool/cfuhash.c?view=co

Not finish.
