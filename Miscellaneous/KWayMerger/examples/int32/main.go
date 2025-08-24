// This example demonstrates how to use the K-Way Merger library with int32 values.
// It reads integers from multiple input files, sorts each file, and merges them
// into a single sorted output file using a min-heap.
package main

import (
	"KWayMerger/app"
	"fmt"
	"os"
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
	// Define input files and output file
	inputFiles := []string{
		"input1.txt",
		"input2.txt",
		"input3.txt",
	}
	outputFile := "merged_output.txt"

	// Run the K-Way Merger using generic implementation
	fmt.Println("Merging files...")
	if err := app.Run(inputFiles, outputFile, parseInt32, formatInt32, compareInt32); err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}
	fmt.Printf("Successfully merged %v files into %v\n", len(inputFiles), outputFile)
}