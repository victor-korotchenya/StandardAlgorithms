#!/usr/bin/env python3
"""
Wsem salut to repair source files: lint, tidy, reformat and remove extra spaces.

pip install --upgrade autopep8
pip install --upgrade cpplint
pip install --upgrade flawfinder
pip install --upgrade lizard
pip install --upgrade pycodestyle
pip install --upgrade pylint
pip install --upgrade pylint[spelling]
"""
import multiprocessing
import threading
import traceback
import os
import subprocess
import sys
import time

repkas = (("#include ", "#include"), ("{ //", "{//"))
FILE_ENCODING = "cp1252"
GSL_ENV_NAME = "GSL_INCLUDE"
FLUSH_COUT = True

# Found a 12 line (345 tokens) duplication in the following files:
CPD_FOUND_PREFIX = "Found a "
CPD_FOUND_PREFIX_SIZE = len(CPD_FOUND_PREFIX)
CPD_LINE_MID = " line ("
CPD_LINE_MID_SIZE = len(CPD_LINE_MID)

# Starting at line 159 of file1.h
CPD_START_PREFIX = "Starting at line "
CPD_START_PREFIX_SIZE = len(CPD_START_PREFIX)

# Duplicate block:
# --------------------------
# fi2.h:110 ~ 120
# fi2.h:310 ~ 320
# ^^^^^^^^^^^^^^^^^^^^^^^^^^
DUPLICATE_BLOCK_BEGIN = "Duplicate block:"
DUPLICATE_BLOCK_MID = " ~ "
DUPLICATE_BLOCK_END = "^^^^^^^^^^^^^^^^^^^^^^^^^^"


def first_argument() -> str:
    """
    The first argument of the program.
    """
    name = None if (len(sys.argv) < 2) else str(sys.argv[1])

    if not name:
        raise RuntimeError(
            "Error: should have passed in the argument - file name.")

    return name


def get_cores() -> int:
    """
    The number of CPU cores.
    """
    cores = multiprocessing.cpu_count()
    if not cores:
        cores = 3
    else:
        cores += 1

    return cores


def relative_path(dira: str, file_name: str) -> str:
    """
    Motiv: Cpp check cannot handle relative paths.
    Tests:
    >>> relative_path("/dira", "/dira/qwe\\rty.tXt")
    'qwe\\rty.tXt'
    >>> relative_path("diraQ", "dira/qwe\\rty.tXt")
    'dira/qwe\\rty.tXt'
    >>> relative_path("", "dira/qwe\\rty.tXt")
    'dira/qwe\\rty.tXt'
    >>> relative_path(".", "dira/qwe\\rty.tXt")
    'dira/qwe\\rty.tXt'
    """
    if not dira or not file_name or not file_name.startswith(dira):
        return file_name

    lena = len(dira)
    temp = file_name[lena:]

    while temp.startswith("\\") or temp.startswith("/"):
        temp = temp[1:]

    if temp:
        return temp
    return file_name


def int_str_parse(line: str, start_pos: int = 0):
    """
    Given ("Some 789 another", 5), return "789".
    Tests:
    >>> int_str_parse("Some 789 another", -9)
    ''
    >>> int_str_parse("Some 789 another", -1)
    ''
    >>> int_str_parse("Some 789 another")
    ''
    >>> int_str_parse("Some 789 another", 4)
    ''
    >>> int_str_parse("Some 789 another", 5)
    '789'
    >>> int_str_parse("Some 789 another", 6)
    '89'
    >>> int_str_parse("Some 789 another", 7)
    '9'
    >>> int_str_parse("Some 789 another", 8)
    ''
    >>> int_str_parse("Some 789 another", 9999)
    ''
    >>> int_str_parse("789 another")
    '789'
    >>> int_str_parse("789")
    '789'
    >>> int_str_parse("789", 1)
    '89'
    >>> int_str_parse("")
    ''
    """
    digits = ""
    lena = len(line) if line else 0

    if start_pos < 0 or lena <= 0 or lena <= start_pos:
        return digits

    for index in range(start_pos, lena):
        cha = line[index]
        if (cha < '0' or '9' < cha):
            break

        digits += cha

    return digits


class StrDer:
    """Act as if a string builder.
    Tests:
    >>> Dinar = StrDer("1 Dir")
    >>> str(Dinar)
    '1 Dir'
    >>> Dinar += "ham"
    >>> str(Dinar)
    '1 Dirham'
    """

    def __init__(self, initial: str = ""):
        self._str = initial

    def __add__(self, added: str):
        """Add another string."""
        self._str += added
        return self

    def clear(self):
        """Clear the string."""
        self._str = ""

    def ensure_line_end(self):
        """Ensure there is a '\n' at the end."""
        eol = '\n'

        if not self._str or eol == self._str[-1:]:
            return

        self._str += eol

    def is_empty(self):
        """Whether the string is empty."""
        return not self._str

    def __str__(self) -> str:
        """Return the accumulated string."""
        return self._str


class Opa:
    """
    Options.
    """

    def __init__(
            self,
            repair=False,
            reformat=True,
            files_count: int = 0,
            log_verbose=True):
        self._cur_dir = os.getcwd()
        if not self._cur_dir:
            raise RuntimeError("The currect directory must be set error.")

        self._repair = repair
        self._reformat = reformat
        self._is_parallel = not files_count or files_count <= 1
        self._log_verbose = log_verbose

        if repair:
            self._gsl_path = os.environ[GSL_ENV_NAME]
            self._gsl_path = "-I" + self.gsl_path()

        self._lock = multiprocessing.Lock()

        if reformat or repair:
            return

        raise RuntimeError("At least 1 option must be set.")

    def is_reformat(self) -> bool:
        """
        Whether to reformat the file(s).
        """
        return self._reformat

    def is_repair(self) -> bool:
        """
        Whether to run Cpp check, Cpp lint, Copy-paste detector, Clang-tidy.
        """
        return self._repair

    def gsl_path(self) -> str:
        """
        Path to GSL include.
        """
        if not self._gsl_path:
            raise RuntimeError(
                "Error: please set the environment variable '" +
                GSL_ENV_NAME +
                "'.")

        return self._gsl_path

    def is_parallel(self) -> bool:
        """
        Whether to run in parallel.
        """
        return self._is_parallel

    def is_log_verbose(self) -> bool:
        """
        Whether to log verbose.
        """
        return self._log_verbose

    def ban_log_verbose(self):
        """
        Ban log verbose.
        """
        self._log_verbose = False

    def curdir(self) -> str:
        """
        Path to the current directory.
        """
        return self._cur_dir

    def lock(self) -> multiprocessing.Lock:
        """
        The lock.
        """
        return self._lock


def kharasho_opa(files_count: int = 0) -> Opa:
    """
    Kharasho reformat options.
    """
    return Opa(files_count=files_count)


def parena_opa(files_count: int = 0) -> Opa:
    """
    Parena repa options.
    """
    return Opa(repair=True, files_count=files_count)


class AtomicInt:
    """
    To increment a value thread-safely.
    """

    def __init__(self, lock, end_value: int):
        if not lock:
            raise RuntimeError("The lock must have been set.")

        if not end_value or end_value < 0:
            end_value = 0

        self._lock = lock
        self._current = 0
        self._end = end_value

    def end(self) -> int:
        """ Return the end value. """
        return self._end

    def next(self) -> int:
        """
        Increment a value.
        Return -1 if failed.
        """
        res = -1

        with self._lock:
            if self._current < self._end:
                res = self._current
                self._current += 1

        return res


def skip_factor(source_stream: str, factor: str = "") -> str:
    """
    Skip every line, containing the factor if not empty, in a stream/string.
    """
    if not source_stream:
        return ""
    if not factor:
        return source_stream

    keepends = True
    lines = source_stream.splitlines(keepends)

    contents = "".join(lin for lin in lines if factor not in lin)
    return contents


def format_return_code(return_code: int) -> str:
    """
    Format the return code.
    """
    return f"   Return code is {return_code}.\n"


def append_process_result(
        report_ref: StrDer,
        parent_process,
        factor: str = ""):
    """
    Omit some false errors.
    """
    is_empty = True

    for stream in [parent_process.stdout, parent_process.stderr]:
        sub_rep = skip_factor(stream, factor)
        if sub_rep.strip():
            is_empty = False
            report_ref += sub_rep
            report_ref.ensure_line_end()

    return_code = parent_process.returncode
    if not is_empty:
        report_ref += format_return_code(return_code)

    return return_code


def safe_wrapper(func, file_name: str, opa: Opa):
    """
    Run a function in a try-except.
    In order to gather log messages together,
    the called function should append messages to report_ref,
    not print them to the console.
    """
    assert func and file_name and opa

    tnam = threading.current_thread().name
    report_ref = StrDer()

    try:
        func(file_name, opa, report_ref)
        print(report_ref, flush=FLUSH_COUT)
        # It may run in a thread - had better catch all exceptions.
    # pylint: disable-next=broad-except
    except Exception as exc:
        try:
            print(report_ref,
                  f"Error in thread '{tnam}', file '{file_name}'")

            traceback.print_exception(exc)
        # pylint: disable-next=broad-except
        except Exception:
            print("Unknown error in safe_wrapper.")


def check_cpp(file_name: str, opa: Opa, report_ref: StrDer):
    """
    Run Cpp check.
    """
    start_time = time.time()

    if opa.is_log_verbose():
        report_ref += f"  Start Cpp check '{file_name}'..\n"

    # pylint: disable-next=line-too-long
# cppcheck --enable=all --error-exitcode=0 --quiet --suppress=missingIncludeSystem --language=c++ --std=c++20 -D_DEBUG=1 --platform=win64 --inline-suppr --check-level=exhaustive --suppressions-list=cppcheck.surpress.txt main.cpp | findstr /V "missingIncludeSystem" # nopep8

    proc = subprocess.run(["cppcheck", "--enable=all",
                           "--quiet", "--error-exitcode=0",
                           "--language=c++", "--std=c++23",
                           "-D_DEBUG=1", "--platform=win64",
                           "--inline-suppr",  # "--check-level=exhaustive",
                           "--suppress=missingIncludeSystem",
                           "--suppressions-list=cppcheck.surpress.txt",
                           # Hm, it forces writing many temporary files.
                           # "--cppcheck-build-dir=.",
                           file_name],
                          check=False, capture_output=True,
                          encoding=FILE_ENCODING)

    append_process_result(report_ref, proc, "missingIncludeSystem")

    if opa.is_log_verbose():
        report_ref += f" Cpp check '{file_name}' done in {time.time() - start_time} seconds.\n"


def lint_cpp(file_name: str, opa: Opa, report_ref: StrDer):
    """
    Run Cpp lint.
    """
    start_time = time.time()

    if opa.is_log_verbose():
        report_ref += f"  Start Cpp lint '{file_name}'..\n"

    # pylint: disable-next=line-too-long
# cpplint --verbose=0 --output=emacs --counting=total --repository=. --recursive --quiet main.cpp # nopep8

    proc = subprocess.run(["cpplint",
                           "--verbose=0",
                           "--output=emacs",
                           "--counting=total",
                           "--repository=.",
                           "--recursive",
                           "--quiet",
                           file_name],
                          check=False,
                          capture_output=True,
                          encoding=FILE_ENCODING)

    append_process_result(report_ref, proc)

    if opa.is_log_verbose():
        report_ref += f" Cpp lint '{file_name}' done in {time.time() - start_time} seconds.\n"


def flaw_finder(file_name: str, opa: Opa, report_ref: StrDer):
    """
    Run Flaw finder.
    """
    start_time = time.time()

    if opa.is_log_verbose():
        report_ref += f"  Start Flaw finder '{file_name}'..\n"

    # pylint: disable-next=line-too-long
# flawfinder --columns --context --minlevel=0 --singleline --quiet main.cpp | findstr /V "Examining " # nopep8
    proc = subprocess.run(["flawfinder",
                           "--columns",
                           "--context",
                           "--minlevel=0",
                           "--singleline",
                           "--quiet",
                           file_name],
                          check=False,
                          capture_output=True,
                          encoding=FILE_ENCODING)

    append_process_result(report_ref, proc, "Examining ")

    if opa.is_log_verbose():
        report_ref += f" Flaw finder '{file_name}' done in {time.time() - start_time} seconds.\n"


def append_lizard_detail(
        filename: str,
        line: str,
        report_ref: StrDer,
        dup_count_arr):
    """
    Append Lizard details.
    """
    # fi2.h:110 ~ 120
    assert (line and dup_count_arr and DUPLICATE_BLOCK_MID in line)

    separ = ':'
    if separ not in line:
        report_ref += "Skipped a Lizard line separ warning.\n"
        return

    st_pos = line.find(separ)
    from_digits = int_str_parse(line, st_pos + len(separ))

    mid_pos = line.find(DUPLICATE_BLOCK_MID)
    tod_digits = int_str_parse(line, mid_pos + len(DUPLICATE_BLOCK_MID))

    if not from_digits or not tod_digits:
        report_ref += "Skipped Lizard digits warning.\n"
        return

    dup_count_arr[0] += 1
    dup = dup_count_arr[0]

    report_ref.ensure_line_end()
    report_ref += f"{filename}:{from_digits}: error: A Lizard duplicate"
    report_ref += f" #{dup} in lines {from_digits}..{tod_digits}.\n"


def report_lizard(parent_process, filename: str, report_ref: StrDer):
    """
    Report Cpd result.
    """
    for stream in [parent_process.stdout, parent_process.stderr]:
        if stream.strip():
            report_ref += stream
            report_ref.ensure_line_end()

    return_code = parent_process.returncode

    keepends = True
    lines = str(report_ref).splitlines(keepends)
    report_ref.clear()
    local_dups_arr = [0 * 1]
    total_duplicates = 0
    has_duplicate_block = False
    has_any = False

    for line in lines:
        report_ref += line

        if line.startswith(DUPLICATE_BLOCK_BEGIN):
            local_dups_arr[0] = 0
            has_duplicate_block = True
        elif has_duplicate_block and DUPLICATE_BLOCK_MID in line:
            append_lizard_detail(filename, line, report_ref, local_dups_arr)
        elif has_duplicate_block and 0 < local_dups_arr[0] and DUPLICATE_BLOCK_END in line:
            total_duplicates += local_dups_arr[0]

            report_ref.ensure_line_end()
            report_ref += f"{filename}: warning: {local_dups_arr[0]} Lizard duplicates.\n"

            local_dups_arr[0] = 0
            has_duplicate_block = False
            has_any = True

    if has_any:
        report_ref.ensure_line_end()
        report_ref += format_return_code(return_code)
        report_ref += filename
        report_ref += f": warning: Lizard has found {total_duplicates} total duplicates.\n"


def lizard_complexity(file_name: str, opa: Opa, report_ref: StrDer):
    """
    Analyze complexity.
    """
    start_time = time.time()

    if opa.is_log_verbose():
        report_ref += f"  Start Lizard '{file_name}'..\n"

    # pylint: disable-next=line-too-long
# lizard -Eduplicate --warnings_only -l cpp --length 100 --CCN 15 --arguments 5 --working_threads 1 main.cpp # nopep8

    proc = subprocess.run(["lizard",
                           "-Eduplicate",
                           "--warnings_only",
                           "-l",
                           "cpp",
                           "--length",
                           "100",
                           "--CCN",
                           "15",
                           "--arguments",
                           "5",
                           "--working_threads",
                           "1",
                           file_name],
                          check=False,
                          capture_output=True,
                          encoding=FILE_ENCODING)

    report_lizard(proc, file_name, report_ref)

    if opa.is_log_verbose():
        report_ref += f" Lizard '{file_name}' done in {time.time() - start_time} seconds.\n"


def cpd_header(filename: str, line: str) -> str:
    """
    Parse a Cpd header.
    """
    # Found a 12 line (345 tokens) duplication in the following files:
    assert (line and CPD_FOUND_PREFIX in line)

    if CPD_LINE_MID not in line:
        return "Has Cpd changed the output format warning?\n"

    line_digits = int_str_parse(line, CPD_FOUND_PREFIX_SIZE)
    # Must be like "12".

    mindex = line.index(CPD_LINE_MID)
    tokens = int_str_parse(line, mindex + CPD_LINE_MID_SIZE)
    # Must be like "345".

    if not line_digits or not tokens:
        return "No Cpd tokens warning.\n"

    return f"{filename} warning: {line_digits} line ({tokens} tokens) Cpd duplicate.\n"


def cpd_detail(filename: str, line: str, duplicate_count_arr) -> str:
    """
    Parse Cpd details.
    """
    # Starting at line 159 of file1.h
    assert (line and CPD_START_PREFIX in line)

    line_digits = int_str_parse(line, CPD_START_PREFIX_SIZE)
    # line_digits must be like "159"

    if not line_digits:
        return "No Cpd line digits warning.\n"

    duplicate_count_arr[0] += 1

    return f"{filename}:{line_digits}: error: Cpd duplicate #{duplicate_count_arr[0]} is found.\n"


def cpd_result(parent_process, filename: str):
    """
    Report Cpd result.
    """
    report_ref = StrDer()

    for stream in [parent_process.stdout, parent_process.stderr]:
        if stream.strip():
            report_ref += stream
            report_ref.ensure_line_end()

    return_code = parent_process.returncode

    keepends = True
    lines = str(report_ref).splitlines(keepends)
    report_ref.clear()
    duplicate_count_arr = [0 * 1]

    for line in lines:
        report_ref += line

        if line.startswith(CPD_FOUND_PREFIX):
            report_ref += cpd_header(filename, line)
        elif line.startswith(CPD_START_PREFIX):
            report_ref += cpd_detail(filename, line, duplicate_count_arr)

    if not report_ref.is_empty():
        report_ref.ensure_line_end()
        report_ref += format_return_code(return_code)

    return [return_code, duplicate_count_arr[0], str(report_ref)]


def run_copy_paste_detector(file_name: str, opa: Opa, report_ref: StrDer):
    """
    Run copy-paste detector.
    """
    start_time = time.time()

    if opa.is_log_verbose():
        report_ref += f"  Start copy-paste detector '{file_name}'..\n"

    # pylint: disable-next=line-too-long
# cpd.bat --language cpp --minimum-tokens 20 --dir main.cpp # nopep8

    proc = subprocess.run(["cpd.bat",
                           "--language",
                           "cpp",
                           "--minimum-tokens",
                           "20",
                           "--format",
                           "text",
                           "--dir",
                           file_name],
                          check=False,
                          capture_output=True,
                          encoding=FILE_ENCODING)

    [return_code, duplicate_count,
        local_report] = cpd_result(proc, file_name)

    report_ref += local_report

    if 0 == return_code and 0 != duplicate_count:
        return_code = -4
    elif 0 != return_code and 0 == duplicate_count:
        duplicate_count = -1

    if 4 == return_code:
        report_ref.ensure_line_end()
        report_ref += f"warning: There are {duplicate_count} Cpd duplicates in {file_name}.\n"
    elif 0 != return_code:
        report_ref.ensure_line_end()
        report_ref += f"error: {duplicate_count} Cpd duplicates.\n"
        report_ref += f" Unknown error code {return_code}.\n"

    if opa.is_log_verbose():
        elapsed = time.time() - start_time

        report_ref += f"  Done copy-paste detector '{file_name}' done in {elapsed} seconds.\n"


def clang_tidy(file_name: str, opa: Opa, report_ref: StrDer):
    """
    Run Clang tidy.
    """
    start_time = time.time()

    if opa.is_log_verbose():
        report_ref += f"  Start Clang tidy '{file_name}'..\n"

    # pylint: disable-next=line-too-long
# clang-tidy --config-file=.clang-tidy --fix --fix-notes --fix-errors --header-filter=*.h main.cpp -- -std=c++2b -x c++ -m64 -I%GSL_INCLUDE% -I../../clang_err_includes -D_DEBUG=1 | findstr /V "#pragma once" # nopep8

    proc = subprocess.run(["clang-tidy",
                           "--config-file=.clang-tidy",
                           "--fix",
                           "--fix-notes",
                           "--fix-errors",
                           "--header-filter=*.h",
                           file_name,
                           "--",
                           "-std=c++2b",
                           "-x",
                           "c++",
                           "-m64",
                           opa.gsl_path(),
                           "-I../../clang_err_includes",
                           "-D_DEBUG=1"],
                          check=False,
                          capture_output=True,
                          encoding=FILE_ENCODING)

    append_process_result(report_ref, proc, "#pragma once")

    elapsed = time.time() - start_time
    if opa.is_log_verbose():
        report_ref += f" Clang tidy '{file_name}' in {elapsed} seconds.\n"


def repair_lines(lines):
    """
    Repair the lines.
    """
    if not lines:
        return

    lena = len(lines)

    for index in range(lena):
        lin = lines[index]
        for froma, toda in repkas:
            lin = lin.replace(froma, toda)
        lines[index] = lin


def read_file_lines(file_name: str):
    """
    Read the file, and return the string lines.
    """
    with open(file_name, "r", encoding=FILE_ENCODING) as file:
        lines = list(file)
        return lines


def write_file(file_name: str, lines):
    """
    Write the file from the string lines.
    """
    with open(file_name, "w", encoding=FILE_ENCODING) as file:
        for lin in lines:
            file.write(lin)


def clang_format_and_space_repair(file_name: str, opa: Opa):
    """
    Run Clang format, then in-memory space repair.
    """
    start_time = time.time()

    if opa.is_log_verbose():
        print(f"  Start Clang format and space repair '{file_name}'..")

    fmt = subprocess.run(["clang-format", file_name], check=False,
                         capture_output=True, encoding=FILE_ENCODING)

    contents = skip_factor(fmt.stdout)
    errors = skip_factor(fmt.stderr)
    if errors:
        err = f"Clang format file '{file_name}' errors: {errors}"
        raise RuntimeError(err)

    if not contents:
        err = f"File '{file_name}' is empty error."
        raise RuntimeError(err)

    keepends = True
    lines = contents.splitlines(keepends)  # read_file_lines(file_name)

    repair_lines(lines)
    write_file(file_name, lines)

    elapsed = time.time() - start_time
    if opa.is_log_verbose():
        print(
            f" Clang format and space repair '{file_name}' in {elapsed} seconds.")


def run_repair(file_name: str, opa: Opa, shall_run_partially_useful=True):
    """
    Repair a file.
    """
    func_names = [  # These do not change the analyzed file:
        [check_cpp, "Cpp check"],
        [lint_cpp, "Cpp lint"]]

    if shall_run_partially_useful:
        func_names.append([run_copy_paste_detector, "Copy-paste detector"])
        func_names.append([lizard_complexity, "Analyze complexity"])
        func_names.append([flaw_finder, "Flaw finder"])

    # It may change the file, but it runs for 2 or more seconds.
    func_names.append([clang_tidy, "Clang-tidy"])

    threads = []

    for [func, name] in func_names:
        assert func and name

        if not opa.is_parallel():
            safe_wrapper(func, file_name, opa)
            continue

        threads.append(
            threading.Thread(
                target=safe_wrapper,
                args=(
                    func,
                    file_name,
                    opa),
                daemon=True,
                name=name))

    if not opa.is_parallel():
        return

    thread_count = len(func_names)

    if opa.is_log_verbose():
        print(f"  Starting {thread_count} repair threads..")

    for thread in threads:
        thread.start()

    for index in range(thread_count):
        thread = threads[index]
        _, thread_name = func_names[index]

        if opa.is_log_verbose():
            print(
                f"   Start joining thread '{thread_name}' {index} out of {thread_count}..")

        thread.join()

        if opa.is_log_verbose():
            print(
                f"  Done joining thread '{thread_name}' {index} out of {thread_count}.")


def wsem_unsafe(file_name: str, opa: Opa, report_ref: StrDer):
    """
    Wsem salut to repair a file.
    """
    start_time = time.time()

    if not file_name:
        raise RuntimeError("Error: should have passed in the file name.")

    cur_dir = opa.curdir()
    file_name = relative_path(cur_dir, file_name)

    if opa.is_log_verbose():
        print(report_ref, f"  Start '{file_name}' in '{cur_dir}'.")

    if opa.is_repair():
        run_repair(file_name, opa)

    if opa.is_reformat():
        # It will change the file.
        clang_format_and_space_repair(file_name, opa)

    # Last section.
    elapsed = time.time() - start_time
    if opa.is_log_verbose():
        print(f" Wsem salut '{file_name}' in {elapsed} seconds.")


def wsem(file_name: str, opa: Opa):
    """
    Wsem salut to repair a file.
    """
    safe_wrapper(wsem_unsafe, file_name, opa)


if __name__ == "__main__":
    import doctest  # via >>>
    doctest.testmod()
# todo(p3): Run Cppcheck, Cpplint, Tidy in RAM - should be faster.
