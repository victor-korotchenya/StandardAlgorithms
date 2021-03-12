#!/usr/bin/env python3
"""
Zip the source files.
"""
import subprocess
import time

start_time = time.time()

OUTPUT_FILE = "StandardAlgorithms.zip"

subprocess.run(["7z", "a", "-bt", "-mx9", "-r",
                OUTPUT_FILE, "*.h", "*.cpp", "*.py", "*.sdb"],
               check=True)

elapsed = time.time() - start_time
print("\nZip elapsed", elapsed, "seconds.")

pause = input("Press <ENTER> to continue..")
