// Package app provides generic functionality for the K-Way Merger application.
// It handles reading input files, sorting their contents, and merging them
// using a min-heap to produce a single sorted output file with support for any type.
package app

import (
	myHeap "KWayMerger/heap"
	"bufio"
	"fmt"
	"os"
	"runtime"
	"sort"
	"sync"
)

// ParseFunc defines a function type for parsing a string into type T.
type ParseFunc[T any] func(string) (T, error)

// FormatFunc defines a function type for formatting a value of type T into a string.
type FormatFunc[T any] func(T) string

// NewNode opens the given file, reads its first value using the provided parser,
// and returns a Node[T]. If any error occurs, it closes the file before returning.
func NewNode[T any](filename string, parser ParseFunc[T]) (myHeap.Node[T], error) {
	fd, err := os.Open(filename)
	if err != nil {
		return myHeap.Node[T]{}, fmt.Errorf("open file %s: %w", filename, err)
	}

	scanner := bufio.NewScanner(fd)
	scanner.Split(bufio.ScanWords)

	if !scanner.Scan() {
		// Close on failure to read
		fd.Close()
		if scanErr := scanner.Err(); scanErr != nil {
			return myHeap.Node[T]{}, fmt.Errorf("scan file %s: %w", filename, scanErr)
		}
		return myHeap.Node[T]{}, fmt.Errorf("no value found in file %s", filename)
	}

	val, err := parser(scanner.Text())
	if err != nil {
		fd.Close()
		return myHeap.Node[T]{}, fmt.Errorf("parse value in file %s: %w", filename, err)
	}

	return myHeap.Node[T]{Val: val, Fd: fd, Scanner: scanner}, nil
}

// readSortRewrite reads values of type T from a file, sorts them using the provided comparator,
// and rewrites the sorted values back to the same file using the provided formatter.
//
// Parameters:
//
//	file - The path to the file to be read, sorted, and rewritten
//	parser - Function to parse string values into type T
//	formatter - Function to format values of type T into strings
//	cmp - Comparator function for sorting values of type T
//
// Returns:
//
//	error - Any error encountered during reading, sorting, or writing
func readSortRewrite[T any](file string, parser ParseFunc[T], formatter FormatFunc[T], cmp func(T, T) bool) error {
	// Open file for reading
	fd, err := os.Open(file)
	if err != nil {
		return fmt.Errorf("failed to open file %s: %w", file, err)
	}
	// Ensure file is closed when function exits
	defer func() {
		closeErr := fd.Close()
		if closeErr != nil && err == nil {
			err = fmt.Errorf("failed to close file %s: %w", file, closeErr)
		}
	}()

	// Read values from file
	scanner := bufio.NewScanner(fd)
	scanner.Split(bufio.ScanWords) // Split on whitespace
	var list []T
	for scanner.Scan() {
		val, parseErr := parser(scanner.Text())
		if parseErr != nil {
			return fmt.Errorf("failed to parse value in file %s: %w", file, parseErr)
		}
		list = append(list, val)
	}
	if err = scanner.Err(); err != nil {
		return fmt.Errorf("error reading file %s: %w", file, err)
	}

	// Sort the values using the provided comparator
	sort.Slice(list, func(i, j int) bool {
		return cmp(list[i], list[j])
	})

	// Open file for writing (truncate existing content)
	fd2, err := os.OpenFile(file, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, 0644)
	if err != nil {
		return fmt.Errorf("failed to open file %s for writing: %w", file, err)
	}
	// Ensure file is closed when function exits
	defer func() {
		closeErr := fd2.Close()
		if closeErr != nil && err == nil {
			err = fmt.Errorf("failed to close file %s: %w", file, closeErr)
		}
	}()

	// Write sorted values back to file
	for i := 0; i < len(list); i++ {
		_, err = fmt.Fprintf(fd2, "%s\n", formatter(list[i]))
		if err != nil {
			return fmt.Errorf("failed to write to file %s: %w", file, err)
		}
	}

	// Sync file to ensure data is written to disk
	err = fd2.Sync()
	if err != nil {
		return fmt.Errorf("failed to sync file %s: %w", file, err)
	}

	return nil
}

// mergeAndWrite merges values of type T from multiple sorted input files into a single
// sorted output file using a min-heap. It reads the smallest available value
// from each input file, adds it to the heap, and then extracts the minimum
// value to write to the output file.
//
// Parameters:
//
//	inputFiles - Slice of paths to the input files containing sorted values
//	outputFile - Path to the output file where merged sorted values will be written
//	parser - Function to parse string values into type T
//	formatter - Function to format values of type T into strings
//	cmp - Comparator function for ordering values of type T
//
// Returns:
//
//	error - Any error encountered during merging or writing
func mergeAndWrite[T any](inputFiles []string, outputFile string, parser ParseFunc[T], formatter FormatFunc[T], cmp func(T, T) bool) error {
	// Open output file for writing
	fd, err := os.OpenFile(outputFile, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, 0644)
	if err != nil {
		return fmt.Errorf("failed to open output file %s: %w", outputFile, err)
	}
	// Ensure output file is closed when function exits
	defer func() {
		closeErr := fd.Close()
		if closeErr != nil && err == nil {
			err = fmt.Errorf("failed to close output file %s: %w", outputFile, closeErr)
		}
	}()

	// Initialize min-heap with the provided comparator
	minHeap := myHeap.NewHeap(len(inputFiles), cmp)

	// Track all open files for proper cleanup
	var openFiles []*os.File
	defer func() {
		// Close all remaining open files on error
		for _, f := range openFiles {
			f.Close()
		}
	}()

	// Create nodes for each input file and add to heap
	for i := range inputFiles {
		node, newErr := NewNode(inputFiles[i], parser)
		if newErr != nil {
			return fmt.Errorf("failed to create node for file %s: %w", inputFiles[i], newErr)
		}
		openFiles = append(openFiles, node.Fd)
		minHeap.PushNode(node)
	}

	// Merge process: extract minimum value from heap and write to output
	for !minHeap.Empty() {
		node := minHeap.PopNode()
		// Write the smallest value to output file
		_, err = fmt.Fprintf(fd, "%s\n", formatter(node.Val))
		if err != nil {
			return fmt.Errorf("failed to write to output file %s: %w", outputFile, err)
		}

		// Read next value from the same file if available
		if node.Scanner.Scan() {
			val, parseErr := parser(node.Scanner.Text())
			if parseErr != nil {
				return fmt.Errorf("failed to parse value in file %s: %w", node.Fd.Name(), parseErr)
			}
			node.Val = val
			minHeap.PushNode(node) // Reinsert node with new value
		} else {
			// File is exhausted, remove from open files list
			for i, f := range openFiles {
				if f == node.Fd {
					openFiles = append(openFiles[:i], openFiles[i+1:]...)
					break
				}
			}

			// Close the file
			closeErr := node.Fd.Close()
			if closeErr != nil {
				return fmt.Errorf("failed to close file: %w", closeErr)
			}
		}
	}

	// Sync output file to ensure data is written to disk
	err = fd.Sync()
	if err != nil {
		return fmt.Errorf("failed to sync output file %s: %w", outputFile, err)
	}

	return nil
}

// Run is the generic entry point for the K-Way Merger application.
// It sorts each input file individually using the provided parser, formatter, and comparator,
// then merges them into a single sorted output file.
//
// Parameters:
//
//	inputFiles - Slice of paths to the input files containing unsorted values
//	outputFile - Path to the output file where merged sorted values will be written
//	parser - Function to parse string values into type T
//	formatter - Function to format values of type T into strings
//	cmp - Comparator function for ordering values of type T
//	concurrency - Number of concurrent goroutines to use for sorting
//
// Returns:
//
//	error - Any error encountered during the process
func Run[T any](inputFiles []string, outputFile string, parser ParseFunc[T], formatter FormatFunc[T], cmp func(T, T) bool) error {
	// Limit concurrency to number of input files if necessary
	// Get the number of CPU cores for concurrency, limit concurrency to number of input files if necessary
	concurrency := runtime.NumCPU()
	if concurrency > len(inputFiles) {
		concurrency = len(inputFiles)
	}

	// Create a semaphore to control concurrency
	sem := make(chan struct{}, concurrency)
	var wg sync.WaitGroup
	var errMu sync.Mutex
	var firstErr error

	// Sort each input file in parallel
	for _, file := range inputFiles {
		sem <- struct{}{} // Acquire semaphore
		wg.Add(1)
		go func(file string) {
			defer wg.Done()
			defer func() { <-sem }() // Release semaphore

			if err := readSortRewrite(file, parser, formatter, cmp); err != nil {
				errMu.Lock()
				if firstErr == nil {
					firstErr = err
				}
				errMu.Unlock()
			}
		}(file)
	}

	// Wait for all sorting goroutines to complete
	wg.Wait()

	// Check if any sorting operation failed
	if firstErr != nil {
		return fmt.Errorf("failed to sort input files: %w", firstErr)
	}

	// Merge the sorted files
	if err := mergeAndWrite(inputFiles, outputFile, parser, formatter, cmp); err != nil {
		return fmt.Errorf("failed to merge files: %w", err)
	}

	return nil
}
