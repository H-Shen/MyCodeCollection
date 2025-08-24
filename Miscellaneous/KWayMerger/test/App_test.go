// Package test contains unit tests for the K-Way Merger application.
// It includes tests for the core functionality of reading, sorting,
// and merging files, as well as helper functions for generating test data
// and verifying results.
package test

import (
	"KWayMerger/app"
	"bufio"
	"errors"
	"fmt"
	"math"
	"math/rand"
	"os"
	"sort"
	"strconv"
	"sync"
	"testing"
	"time"
)

const (
	INT32_NUMBER  = 1000000 // Number of random int32 to generate per test file
	INT64_NUMBER  = 1000000 // Number of random int64 to generate per test file
	STRING_NUMBER = 100000  // Number of random strings to generate per test file
	STRING_LENGTH = 20      // Length of each random string
)

// generateRandomNumberInt32 generates n random int32 values and writes them to the specified file.
// The integers span the full int32 range.
func generateRandomNumberInt32(filename string, n int) error {
	// create a new, locally seeded RNG
	rng := rand.New(rand.NewSource(time.Now().UnixNano()))

	// open (or create) the file
	fd, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("failed to open file %s: %w", filename, err)
	}
	defer fd.Close()

	// buffer writes for performance
	buf := bufio.NewWriter(fd)

	for i := 0; i < n; i++ {
		// rng.Int31() ∈ [0, 2³¹)
		// + MinInt32 shifts it to [−2³¹, 2³¹−1]
		num := rng.Int31() + int32(math.MinInt32)

		if _, err := fmt.Fprintln(buf, num); err != nil {
			return fmt.Errorf("failed to write to file %s: %w", filename, err)
		}
	}

	// flush buffered data
	if err := buf.Flush(); err != nil {
		return fmt.Errorf("failed to flush buffer to file %s: %w", filename, err)
	}

	// ensure on-disk sync
	if err := fd.Sync(); err != nil {
		return fmt.Errorf("failed to sync file %s: %w", filename, err)
	}

	return nil
}

// generateRandomNumberInt64 generates n random int64 values and writes them to the specified file.
// The integers span the full int64 range.
func generateRandomNumberInt64(filename string, n int) error {
	// create a new, locally seeded RNG
	rng := rand.New(rand.NewSource(time.Now().UnixNano()))

	// open (or create) the file
	fd, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("failed to open file %s: %w", filename, err)
	}
	defer fd.Close()

	// buffer writes for performance
	buf := bufio.NewWriter(fd)

	for i := 0; i < n; i++ {
		// rng.Int63() ∈ [0, 2⁶³)
		// + MinInt64 shifts it to [−2⁶³, 2⁶³−1]
		num := rng.Int63() + math.MinInt64

		if _, err := fmt.Fprintln(buf, num); err != nil {
			return fmt.Errorf("failed to write to file %s: %w", filename, err)
		}
	}

	// flush buffered data
	if err := buf.Flush(); err != nil {
		return fmt.Errorf("failed to flush buffer to file %s: %w", filename, err)
	}

	// ensure on-disk sync
	if err := fd.Sync(); err != nil {
		return fmt.Errorf("failed to sync file %s: %w", filename, err)
	}

	return nil
}

// generateRandomString generates n random strings and writes them to the specified file.
func generateRandomString(filename string, n int) error {
	// create a new, locally seeded RNG
	rng := rand.New(rand.NewSource(time.Now().UnixNano()))

	// Define character set for random strings
	const charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

	// open (or create) the file
	fd, err := os.Create(filename)
	if err != nil {
		return fmt.Errorf("failed to open file %s: %w", filename, err)
	}
	defer fd.Close()

	// buffer writes for performance
	buf := bufio.NewWriter(fd)

	for i := 0; i < n; i++ {
		// Generate a random string of fixed length
		b := make([]byte, STRING_LENGTH)
		for j := range b {
			b[j] = charset[rng.Intn(len(charset))]
		}
		randomString := string(b)

		if _, err := fmt.Fprintln(buf, randomString); err != nil {
			return fmt.Errorf("failed to write to file %s: %w", filename, err)
		}
	}

	// flush buffered data
	if err := buf.Flush(); err != nil {
		return fmt.Errorf("failed to flush buffer to file %s: %w", filename, err)
	}

	// ensure on-disk sync
	if err := fd.Sync(); err != nil {
		return fmt.Errorf("failed to sync file %s: %w", filename, err)
	}

	return nil
}

// verifyInt32 checks if the int32 values in the specified file are sorted in ascending order.
func verifyInt32(filename string) (bool, error) {
	// Open file for reading
	fd, err := os.Open(filename)
	if err != nil {
		return false, fmt.Errorf("failed to open file %s: %w", filename, err)
	}
	// Ensure file is closed when function exits
	defer func() {
		closeErr := fd.Close()
		if closeErr != nil && err == nil {
			err = fmt.Errorf("failed to close file %s: %w", filename, closeErr)
		}
	}()

	// Read integers from file
	scanner := bufio.NewScanner(fd)
	scanner.Split(bufio.ScanWords) // Split on whitespace
	var temp int32
	var list []int32
	for scanner.Scan() {
		val, parseErr := strconv.ParseInt(scanner.Text(), 10, 32)
		if parseErr != nil {
			return false, fmt.Errorf("failed to parse number in file %s: %w", filename, parseErr)
		}
		temp = int32(val)
		list = append(list, temp)
	}
	if err = scanner.Err(); err != nil {
		return false, fmt.Errorf("error reading file %s: %w", filename, err)
	}

	// Check if the list is sorted
	return sort.SliceIsSorted(list, func(i, j int) bool { return list[i] < list[j] }), nil
}

// verifyInt64 checks if the int64 values in the specified file are sorted in ascending order.
func verifyInt64(filename string) (bool, error) {
	// Open file for reading
	fd, err := os.Open(filename)
	if err != nil {
		return false, fmt.Errorf("failed to open file %s: %w", filename, err)
	}
	// Ensure file is closed when function exits
	defer func() {
		closeErr := fd.Close()
		if closeErr != nil && err == nil {
			err = fmt.Errorf("failed to close file %s: %w", filename, closeErr)
		}
	}()

	// Read integers from file
	scanner := bufio.NewScanner(fd)
	scanner.Split(bufio.ScanWords) // Split on whitespace
	var temp int64
	var list []int64
	for scanner.Scan() {
		val, parseErr := strconv.ParseInt(scanner.Text(), 10, 64)
		if parseErr != nil {
			return false, fmt.Errorf("failed to parse number in file %s: %w", filename, parseErr)
		}
		temp = val
		list = append(list, temp)
	}
	if err = scanner.Err(); err != nil {
		return false, fmt.Errorf("error reading file %s: %w", filename, err)
	}

	// Check if the list is sorted
	return sort.SliceIsSorted(list, func(i, j int) bool { return list[i] < list[j] }), nil
}

// verifyString checks if the strings in the specified file are sorted in ascending order.
func verifyString(filename string) (bool, error) {
	// Open file for reading
	fd, err := os.Open(filename)
	if err != nil {
		return false, fmt.Errorf("failed to open file %s: %w", filename, err)
	}
	// Ensure file is closed when function exits
	defer func() {
		closeErr := fd.Close()
		if closeErr != nil && err == nil {
			err = fmt.Errorf("failed to close file %s: %w", filename, closeErr)
		}
	}()

	// Read strings from file
	scanner := bufio.NewScanner(fd)
	scanner.Split(bufio.ScanWords) // Split on whitespace
	var list []string
	for scanner.Scan() {
		list = append(list, scanner.Text())
	}
	if err = scanner.Err(); err != nil {
		return false, fmt.Errorf("error reading file %s: %w", filename, err)
	}

	// Check if the list is sorted
	return sort.SliceIsSorted(list, func(i, j int) bool { return list[i] < list[j] }), nil
}

// createDir creates a directory with the specified name if it doesn't exist.
// If the directory already exists, it verifies that it is indeed a directory.
func createDir(dirName string) error {
	err := os.Mkdir(dirName, 0755)
	if err != nil && os.IsExist(err) {
		fileInfo, statErr := os.Stat(dirName)
		if statErr != nil {
			return statErr
		}
		if !fileInfo.IsDir() {
			return errors.New("path exists but is not a directory")
		}
		return nil
	}
	return err
}

// TestAppInt32 tests the K-Way Merger application with multiple test cases using int32 values.
func TestAppInt32(t *testing.T) {
	path, err := os.Getwd()
	if err != nil {
		t.Log(err)
	}
	// Define test cases
	tests := []struct {
		name string
		want bool
	}{{
			name: "Test_with_random_Int32_dataset_1", want: true},
		{
			name: "Test_with_random_Int32_dataset_2", want: true},
		{
			name: "Test_with_random_Int32_dataset_3", want: true},
		{
			name: "Test_with_random_Int32_dataset_4", want: true},
		{
			name: "Test_with_random_Int32_dataset_5", want: true},
		{
			name: "Test_with_random_Int32_dataset_6", want: true},
		{
			name: "Test_with_random_Int32_dataset_7", want: true},
		{
			name: "Test_with_random_Int32_dataset_8", want: true},
	}
	// Run each test case
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			// Create data and output directories
			dataDir := path + "/data"
			if err = createDir(dataDir); err != nil {
				t.Fatalf("Failed to create data directory: %v\n", err)
			}
			outputDir := path + "/output"
			if err = createDir(outputDir); err != nil {
				t.Fatalf("Failed to create output directory: %v\n", err)
			}
			// Generate input file paths
			inputFilesCount := 4
			var wg sync.WaitGroup
			var inputFiles []string
			for i := 1; i <= inputFilesCount; i++ {
				filename := dataDir + "/" + tt.name + "_" + strconv.Itoa(i) + ".txt"
				inputFiles = append(inputFiles, filename)
			}
			// Generate random numbers in input files (parallel)
			wg.Add(inputFilesCount)
			errCh := make(chan error, inputFilesCount)
			for i := range inputFilesCount {
				j := i
				go func() {
					defer wg.Done()
					// Generate random numbers and write to file
					if err := generateRandomNumberInt32(inputFiles[j], INT32_NUMBER); err != nil {
						errCh <- err
					}
				}()
			}

			// Close error channel once all goroutines are done
			go func() {
				wg.Wait()
				close(errCh)
			}()

			// Collect all errors from random number generation
			var errs []error
			for err := range errCh {
				errs = append(errs, err)
			}

			// If there are errors, fail the test
			if len(errs) > 0 {
				t.Fatalf("%d errors occurred during random number generation: %v", len(errs), errs)
			}
			// Define output file path
			outputFile := outputDir + "/" + tt.name + "out.txt"
			// Define parser, formatter, and comparator for int32
			parseInt32 := func(s string) (int32, error) {
				val, err := strconv.ParseInt(s, 10, 32)
				return int32(val), err
			}
			formatInt32 := func(i int32) string {
				return strconv.FormatInt(int64(i), 10)
			}
			compareInt32 := func(a, b int32) bool {
				return a < b
			}

			// Run the K-Way Merger application using generic implementation
			if err := app.Run(inputFiles, outputFile, parseInt32, formatInt32, compareInt32); err != nil {
				t.Fatalf("Failed to run K-Way Merger: %v", err)
			}

			// Verify the output file is sorted
			got, err := verifyInt32(outputFile)
			if err != nil {
				t.Fatalf("Failed to verify output file: %v", err)
			}
			if got != tt.want {
				t.Errorf("Are numbers in the output file sorted? %v, want %v", got, tt.want)
			}
			// Clean up test data after the test
			t.Cleanup(func() {
				err = os.RemoveAll(dataDir)
				if err != nil {
					t.Errorf("Failed to remove data directory: %v", err)
				}
				err = os.RemoveAll(outputDir)
				if err != nil {
					t.Errorf("Failed to remove output directory: %v", err)
				}
			})
		})}
}

// TestAppInt64 tests the K-Way Merger application with multiple test cases using int64 values.
func TestAppInt64(t *testing.T) {
	path, err := os.Getwd()
	if err != nil {
		t.Log(err)
	}
	// Define test cases
	tests := []struct {
		name string
		want bool
	}{{
			name: "Test_with_random_Int64_dataset_1", want: true},
		{
			name: "Test_with_random_Int64_dataset_2", want: true},
		{
			name: "Test_with_random_Int64_dataset_3", want: true},
		{
			name: "Test_with_random_Int64_dataset_4", want: true},
		{
			name: "Test_with_random_Int64_dataset_5", want: true},
		{
			name: "Test_with_random_Int64_dataset_6", want: true},
		{
			name: "Test_with_random_Int64_dataset_7", want: true},
		{
			name: "Test_with_random_Int64_dataset_8", want: true},
	}
	// Run each test case
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			// Create data and output directories
			dataDir := path + "/data"
			if err = createDir(dataDir); err != nil {
				t.Fatalf("Failed to create data directory: %v\n", err)
			}
			outputDir := path + "/output"
			if err = createDir(outputDir); err != nil {
				t.Fatalf("Failed to create output directory: %v\n", err)
			}
			// Generate input file paths
			inputFilesCount := 4
			var wg sync.WaitGroup
			var inputFiles []string
			for i := 1; i <= inputFilesCount; i++ {
				filename := dataDir + "/" + tt.name + "_" + strconv.Itoa(i) + ".txt"
				inputFiles = append(inputFiles, filename)
			}
			// Generate random numbers in input files (parallel)
			wg.Add(inputFilesCount)
			errCh := make(chan error, inputFilesCount)
			for i := range inputFilesCount {
				j := i
				go func() {
					defer wg.Done()
					// Generate random numbers and write to file
					if err := generateRandomNumberInt64(inputFiles[j], INT64_NUMBER); err != nil {
						errCh <- err
					}
				}()
			}

			// Close error channel once all goroutines are done
			go func() {
				wg.Wait()
				close(errCh)
			}()

			// Collect all errors from random number generation
			var errs []error
			for err := range errCh {
				errs = append(errs, err)
			}

			// If there are errors, fail the test
			if len(errs) > 0 {
				t.Fatalf("%d errors occurred during random number generation: %v", len(errs), errs)
			}
			// Define output file path
			outputFile := outputDir + "/" + tt.name + "out.txt"
			// Define parser, formatter, and comparator for int64
			parseInt64 := func(s string) (int64, error) {
				val, err := strconv.ParseInt(s, 10, 64)
				return val, err
			}
			formatInt64 := func(i int64) string {
				return strconv.FormatInt(i, 10)
			}
			compareInt64 := func(a, b int64) bool {
				return a < b
			}

			// Run the K-Way Merger application using generic implementation
			if err := app.Run(inputFiles, outputFile, parseInt64, formatInt64, compareInt64); err != nil {
				t.Fatalf("Failed to run K-Way Merger: %v", err)
			}

			// Verify the output file is sorted
			got, err := verifyInt64(outputFile)
			if err != nil {
				t.Fatalf("Failed to verify output file: %v", err)
			}
			if got != tt.want {
				t.Errorf("Are numbers in the output file sorted? %v, want %v", got, tt.want)
			}
			// Clean up test data after the test
			t.Cleanup(func() {
				err = os.RemoveAll(dataDir)
				if err != nil {
					t.Errorf("Failed to remove data directory: %v", err)
				}
				err = os.RemoveAll(outputDir)
				if err != nil {
					t.Errorf("Failed to remove output directory: %v", err)
				}
			})
		})}
}

// TestAppString tests the K-Way Merger application with multiple test cases using string values.
func TestAppString(t *testing.T) {
	path, err := os.Getwd()
	if err != nil {
		t.Log(err)
	}
	// Define test cases
	tests := []struct {
		name string
		want bool
	}{{
			name: "Test_with_random_string_dataset_1", want: true},
		{
			name: "Test_with_random_string_dataset_2", want: true},
		{
			name: "Test_with_random_string_dataset_3", want: true},
		{
			name: "Test_with_random_string_dataset_4", want: true},
		{
			name: "Test_with_random_string_dataset_5", want: true},
		{
			name: "Test_with_random_string_dataset_6", want: true},
		{
			name: "Test_with_random_string_dataset_7", want: true},
		{
			name: "Test_with_random_string_dataset_8", want: true},
	}
	// Run each test case
	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			// Create data and output directories
			dataDir := path + "/data"
			if err = createDir(dataDir); err != nil {
				t.Fatalf("Failed to create data directory: %v\n", err)
			}
			outputDir := path + "/output"
			if err = createDir(outputDir); err != nil {
				t.Fatalf("Failed to create output directory: %v\n", err)
			}
			// Generate input file paths
			inputFilesCount := 4
			var wg sync.WaitGroup
			var inputFiles []string
			for i := 1; i <= inputFilesCount; i++ {
				filename := dataDir + "/" + tt.name + "_" + strconv.Itoa(i) + ".txt"
				inputFiles = append(inputFiles, filename)
			}
			// Generate random strings in input files (parallel)
			wg.Add(inputFilesCount)
			errCh := make(chan error, inputFilesCount)
			for i := range inputFilesCount {
				j := i
				go func() {
					defer wg.Done()
					// Generate random strings and write to file
					if err := generateRandomString(inputFiles[j], STRING_NUMBER); err != nil {
						errCh <- err
					}
				}()
			}

			// Close error channel once all goroutines are done
			go func() {
				wg.Wait()
				close(errCh)
			}()

			// Collect all errors from random string generation
			var errs []error
			for err := range errCh {
				errs = append(errs, err)
			}

			// If there are errors, fail the test
			if len(errs) > 0 {
				t.Fatalf("%d errors occurred during random string generation: %v", len(errs), errs)
			}
			// Define output file path
			outputFile := outputDir + "/" + tt.name + "out.txt"
			// Define parser, formatter, and comparator for string
			parseString := func(s string) (string, error) {
				return s, nil
			}
			formatString := func(s string) string {
				return s
			}
			compareString := func(a, b string) bool {
				return a < b
			}

			// Run the K-Way Merger application using generic implementation
			if err := app.Run(inputFiles, outputFile, parseString, formatString, compareString); err != nil {
				t.Fatalf("Failed to run K-Way Merger: %v", err)
			}

			// Verify the output file is sorted
			got, err := verifyString(outputFile)
			if err != nil {
				t.Fatalf("Failed to verify output file: %v", err)
			}
			if got != tt.want {
				t.Errorf("Are strings in the output file sorted? %v, want %v", got, tt.want)
			}
			// Clean up test data after the test
			t.Cleanup(func() {
				err = os.RemoveAll(dataDir)
				if err != nil {
					t.Errorf("Failed to remove data directory: %v", err)
				}
				err = os.RemoveAll(outputDir)
				if err != nil {
					t.Errorf("Failed to remove output directory: %v", err)
				}
			})
		})}
}