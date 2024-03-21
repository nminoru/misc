#! /usr/bin/env python3

input_file = '/home/nminoru/xxx.csv'
elapsed_time_array = []
current_project_id = 0
max_val = -1.0
min_val = 1000000.0

def isint(s):
    try:
        int(s, 10)
    except ValueError:
        return False
    else:
        return True            


with open(input_file, 'r') as f:
    for line in f:
        array = line.split(',')
        # array[0]: project-id
        # array[1]: exit
        # array[4]: elapsed-time
        project_id  = int(array[0])
        elapsed_time = float(array[4])

        if project_id != current_project_id:
            count = len(elapsed_time_array)
            if count > 0:
                elapsed_time_array.sort()
                percentile50 = elapsed_time_array[int(count * 0.5)]
                percentile90 = elapsed_time_array[int(count * 0.9)]
                
                print(str(current_project_id) + ', ' + str(min_val) + ', ' + str(max_val) + ', ' + str(percentile50) + ', ' + str(percentile90) + ', ' + str(count))
                elapsed_time_array = []
                current_project_id = project_id
                max_val = -1.0
                min_val = 1000000.0

        if isint(array[1]) and int(array[1], 10) == 0:
            if max_val < elapsed_time:
                max_val = elapsed_time

            if min_val > elapsed_time:
                min_val = elapsed_time
            
            elapsed_time_array.append(elapsed_time)
