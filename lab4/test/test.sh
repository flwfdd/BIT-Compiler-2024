###
 # @Author: flwfdd
 # @Date: 2024-04-16 20:51:34
 # @LastEditTime: 2024-06-08 13:52:27
 # @Description: 
 # _(:з」∠)_
### 
script_dir=$(dirname "$0")
cd $script_dir/../build
cmake ..
make
cd ../test
../build/Compilerlab4 src.txt > result.s
gcc -m32 -no-pie result.s -o result.out
./result.out 