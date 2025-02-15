# B-tree file organization
<div align="justify">Purpose of this project is to implement indexed file organization using B-tree. It allows to add, update and search for records in file easily, as number of accesses to file with data is reduced. Program can be tested by entering data manually or by reading data from included with project input file.</div>

### Implementation
<div align="justify">When new record is added to data file, it's index is added to B-tree. This helps to find record efficiently when it is necessary. There are two binary files used to store data: data file that stores trapeziums data (their bases, height and index) and index file that stores indexes and reference to place in data file where particular record is located. Storing B-tree nodes in buffer which size is current height of the tree optimizes searching through index, as recently loaded nodes are stored in memory. Implementation allows changing order of the B-tree and printing structure of index and sorted data file.</div>
