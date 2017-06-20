#!/usr/bin/python3.5

## Modify the SJF and FCFS algorithm in the topic of dealing with jobs.

job_num = 5


class Job():
    # 定义作业
    def __init__(self, arr_time=-1, sev_time=-1, cpt_time=-1, wghted_run_time=-1):
        self.arr_time = arr_time
        self.sev_time = sev_time
        self.cpt_time = cpt_time
        self.wghted_run_time = wghted_run_time
        self.run_time = self.cpt_time - self.arr_time

# 初始化工作
job0 = Job(arr_time=0, sev_time=4)
job1 = Job(arr_time=1, sev_time=1)
job2 = Job(arr_time=4, sev_time=2)
job3 = Job(arr_time=5, sev_time=1)
job4 = Job(arr_time=6, sev_time=3)

job_list = []

for i in range(job_num):
    job_list.append(locals()['job' + str(i)])


def FCFS(job_list):
    # 先来先服务算法, 比较简单, 不赘述.
    avg_run_time = 0
    avg_wght_run_time = 0
    print('NO.\t\tarr_time\tsev_time\tweighted_run_time')
    curr_cpt_time = 0
    for i in job_list:
        curr_cpt_time += i.sev_time
        i.cpt_time = curr_cpt_time
        i.wghted_run_time = (i.cpt_time - i.arr_time) / i.sev_time
        i.run_time = i.cpt_time - i.arr_time
        print("job{}\t{}\t\t\t{}\t\t\t{}".format(str(job_list.index(i)), i.arr_time, i.cpt_time, i.wghted_run_time))
    avg_run_time = sum([i.run_time for i in job_list]) / len(job_list)
    avg_wght_run_time = sum([i.run_time for i in job_list]) / sum([i.sev_time for i in job_list])
    print('avg_run_time: ', avg_run_time)
    print('avg_wght_run_time: ', avg_wght_run_time)
    return 0


def SJF(job_list):
    # 短作业优先算法(非抢占式).
    job_list_back_up = job_list.copy()
    print('NO.\t\tarr_time\tsev_time\tweighted_run_time')
    curr_cpt_time = 0
    curr_time = 0
    # 预留初始作业表, 用于之后的确定作业序号.
    arr_time_list_initial = [i.arr_time for i in job_list.copy()]
    for n in range(len(job_list)):
        # 遍历先存的作业构成的作业表, 选出已经或刚好在等待的作业, 选出它们之中运行时间最短的.
        arr_time_list = [i.arr_time for i in job_list]
        for i in range(len(arr_time_list)):
            arr_time_list[i] -= curr_time
            arr_time_list[i] = max(0, arr_time_list[i])    # 已经等待或刚好等待都记为刚到
        min_list = [i for i in range(len(arr_time_list)) if arr_time_list[i] == min(arr_time_list)] # 选出刚到的作业的序号
        t_list = [job_list[i] for i in min_list]    # 将"刚到"的作业存入t_list
        ely_cpt_time = [i.sev_time for i in t_list]    # 计算它们的服务时间
        idx = ely_cpt_time.index(min(ely_cpt_time))    # 选出"刚到"的作业中服务时间最短的
        curr_job = job_list.pop(min_list[idx])    # 选出目标作业, 进行一系列的操作
        curr_job_no = arr_time_list_initial.index(curr_job.arr_time)
        curr_cpt_time += curr_job.sev_time
        curr_job.cpt_time = curr_cpt_time
        curr_job.wghted_run_time = (curr_job.cpt_time - curr_job.arr_time) / curr_job.sev_time
        curr_job.run_time = curr_job.cpt_time - curr_job.arr_time
        print("job{}\t{}\t\t\t{}\t\t\t{}".format(curr_job_no, curr_job.arr_time, curr_job.sev_time,
                                                 curr_job.wghted_run_time))
        curr_time += curr_job.sev_time

    avg_run_time = sum([i.run_time for i in job_list_back_up]) / len(job_list_back_up)
    avg_wght_run_time = sum([i.run_time for i in job_list_back_up]) / sum([i.sev_time for i in job_list_back_up])
    print('avg_run_time: ', avg_run_time)
    print('avg_wght_run_time: ', avg_wght_run_time)
    return 0

while True:
    algorithm_choice = input("FCFS or SJF?\n")
    if algorithm_choice == 'FCFS':
        FCFS(job_list)
        break
    elif algorithm_choice == 'SJF':
        SJF(job_list)
        break
    elif algorithm_choice == 'q':
        break
    else:
        print("Invalid choice, enter 'q' to quit.\n")
