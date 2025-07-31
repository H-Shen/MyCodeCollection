package main

import (
    "sync"
)

const threshold = 1024 // below this size, switch to sequential

// ParallelMergeSort sorts s in place, using up to two goroutines
// per split and a single auxiliary buffer.
func ParallelMergeSort(s []int) {
    buf := make([]int, len(s))
    parallelSort(s, buf)
}

// SequentialMergeSort sorts s in place using only the current goroutine.
func SequentialMergeSort(s []int) {
    buf := make([]int, len(s))
    seqSort(s, buf)
}

func parallelSort(s, buf []int) {
    n := len(s)
    if n <= 1 {
        return
    }
    if n <= threshold {
        seqSort(s, buf)
        return
    }

    mid := n / 2
    var wg sync.WaitGroup
    wg.Add(1)
    go func() {
        defer wg.Done()
        parallelSort(s[:mid], buf[:mid])
    }()
    parallelSort(s[mid:], buf[mid:])
    wg.Wait()

    merge(s, buf, mid)
}

func seqSort(s, buf []int) {
    n := len(s)
    if n <= 1 {
        return
    }

    mid := n / 2
    seqSort(s[:mid], buf[:mid])
    seqSort(s[mid:], buf[mid:])
    merge(s, buf, mid)
}

// merge assumes s[:mid] and s[mid:] are each sorted.
// It writes the merged result into buf, then copies it back to s.
func merge(s, buf []int, mid int) {
    i, j, k := 0, mid, 0
    n := len(s)

    // merge into buf
    for i < mid && j < n {
        if s[i] <= s[j] {
            buf[k] = s[i]
            i++
        } else {
            buf[k] = s[j]
            j++
        }
        k++
    }
    // copy any leftovers
    copy(buf[k:], s[i:mid])
    copy(buf[k+(mid-i):], s[j:])

    // write back
    copy(s, buf[:n])
}
