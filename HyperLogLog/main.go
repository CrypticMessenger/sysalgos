package main

import (
	"hyperloglog/pkg"
	"math/rand"
	"sync"
)

func generateRandomString(length int, rng *rand.Rand) string {
	charset := "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
	b := make([]byte, length)
	for i := range b {
		b[i] = charset[rng.Intn(len(charset))]
	}
	return string(b)
}

var wg sync.WaitGroup

func main() {
	seed := int64(37) // Change this seed value as needed
	source := rand.NewSource(seed)

	// Create a new random number generator using the source
	rng := rand.New(source)

	var estimator pkg.CardinalityEstimator = pkg.NewHyperLogLog(4)
	for i := 0; i < 1000000; i++ {
		wg.Add(1)
		func() {
			defer wg.Done()
			// Generate a random string
			randomString := generateRandomString(10, rng)

			// Add the random string to the HyperLogLog
			estimator.AddString(randomString)
		}()
	}
	wg.Wait()
	println(estimator.Cardinality())

}
