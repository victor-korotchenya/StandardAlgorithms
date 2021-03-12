#!/usr/bin/env python3
"""
Repair the file, specified in the first argument.
"""
from wsem import first_argument, parena_opa, wsem

opa = parena_opa()
FILE_NAME = first_argument()
wsem(FILE_NAME, opa)
