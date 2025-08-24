// This example demonstrates how to use the K-Way Merger library with string values.
// It reads strings from multiple input files, sorts each file, and merges them
// into a single sorted output file using a min-heap.
package main

import (
	"KWayMerger/app"
	"fmt"
	"os"
)

// parseString simply returns the input string as is.
func parseString(s string) (string, error) {
	return s, nil
}

// formatString simply returns the input string as is.
func formatString(s string) string {
	return s
}

// compareString compares two strings lexicographically.
func compareString(a, b string) bool {
	return a < b
}

func main() {
	// Define input files and output file
	inputFiles := []string{
		"strings1.txt",
		"strings2.txt",
		"strings3.txt",
	}
	outputFile := "merged_strings.txt"

	// Run the K-Way Merger using generic implementation
	fmt.Println("Merging string files...")
	if err := app.Run(inputFiles, outputFile, parseString, formatString, compareString); err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}
	fmt.Printf("Successfully merged %v string files into %v\n", len(inputFiles), outputFile)
}