This project just calls a function written in Python(i.e Rectangle.py) from Golang(i.e main.go), using CPYTHON api(i.e. glue.c).

# How to run this project, you asked?
- OS : Ubuntu 22.04.1 LTS
- Python Version Used 3.8.9(But should run with any Python3.X version)
- export PKG_CONFIG_PATH=~/.pyenv/versions/3.8.9/lib/pkgconfig/
 
 The `PKG_CONFIG_PATH` would be different for the reader. To check it on your system type the following command.

`locate python3.pc`

- run `make find_area`