[![Build Status](https://travis-ci.org/rizkg/BBHash.svg?branch=master)](https://travis-ci.org/rizkg/BBHash)

# BBHash
**BBHash** is a simple library for building minimal perfect hash function.
It is designed to handle large scale datasets. The function is just a little bit larger than other state-of-the-art libraries, it takes approximately 3 bits / elements (compared to 2.62 bits/elem for the emphf lib), but construction is faster and does not require additional memory. 

It is easy to include in other projects (just include a single .h file) and has no dependencies.

# Citation

A. Limasset, G. Rizk, R. Chikhi, P. Peterlongo, _Fast and Scalable Minimal Perfect Hashing for Massive Key Sets_, SEA 2017: http://drops.dagstuhl.de/opus/volltexte/2017/7619/pdf/LIPIcs-SEA-2017-25.pdf

```
@InProceedings{bbhash,
  author ={Antoine Limasset and Guillaume Rizk and Rayan Chikhi and Pierre Peterlongo},
  title ={{Fast and Scalable Minimal Perfect Hashing for Massive Key Sets}},
  booktitle ={16th International Symposium on Experimental Algorithms (SEA 2017)},
  pages ={25:1--25:16},
  series ={Leibniz International Proceedings in Informatics (LIPIcs)},
  year ={2017},
  volume ={75},
  ISSN ={1868-8969},
  publisher ={Schloss Dagstuhl--Leibniz-Zentrum fuer Informatik},
  address ={Dagstuhl, Germany},
  URL ={http://drops.dagstuhl.de/opus/volltexte/2017/7619},
  doi ={10.4230/LIPIcs.SEA.2017.25}
}
```

# Modification
The BBHash source code is copied here without any modification.

Use testTime.cpp as the default benchmark program for input as a folder.
For example, FOLDER_PATH="../Input_data"

```
make
./testTime FOLDER_PATH/
```

Above commands will test the input files in FOLDER_PATH, and provide the time and space result.

