package main

import "fmt"

// add to queue function
func (q *Queue) addJobToEnd(job_id int) {
	q.job_ids = append(q.job_ids, job_id)
}
func (q *Queue) addJobToStart(job_id int) {
	q.job_ids = append([]int{job_id}, q.job_ids...)
}

func (q *Queue) removeJobAt(i int) {
	if i < 0 || i >= len(q.job_ids) {
		return
	}
	q.job_ids = append(q.job_ids[:i], q.job_ids[i+1:]...)
}

func findJobToRun(queues [NUMQUEUES]Queue) (int, int) {
	for i := len(queues) - 1; i >= 0; i-- {
		if len(queues[i].job_ids) > 0 {
			return i, queues[i].job_ids[0]
		}
	}
	return -1, -1
}

func main() {
	var i_o_queue []IOQueuePair = []IOQueuePair{} // stores (job_id,time_to_end_io) in form queue
	var quantums [NUMQUEUES]int
	var allotments [NUMQUEUES]int
	for i := range quantums {
		quantums[i] = QUANTUM
		allotments[i] = ALLOTMENT
	}

	fmt.Println("Enter number of jobs: ")
	var numJobs int
	fmt.Scanln(&numJobs)
	var jobs []Job
	var startTime, timeToComplete, ioFreq int
	for i := 0; i < numJobs; i++ {
		fmt.Println("Enter start time, time to complete and I/O frequency for job ", i+1)
		fmt.Scanln(&startTime)
		fmt.Scanln(&timeToComplete)
		fmt.Scanln(&ioFreq)

		jobs = append(jobs, Job{id: i, priority: HIGHEST_PRIORITY, ticksLeft: quantums[HIGHEST_PRIORITY], allotedTimeLeft: allotments[HIGHEST_PRIORITY], startTime: startTime, runTime: timeToComplete, timeLeft: timeToComplete, ioFreq: ioFreq, doingIo: false, firstRunTime: -1, finishTime: -1})
	}
	// create queues, each initialized with empty jobs
	var queues [NUMQUEUES]Queue // queues[HIGHEST_PRIORITY] is the highest priority queue
	for i := range queues {
		queues[i] = Queue{job_ids: []int{}}
	}

	// initialize currentTime (globalized) = 0
	var currentTime int = 0

	// initiialize finished_jobs = 0
	var finishedJobs int = 0

	for finishedJobs < numJobs {
		// 	// check if any job has arrived at CurrentTime
		for i := range jobs {
			// 	// 	//if yes, add it to the highest priority queue
			if jobs[i].startTime == currentTime {
				queues[HIGHEST_PRIORITY].addJobToEnd(i)
			}
		}

		var newStart int = -1
		// 	// check if any job has completed its I/O at CurrentTime
		for i, io_out_entry := range i_o_queue {
			if io_out_entry.time_to_end_io == currentTime {
				fmt.Printf("[ time %d ] IO_END for JOB %d\n", currentTime, io_out_entry.job_id)
				queues[jobs[io_out_entry.job_id].priority].addJobToEnd(io_out_entry.job_id)
				jobs[io_out_entry.job_id].doingIo = false

			} else {
				newStart = i
				break
			}
		}
		if newStart != -1 {
			i_o_queue = i_o_queue[newStart:]
		} else {
			i_o_queue = []IOQueuePair{}
		}

		// 	// check if it's time to boost
		if BOOSTTIME != 0 && currentTime != 0 {
			if currentTime%BOOSTTIME == 0 {
				fmt.Printf("[ time %d ] BOOST (every %d)\n", currentTime, BOOSTTIME)
				// 	// 	//if yes, move all jobs to highest priority queue and reset allotedTimeLeft, ticksLeft
				for i := range queues {
					for j := range queues[i].job_ids {
						jobs[queues[i].job_ids[j]].priority = HIGHEST_PRIORITY
						jobs[queues[i].job_ids[j]].allotedTimeLeft = allotments[HIGHEST_PRIORITY]
						jobs[queues[i].job_ids[j]].ticksLeft = quantums[HIGHEST_PRIORITY]
					}
				}
				for i := NUMQUEUES - 2; i >= 0; i-- {
					queues[HIGHEST_PRIORITY].job_ids = append(queues[HIGHEST_PRIORITY].job_ids, queues[i].job_ids...)
					queues[i].job_ids = []int{}
				}
			}
		}

		var currQueue, jobToRun int = findJobToRun(queues)
		if jobToRun == -1 {
			fmt.Printf("[ time %d ] IDLE\n", (currentTime))
			currentTime++
			continue
		}
		jobs[jobToRun].timeLeft--
		jobs[jobToRun].ticksLeft--
		fmt.Printf("[ time %d ] Run JOB %d at PRIORITY %d [ TICKS %d ALLOT %d TIME %d (of %d) ]\n", currentTime, jobToRun, jobs[jobToRun].priority, jobs[jobToRun].ticksLeft, jobs[jobToRun].allotedTimeLeft, jobs[jobToRun].timeLeft, jobs[jobToRun].runTime)

		if jobs[jobToRun].firstRunTime == -1 {
			jobs[jobToRun].firstRunTime = currentTime
		}

		currentTime++

		if jobs[jobToRun].timeLeft == 0 {
			finishedJobs++
			queues[currQueue].removeJobAt(0)
			fmt.Printf("[ time %d ] FINISHED JOB %d\n", currentTime, jobToRun)
			jobs[jobToRun].finishTime = currentTime

			continue
		}

		var issuedIo bool = false
		if jobs[jobToRun].ioFreq != 0 && (jobs[jobToRun].runTime-jobs[jobToRun].timeLeft)%jobs[jobToRun].ioFreq == 0 {
			fmt.Printf("[ time %d ] IO_START for JOB %d\n", currentTime, jobToRun)
			issuedIo = true
			jobs[jobToRun].doingIo = true
			i_o_queue = append(i_o_queue, IOQueuePair{job_id: jobToRun, time_to_end_io: currentTime + IOTIME})
			queues[currQueue].removeJobAt(0)
		}

		if jobs[jobToRun].ticksLeft == 0 {
			if !issuedIo {
				queues[currQueue].removeJobAt(0)
			}
			jobs[jobToRun].allotedTimeLeft--
			if jobs[jobToRun].allotedTimeLeft == 0 {
				if jobs[jobToRun].priority != 0 {
					jobs[jobToRun].priority--
					jobs[jobToRun].allotedTimeLeft = allotments[currQueue-1]
					jobs[jobToRun].ticksLeft = quantums[currQueue-1]
					if !issuedIo {
						queues[currQueue-1].addJobToEnd(jobToRun)
					}
				} else {
					jobs[jobToRun].ticksLeft = quantums[currQueue]
					jobs[jobToRun].allotedTimeLeft = allotments[currQueue]
					if !issuedIo {
						queues[currQueue].addJobToEnd(jobToRun)
					}
				}
			} else {
				jobs[jobToRun].ticksLeft = quantums[currQueue]
				if !issuedIo {
					queues[currQueue].addJobToEnd(jobToRun)
				}
			}
		}

	}
	for i, job := range jobs {
		fmt.Printf("Job %d: response time: %d turnaround time: %d\n", i, job.firstRunTime, job.finishTime-job.startTime)
	}

}
