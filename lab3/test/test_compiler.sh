###
 # @Author: flwfdd
 # @Date: 2024-04-16 20:51:34
 # @LastEditTime: 2024-06-07 15:12:33
 # @Description: 
 # _(:з」∠)_
### 
script_dir=$(dirname "$0")
cd $script_dir/../build
cmake ..
make
cd ../test
../build/Compilerlab3 src.txt