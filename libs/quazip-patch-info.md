create the patch for the qmake project file
```sh
$ diff -u quazip-0.7.1_orig/quazip/quazip.pro quazip-0.7.1/quazip/quazip.pro > quazip-0.7.1.patch
```

apply the patch
```sh
$ cd quazip-0.7.1/quazip
$ patch < quazip-0.7.1.patch
```
