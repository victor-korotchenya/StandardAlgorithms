#!/usr/bin/env python3
"""
Remove temporary files.
"""
import os

extensions = (".bak", ".cscope_file_list", ".gch", ".save", ".save-failed")

CURRENT_DIR = os.curdir
COUNT = 0

for root, dirs, files in os.walk(CURRENT_DIR):
    for file in files:
        if file.lower().endswith(extensions):
            COUNT += 1
            full_name = os.path.join(root, file)
            print(f" Del {COUNT}-th file '{full_name}'..")
            os.remove(full_name)

print(f"Has removed {COUNT} files.")

# input("Press <ENTER> to continue..")
