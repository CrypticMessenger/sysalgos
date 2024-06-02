package utils

import (
	"mapreduce/structs"
	"sync"
)

var KVMap map[int][]string
var Trie structs.Trie

func Setup(files []string, mapper structs.Mapper, numMappers int, reducer structs.Reducer, numReducers int, partitioner structs.Partitioner) structs.Input {
	KVMap = make(map[int][]string, numReducers)
	Trie = structs.Trie{}
	return structs.Input{
		Files:       files,
		Map:         mapper,
		NumMappers:  numMappers,
		Reduce:      reducer,
		NumReducers: numReducers,
		Partitioner: partitioner,
	}
}

func MR_DefaultHashPartition(key string, num_partitions uint64) uint64 {
	var hash uint64 = 5381
	for _, c := range key {
		hash = ((hash << 5) + hash) + uint64(c)
	}
	return hash % num_partitions
}

var kvlock = sync.RWMutex{}
var trieMutex = sync.RWMutex{}

func Insert(t *structs.Trie, key string, value int) {
	trieMutex.Lock()
	node := t
	for _, c := range key {
		if node.Children == nil {
			// trieMutex.Lock()
			node.Children = make(map[rune]*structs.Trie)
			// trieMutex.Unlock()
		}
		_, ok := node.Children[c]
		if !ok {
			// trieMutex.Lock()
			node.Children[c] = &structs.Trie{}
			// trieMutex.Unlock()
		}

		node = node.Children[c]
	}
	// trieMutex.Lock()
	node.IsEnd = true
	node.Value += value
	trieMutex.Unlock()
}
func Search(t *structs.Trie, key string) int {
	node := t
	for _, c := range key {
		if node.Children == nil {
			return 0
		}
		if _, ok := node.Children[c]; !ok {
			return 0
		}
		node = node.Children[c]
	}
	if node.IsEnd {
		return node.Value
	}
	return 0
}

func MR_Emit(partition uint64, key string, value int) {
	kvlock.Lock()
	KVMap[int(partition)] = append(KVMap[int(partition)], key)
	kvlock.Unlock()
	Insert(&Trie, key, value)
}

// MRRun orchestrates the MapReduce process
func MRRun(input structs.Input) {
	var semaphore_map chan int = make(chan int, input.NumMappers)
	var wg sync.WaitGroup

	// Start mappers
	for _, filename := range input.Files {
		wg.Add(1)
		semaphore_map <- 1
		go func(filename string) {
			defer wg.Done()
			defer func() { <-semaphore_map }()
			input.Map(filename)
		}(filename)

	}
	wg.Wait()

	// Start reducers
	var semaphore_reduce chan int = make(chan int, input.NumReducers)
	for i := 0; i < input.NumReducers; i++ {
		wg.Add(1)
		semaphore_reduce <- 1
		go func(i int) {
			defer wg.Done()
			defer func() { <-semaphore_reduce }()
			for _, key := range KVMap[i] {
				input.Reduce(key, i)
			}

		}(i)
	}
	wg.Wait()

}
