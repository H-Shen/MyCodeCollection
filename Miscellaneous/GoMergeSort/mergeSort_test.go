package main

import (
    "math/rand"
    "testing"
    "time"
)

const N = 1e6

var src []int

func init() {
    // Seed once, build one big random slice up front
    rand.Seed(time.Now().UnixNano())
    src = make([]int, N)
    for i := range src {
        src[i] = rand.Int()
    }
}

// clone makes a fresh copy so each iteration sorts the same data
func clone(in []int) []int {
    out := make([]int, len(in))
    copy(out, in)
    return out
}

func BenchmarkMergeSorts(b *testing.B) {
    b.ReportAllocs() // show you how many allocations each sort does

    b.Run("Parallel", func(b *testing.B) {
        for i := 0; i < b.N; i++ {
            data := clone(src)
            parMergesort(data)
        }
    })

    b.Run("Sequential", func(b *testing.B) {
        for i := 0; i < b.N; i++ {
            data := clone(src)
            seqMergesort(data)
        }
    })
}
