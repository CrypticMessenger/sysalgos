package main

const (
	// Number of queues
	NUMQUEUES int = 3

	//I/O time - same for all jobs
	IOTIME int = 5

	// Quantum for each queue (same for all queues): amt of time given to the job in Round Robin fashion
	QUANTUM int = 10

	// allotment for each queue (same for all queues): the amount of time a job can spend at a given priority level before the scheduler reduces its priority.
	ALLOTMENT int = 1

	// boost time: the amount of time after which all jobs are moved to the highest priority queue
	BOOSTTIME int = 10

	// HIGHEST PRIORITY QUEUE
	HIGHEST_PRIORITY int = NUMQUEUES - 1
)

type Job struct {
	// id
	id int

	// queue number
	priority int

	// ticks left to run (used for Round Robin scheduling)
	ticksLeft int

	// alloted time left
	allotedTimeLeft int

	// start time of the job
	startTime int

	// Run time of the job
	runTime int

	//time left to run
	timeLeft int

	// IO frequency: every ioFreq time units, the job issues an I/O
	ioFreq int

	// doing I/O
	doingIo bool

	// first run time (useful for response time calculation)
	firstRunTime int

	// finish time
	finishTime int
}

// create struct for queue
type Queue struct {
	// jobs in the queue
	job_ids []int
}

type IOQueuePair struct {
	// job id
	job_id int
	// time to end I/O
	time_to_end_io int
}
