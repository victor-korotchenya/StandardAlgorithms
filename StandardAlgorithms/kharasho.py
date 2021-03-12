#!/usr/bin/env python3
"""
Reformat the file, specified in the first argument.
"""
from wsem import first_argument, kharasho_opa, wsem

opa = kharasho_opa()
FILE_NAME = first_argument()
wsem(FILE_NAME, opa)
