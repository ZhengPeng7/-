#!/usr/bin/python3.5

## Modify the banker algorithm on allocating the memory.
## This is a very stupid way, don't try it, but enough for assignment.
## Final_exam is listed first now. Poor education system.

class process():
    def __init__(self, Max, Allocation, Need):
        self.Max = Max
        self.Allocation = Allocation
        self.Need = Need
        self.max_A = Max['A']
        self.max_B = Max['B']
        self.max_c = Max['C']
        self.allo_a = Allocation['A']
        self.allo_b = Allocation['B']
        self.allo_c = Allocation['C']
        self.need_a = Need['A']
        self.need_b = Need['B']
        self.need_c = Need['C']
    def request(self):
        need = [self.need_a, self.need_b, self.need_c]
        allocate = [self.allo_a, self.allo_b, self.allo_c]
        if 0 == sum([available[i] < need[i] for i in range(len(available))]):
            for i in range(len(available)):
                available[i] += allocate[i]
            return 0
        return -1


max_p0 = {'A': 7, 'B': 5, 'C': 3}
allocation_p0 = {'A': 0, 'B': 1, 'C': 0}
need_p0 = {'A': 7, 'B': 4, 'C': 3}

max_p1 = {'A': 3, 'B': 2, 'C': 2}
allocation_p1 = {'A': 2, 'B': 0, 'C': 0}
need_p1 = {'A': 1, 'B': 2, 'C': 2}

max_p2 = {'A': 9, 'B': 0, 'C': 2}
allocation_p2 = {'A': 3, 'B': 0, 'C': 2}
need_p2 = {'A': 6, 'B': 0, 'C': 0}

max_p3 = {'A': 2, 'B': 2, 'C': 2}
allocation_p3 = {'A': 2, 'B': 1, 'C': 1}
need_p3 = {'A': 0, 'B': 1, 'C': 1}

max_p4 = {'A': 4, 'B': 3, 'C': 3}
allocation_p4 = {'A': 0, 'B': 0, 'C': 2}
need_p4 = {'A': 4, 'B': 3, 'C': 1}


num_A = 3
num_B = 3
num_C = 2

# initial_
available = [num_A, num_B, num_C]
# available = initial_available.copy()
initial_dict_available = {'A': num_A, 'B': num_B, 'C': num_C}
dict_available = initial_dict_available.copy()
p0 = process(max_p0, allocation_p0, need_p0)
p1 = process(max_p1, allocation_p1, need_p1)
p2 = process(max_p2, allocation_p2, need_p2)
p3 = process(max_p3, allocation_p3, need_p3)
p4 = process(max_p4, allocation_p4, need_p4)

plist = [p0, p1, p2, p3, p4]

def check_deadlock(plist):
    if_done = [1, 1, 1, 1, 1]
    circuit = len(plist)
    track = []
    while True:
        if not sum(if_done):
            print('安全路径可为%s' % track)
            return 0
        elif not circuit:
            print('无安全路径')
            return -1
        for i in range(len(plist)):
            if if_done[i] == 0:
                continue
            if not plist[i].request():
                track.append('p' + str(i))
                if_done[i] = 0
                print(i)
                print(circuit, '*')
        circuit -= 1

check_deadlock(plist)

