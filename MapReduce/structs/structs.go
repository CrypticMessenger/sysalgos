package structs

type Input struct {
	Files       []string
	Map         Mapper
	NumMappers  int
	Reduce      Reducer
	NumReducers int
	Partitioner Partitioner
}

type Trie struct {
	Children map[rune]*Trie
	IsEnd    bool
	Value    int
}

type Mapper func(file_name string)                                 // user has to implement this
type Reducer func(key string, partition_number int) // user has to implement this
type Partitioner func(key string, num_partitions uint64) uint64
