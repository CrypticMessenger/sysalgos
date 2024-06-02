package main

import (
	"bufio"
	"fmt"
	"os"

	"mapreduce/structs"
	"mapreduce/utils"
)

var input structs.Input

// Mapper function
func Map(filename string) {
	file, err := os.Open(filename)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	scanner.Split(bufio.ScanWords) // Set the split function to bufio.ScanWords to tokenize by words

	for scanner.Scan() {
		word := scanner.Text()
		var partition = input.Partitioner(word, uint64(input.NumReducers))
		utils.MR_Emit(partition, word, 1)
	}

	if err := scanner.Err(); err != nil {
		fmt.Fprintln(os.Stderr, "reading input:", err)
	}

}

func Reduce(key string, partition_number int) {
	fmt.Printf("key: %v, count: %v\n", key, utils.Search(&utils.Trie, key))
}

func main() {
	// Example usage
	filenames := []string{"test1.txt", "test2.txt"}
	var numMappers int = 5
	var numReducers int = 5

	input = utils.Setup(filenames, Map, numMappers, Reduce, numReducers, utils.MR_DefaultHashPartition)
	utils.MRRun(input)
}
