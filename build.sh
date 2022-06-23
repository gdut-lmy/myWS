#########################################################################
# File Name:    build.sh
# Author:       lmy
# email:        1429185804@qq.com
# Created Time: 2022年06月23日 星期四 19时12分58秒
#########################################################################
#!/bin/bash

rm main
rm -rf build
echo "创建编译目录：build"
mkdir build

echo "进入build"

cd build

cmake ..

make

echo "移动.main"

mv main ../

cd ../  

./main
