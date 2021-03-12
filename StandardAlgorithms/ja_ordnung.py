#!/usr/bin/env python3
"""
Make order in possibly many files.
cls && python3 ja_ordnung.py > ord.txt
"""
import glob
import sys
import time
import threading
from wsem import AtomicInt, get_cores, kharasho_opa, parena_opa, wsem

FILE_MASK = "Geometry\\*.*"
FILE_MASK = "Graphs\\*.*"
FILE_MASK = "Numbers\\*.*"
FILE_MASK = "Strings\\*.*"
FILE_MASK = "Utilities\\*.*"
FILE_MASK = "**\\*.cpp"
FILE_MASK = "**\\*.h"
FILE_MASK = "zum\\*.*"

SHALL_REPAIR = False
SHALL_REPAIR = True


def ja_exec(files_list, increm: AtomicInt, opas):
    """Executes the job in a thread."""
    while True:
        ind = increm.next()
        if ind < 0:
            if opas.is_log_verbose():
                print("No more jobs are left.")
            return

        assert ind < len(files_list)
        filenam = files_list[ind]
        wsem(filenam, opa)


start_time = time.time()

CORES = get_cores()

print(
    f"  Start getting files with mask '{FILE_MASK}' on {CORES} cores..",
    flush=True)

files = glob.glob(FILE_MASK, recursive=True)
files_count = len(files)
if not files_count:
    print(f"No files to run on mask '{FILE_MASK}'.")
    sys.exit(-123456)

opa = (parena_opa if SHALL_REPAIR else kharasho_opa)(files_count)
opa.ban_log_verbose()

incr = AtomicInt(opa.lock(), files_count)
thread_count = min(CORES, files_count)
assert 0 < thread_count

threads = [threading.Thread(target=ja_exec, args=(files, incr, opa),
                            daemon=True, name=str(index))
           for index in range(thread_count)]

if opa.is_log_verbose():
    print(
        f"  Starting {thread_count} threads to process {files_count} files on {CORES} cores..")

for index in range(thread_count):
    threads[index].start()

for index in range(thread_count):
    thre = threads[index]
    if not thre:
        if opa.is_log_verbose():
            print(f"Thread '{index}' has been NOT run.")
        continue

    if opa.is_log_verbose():
        print(f"   Start joining thread {index} out of {thread_count}..")
    thre.join()
    if opa.is_log_verbose():
        print(f"  Done joining thread {index} out of {thread_count}.")

elapsed = time.time() - start_time
print(
    f"Ordered {files_count} files '{FILE_MASK}', {thread_count} threads took {elapsed} seconds.")

# input("Press ENTER to close.")
