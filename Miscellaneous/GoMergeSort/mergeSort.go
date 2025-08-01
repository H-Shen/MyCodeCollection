package example

import "sync"

const (
	THRESHOLD = 1024
	ROUTINE   = 1
)

// ParMergesort sorts s in place using a parallel merge‐sort.
func ParMergesort(s []int) {
	parMergesort(s)
}

// SeqMergesort sorts s in place using a sequential merge‐sort.
func SeqMergesort(s []int) {
	seqMergesort(s)
}

func merge(s []int, mid int) {
	temp := make([]int, len(s))
	l, r, k := 0, mid, 0
	for l < mid && r < len(s) {
		if s[l] < s[r] {
			temp[k] = s[l]
			l++
		} else {
			temp[k] = s[r]
			r++
		}
		k++
	}
	for l < mid {
		temp[k] = s[l]
		l++
		k++
	}
	for r < len(s) {
		temp[k] = s[r]
		r++
		k++
	}
	copy(s, temp)
}

func seqMergesort(s []int) {
	if len(s) <= 1 {
		return
	}
	mid := len(s) >> 1
	seqMergesort(s[:mid])
	seqMergesort(s[mid:])
	merge(s, mid)
}

func parMergesort(s []int) {
	if len(s) <= 1 {
		return
	}
	if len(s) <= THRESHOLD {
		seqMergesort(s)
	} else {
		mid := len(s) >> 1
		var wg sync.WaitGroup
		wg.Add(ROUTINE)
		go func() {
			defer wg.Done()
			parMergesort(s[:mid])
		}()
		parMergesort(s[mid:])
		wg.Wait()
		merge(s, mid)
	}
}
