package heap

import (
	"bufio"
	"container/heap"
	"fmt"
	"os"
)

// Comparator defines a function type for comparing two values of type T
// It should return true if a should come before b in the heap

type Comparator[T any] func(a, b T) bool

// Node holds a value of type T read from a file,
// along with the file descriptor and a Scanner for further reads.

type Node[T any] struct {
	Val     T
	Fd      *os.File
	Scanner *bufio.Scanner
}

// Heap is a generic heap that holds Node[T] elements according to the provided comparator
// and uses a comparator function to order them.

type Heap[T any] struct {
	nodes      []Node[T]
	comparator Comparator[T]
}

// NewNode creates a new Node with a custom value type by opening the specified file
// and reading the first value using the provided parser function.

func NewNode[T any](filename string, parser func(string) (T, error)) (Node[T], error) {
	fd, err := os.Open(filename)
	if err != nil {
		return Node[T]{}, fmt.Errorf("open file %s: %w", filename, err)
	}

	scanner := bufio.NewScanner(fd)
	scanner.Split(bufio.ScanWords)

	if !scanner.Scan() {
		// close on failure to read
		fd.Close()
		if scanErr := scanner.Err(); scanErr != nil {
			return Node[T]{}, fmt.Errorf("scan file %s: %w", filename, scanErr)
		}
		return Node[T]{}, fmt.Errorf("no value found in file %s", filename)
	}

	val, err := parser(scanner.Text())
	if err != nil {
		fd.Close()
		return Node[T]{}, fmt.Errorf("parse value in file %s: %w", filename, err)
	}

	return Node[T]{Val: val, Fd: fd, Scanner: scanner}, nil
}

// NewHeap creates a new heap with the given initial capacity and comparator.
// Use this function to create heaps with custom value types and comparison logic.

func NewHeap[T any](capacity int, comparator Comparator[T]) *Heap[T] {
	h := &Heap[T]{
		nodes:      make([]Node[T], 0, capacity),
		comparator: comparator,
	}
	heap.Init(h)
	return h
}

// Len returns the number of elements in the heap.

func (h *Heap[T]) Len() int {
	return len(h.nodes)
}

// Less reports whether the element at index i is less than the one at j
// using the heap's comparator function.

func (h *Heap[T]) Less(i, j int) bool {
	return h.comparator(h.nodes[i].Val, h.nodes[j].Val)
}

// Swap swaps the elements at indices i and j.

func (h *Heap[T]) Swap(i, j int) {
	h.nodes[i], h.nodes[j] = h.nodes[j], h.nodes[i]
}

// Push inserts x into the heap. x must be a Node[T].

func (h *Heap[T]) Push(x any) {
	h.nodes = append(h.nodes, x.(Node[T]))
}

// Pop removes and returns the smallest element from the heap.

func (h *Heap[T]) Pop() any {
	old := h.nodes
	n := len(old)
	x := old[n-1]
	h.nodes = old[:n-1]
	return x
}

// Empty reports whether the heap contains no elements.

func (h *Heap[T]) Empty() bool {
	return h.Len() == 0
}

// Helper method to push a Node[T] into the heap

func (h *Heap[T]) PushNode(node Node[T]) {
	heap.Push(h, node)
}

// Helper method to pop the top Node[T] from the heap according to the comparator

func (h *Heap[T]) PopNode() Node[T] {
	return heap.Pop(h).(Node[T])
}
