# 一键编译脚本，执行这个脚本可以一键编译整个项目
#!/bin/bash

set -x

rm -rf `pwd`/build/*
cd `pwd`/build &&
	cmake .. &&
	make