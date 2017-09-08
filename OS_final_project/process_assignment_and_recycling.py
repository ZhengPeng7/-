# 动态分区式存储管理
# Grade: 2015
# Class: 02
# Name: 郑鹏
# Sno:  2015015247

# initialize the memories and the process list(actually a dict)
total_memory = 110
os_memory = 10
process_memory = total_memory - os_memory
process = {}
sep_line_counter = 0

while 1:
	# For the neatness of the output of the program
	sep_line_counter += 1
	print('----------------------Seperation Line %d--------------------------' % sep_line_counter)

	# Input of choice, which is robustic here.
	choice = input('Assign memory or recycle memory or quit?\nPlease enter--assgnment: 1, recycle: 2, quit: q\n:')
	if not ( choice == 'q' or choice == '1' or choice == '2'):
		print('Please enter the right choice.')
		continue
	
	if choice == 'q':
		# Choose to quit
		break

	if int(choice) == 1:
		# Choose to add a process
		# Get name
		cur_proc_name = input('Name of the process?\n:')
		if cur_proc_name in process.keys():
			# if name of current process has existed in the process list
			print('The process you assigned has been created, no sharing of processes\' name.')
			continue
		# Get size
		cur_proc_size = int(input('OK, and what about its size?\n:'))
		if sum(process.values()) + int(cur_proc_size) > process_memory:
			# With current process entering the process list, the list would overload
			print('STOP ASSIGNMENT! Otherwise there\'s overload.')
			print( 'The remaining memory: %d KB' % ( process_memory - sum(process.values()) ) )
			print('The process list:', process)
			continue
		# No accidents, current process would be added into the process list
		process[cur_proc_name] = cur_proc_size
		print( 'The remaining memory: %d KB' % ( process_memory - sum(process.values()) ) )
		print('The process list:', process)
		continue

	if int(choice) == 2:
		# Choose to recycle a process
		cur_proc_name = input('Name of the process?\n:')
		if not cur_proc_name in process.keys():
			# If you want to recycle some process that doesn't exist
			print('No such process.')
			continue
		# No accidents, current process would be removed from the process list
		del process[cur_proc_name]
		print( 'The remaining memory: %d KB' % ( process_memory - sum(process.values() ) ) )
		print('The process list:', process)
		continue

