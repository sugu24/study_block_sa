import subprocess
import time

# thread number and total thread number
N = 20
max_N = 5000

# start thread
exe = []
for w in range(2, 13):
    for rho in range(2, w+1):
        exe.append(("./sudoku.exe", w, rho))
        exe.append(("./sudoku_one.exe", w, rho))

# exe = [(4, 2),(4, 2),(3, 2), (3, 3), (4, 2), (4, 3), (4, 4), (5, 2), (5, 3), (5, 4), (5, 5), (6, 2), (6, 3), (6, 4), (6, 5), (6, 6), (7, 2), (7, 3), (7, 4), (7, 5), (7, 6), (7, 7), (8, 2), (8, 3), (8, 4), (8, 5), (8, 6), (8, 7), (8, 8)]
# exe = [(9, 2), (9, 3), (9, 4), (9, 5), (9, 6), (9, 7), (9, 8), (9, 9), (10, 2), (10, 3), (10, 4), (10, 5), (10, 6), (10, 7), (10, 8), (10, 9), (10, 10)]
# exe = [(10, 2)]
def runThread(i):
    global max_N
    exe_file, beam, max_parent = exe[max_N % len(exe)]
    if max_N > 0:
        #sudoku_thread = subprocess.Popen(['../cpp/study3/study/sudoku', '{}'.format(i), '&'])
        #sudoku_thread = subprocess.Popen(['../cpp/sudoku', '{}'.format(i), '&'])
        #sudoku_thread = subprocess.Popen(['../cpp/study4/study/sudoku', '{}'.format(i), '{}'.format((max_N%8)*250+250),'&'])
        #sudoku_thread = subprocess.Popen(['../cpp/cannotwatch/study/sudoku', '{}'.format(i), '{}'.format((max_N%4)+8),'&'])
        #sudoku_thread = subprocess.Popen(['../cpp/study5/study/sudoku', '{}'.format(i), '&'])
        # sudoku_thread = subprocess.Popen(['../cpp/study7/sudoku', '{}'.format(i), '&'])
        #sudoku_thread = subprocess.Popen(['../cpp/study7/sudoku', '{}'.format(i), '{}'.format((max_N%7)*250+500),'&'])
        # sudoku_thread = subprocess.Popen(['../cpp/study10/sudoku', '{}'.format(i), '{}'.format(max_N%39)])
        #with open("exe_done.txt", mode="a") as f:
        #    write_string = str(max_N) + " " + str(beam) + " " + str(max_parent) + "\n"
        #    f.write(write_string)
        # sudoku_thread = subprocess.Popen(['../cpp/study10/sudoku', '{}'.format(beam), '{}'.format(max_parent)])
        # if max_N % 2:
        #     sudoku_thread = subprocess.Popen(['../cpp/study11/sudoku', '4', '2'])
        #sudoku_thread = subprocess.Popen(['../cpp/study12/sudoku', '{}'.format(beam), '{}'.format(max_parent)])
        sudoku_thread = subprocess.Popen(['{}'.format(exe_file), '{}'.format(beam), '{}'.format(max_parent)])
        max_N -= 1
        return sudoku_thread
    else:
        return None


# list of thread
sudoku_threads = list()

# N threads are starting
for i in range(N):
    sudoku_threads.append(runThread(i))

# goal is max_N threads 
while max_N > 0:
    for i in range(N):
        if sudoku_threads[i].poll() is not None:
            sudoku_threads[i] = runThread(i)
    
    time.sleep(10)
