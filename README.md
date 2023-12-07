# Two-way Min-cut Partitioning
Implement Fiduccia–Mattheyses algorithm to solve the problem of two-way min-cut partitioning.

## How to Compile
In `CS613500-Two-way-min-cut-partitioning/src/`, enter the following command:
```
$ make
```
An executable file `hw2` will be generated in `CS613500-Two-way-min-cut-partitioning/bin/`.

If you want to remove it, please enter the following command:
```
$ make clean
```

## How to Run
Usage: 
```
$ ./hw2 <input file> <output file>
```

E.g., in `CS613500-Two-way-min-cut-partitioning/bin/`
```
$ ./hw2 ../testcase/sample.txt ../output/sample.out

```

## How to Test
In `Two-way_Min-cut_Partitioning/src/`, enter the following command:
```
$ make test $name
```
It will build an executable file, test on testcase `$name`, and run a verifier to verify the answer.

E.g., test on sample and verify the answer.
```
$ make test sample
```

