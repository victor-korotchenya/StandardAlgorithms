forfiles /s /m *.py /c "cmd /c autopep8 --in-place --aggressive --aggressive @path"

rem pause
