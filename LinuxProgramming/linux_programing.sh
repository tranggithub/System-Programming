#!/bin/bash

# Kiểm tra xem đã truyền PID chưa
if [ -z "$1" ]; then
  echo "Usage: $0 <PID>"
  exit 1
fi

PID=$1

# Kiểm tra xem process có tồn tại không
if [ ! -d "/proc/$PID" ]; then
  echo "Error: Process with PID $PID does not exist."
  exit 1
fi

# 1. Lấy fullpath của process
echo "Full path of the process with PID $PID:"
readlink -f /proc/$PID/exe

# 2. Liệt kê tất cả các thư viện đang được load trên bộ nhớ
echo
echo "Libraries loaded in memory for process with PID $PID:"
cat /proc/$PID/maps
