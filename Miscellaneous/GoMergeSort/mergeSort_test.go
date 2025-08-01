package example_test

import (
	"example"
	"math/rand"
	"testing"
	"time"
)

const N = 1e6

var src []int

func init() {
	rand.Seed(time.Now().UnixNano())
	src = make([]int, N)
	for i := range src {
		src[i] = rand.Int()
	}
}

// clone returns a fresh copy so each benchmark iteration sorts identical data.
func clone(in []int) []int {
	out := make([]int, len(in))
	copy(out, in)
	return out
}

func BenchmarkMergeSorts(b *testing.B) {
	b.ReportAllocs()

	b.Run("Parallel", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			data := clone(src)
			example.ParMergesort(data)
		}
	})

	b.Run("Sequential", func(b *testing.B) {
		for i := 0; i < b.N; i++ {
			data := clone(src)
			example.SeqMergesort(data)
		}
	})
}
