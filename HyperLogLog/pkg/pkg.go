package pkg

import (
	"fmt"
	"hash/fnv"
	"math"
	"sync"
)

type CardinalityEstimator interface {
	// Add string element to the set
	AddString(element string)

	// Add integer element to the set
	AddInt(element int)

	// Return the cardinality of the set
	Cardinality() uint64
}

type HyperLogLog struct {
	p uint     // first p bits of hash used as index
	M []uint32 // register array; len = 2^p; max element in M can be 2^(32-p)
}

func NewHyperLogLog(p uint) *HyperLogLog {
	return &HyperLogLog{p: p, M: make([]uint32, 1<<p)}
}
func getTrailingZeroes(value uint32) uint32 {
	var count uint32
	for value&1 == 0 {
		count++
		value >>= 1
	}
	return count
}
func getIdxAndValue(hashValue uint32, p uint) (uint32, uint32) {
	var index = hashValue >> (32 - p)
	var w = hashValue << p // remove first p bits, then shift to the right by p bits
	w >>= p
	var j uint32 = getTrailingZeroes(w) // get the position of the leftmost 1 bit

	return index, j
}

var hllMutex = sync.RWMutex{}

func (hll *HyperLogLog) AddInt(element int) {
	var hash = fnv.New32a()
	hash.Write([]byte(fmt.Sprintf("%d", element)))
	var hashValue uint32 = hash.Sum32()
	var index, j = getIdxAndValue(hashValue, hll.p)
	hllMutex.Lock()
	hll.M[index] = max(hll.M[index], j)
	hllMutex.Unlock()
}

func (hll *HyperLogLog) AddString(element string) {
	var hash = fnv.New32a()
	hash.Write([]byte(element))
	var hashValue uint32 = hash.Sum32()
	var index, j = getIdxAndValue(hashValue, hll.p)
	hllMutex.Lock()
	hll.M[index] = max(hll.M[index], j)
	hllMutex.Unlock()

}
func linearCounting(m uint32, V uint32) float64 {
	return float64(m) * math.Log(float64(m)/float64(V))
}

func (hll *HyperLogLog) Cardinality() uint64 {
	var m uint32 = 1 << hll.p
	var alpha float64
	switch m {
	case 16:
		alpha = 0.673
	case 32:
		alpha = 0.697
	case 64:
		alpha = 0.709
	default:
		alpha = 0.7213 / (1 + 1.079/float64(m))
	}

	var estimator float64 = 0
	var sum float64 = 0
	for _, v := range hll.M {
		sum += (1.0 / float64(int(1)<<v))
	}
	estimator += alpha * float64(m*m) / sum
	if estimator < 2.5*float64(m) {
		var V uint32 = 0
		for _, v := range hll.M {
			if v == 0 {
				V++
			}
		}
		if V != 0 {
			fmt.Println("linear counting")
			estimator = linearCounting(m, V)
		}
	} else if estimator > (1.0/30.0)*float64(1<<32) {
		estimator = -1 * float64(1<<32) * math.Log(1-(estimator/float64(1<<32)))
	}
	return uint64(estimator)

}
