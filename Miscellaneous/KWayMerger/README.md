# K-Way Merger

A Go implementation of the K-way merge algorithm as a reusable library for efficiently merging multiple sorted files into a single sorted output file. This implementation uses a generic heap data structure to achieve optimal performance.

## Overview

The K-Way Merger library provides functionality to:

1. Read multiple input files containing sorted data
2. Merge the sorted files into a single output file using a min-heap
3. Support for different data types (int32, int64, string)
4. Customizable parsing and formatting of data
5. Error handling throughout the process

This library is particularly useful for scenarios where you need to merge sorted data from multiple sources, such as in external sorting for large datasets that don't fit into memory.

## Architecture

The library consists of the following main components:

1. **app package**: Contains the core logic for reading, sorting, and merging files
   - `ParseFunc` and `FormatFunc`: Function types for custom data parsing and formatting
   - `readSortRewrite`: Reads a file, sorts its data, and rewrites the sorted data
   - `mergeAndWrite`: Merges multiple sorted files into one using a min-heap
   - `Run`: Orchestrates the sorting and merging process

2. **heap package**: Implements a generic heap data structure for efficient merging
   - `Node`: Represents a file and its current value
   - `Heap`: Implements the heap interface for sorting nodes by value based on a custom comparator
   - Generic implementation supporting different data types

3. **main.go**: Example application demonstrating library usage

## Usage

### As a Library

To use the K-Way Merger in your Go project, import the package and use its API:

```go
package main

import (
	"KWayMerger/app"
	"fmt"
	"log"
	"strconv"
)

// parseInt32 parses a string into an int32.
func parseInt32(s string) (int32, error) {
	val, err := strconv.ParseInt(s, 10, 32)
	return int32(val), err
}

// formatInt32 formats an int32 into a string.
func formatInt32(i int32) string {
	return strconv.FormatInt(int64(i), 10)
}

// compareInt32 compares two int32 values in ascending order.
func compareInt32(a, b int32) bool {
	return a < b
}

func main() {
	// Define input and output files
	inputFiles := []string{"input1.txt", "input2.txt", "input3.txt"}
	outputFile := "output.txt"

	// Run the K-Way Merger for int32 data
	fmt.Println("Merging files...")
	err := app.Run[int32](inputFiles, outputFile, parseInt32, formatInt32, compareInt32)
	if err != nil {
		log.Fatalf("Error running K-Way Merger: %v", err)
	}
	fmt.Printf("Successfully merged %v files into %v\n", len(inputFiles), outputFile)
}
```

### Example Application

The repository includes an example application that demonstrates library usage:

```shell
# Build the example application
go build -o kwaymerger main.go

# Run with input files and output file
./kwaymerger [file1 file2 ... fileN] [outputFile]
```

Example:

```shell
./kwaymerger input1.txt input2.txt input3.txt output.txt
```

### Docker

To build and run the example application using Docker:

```shell
# Build the Docker image
 docker build --no-cache -t kwaymerger .

# Run the container with input files mounted
 docker run -v /path/to/input/files:/data kwaymerger /data/file1.txt /data/file2.txt /data/output.txt
```

## Running Tests

To run the unit tests:

```shell
# Run all tests
 go test -v ./test

# Run with Docker
 docker run kwaymerger go test -v ./test
```

## Performance Considerations

- The algorithm efficiently merges K sorted files using a generic heap with a time complexity of O(N log K), where N is the total number of elements
- Memory usage is optimized by processing files sequentially and using a generic heap of size K
- Generic implementation allows for type-safe usage with different data types

## Requirements

* Go 1.22 or higher

## References

* [K-way merge algorithm](https://en.wikipedia.org/wiki/K-way_merge_algorithm)
* [External merge sort](https://en.wikipedia.org/wiki/External_sorting)
* [Merge k Sorted Lists](https://leetcode.com/problems/merge-k-sorted-lists)
* [Kth Smallest Element in a Sorted Matrix](https://leetcode.com/problems/kth-smallest-element-in-a-sorted-matrix)
