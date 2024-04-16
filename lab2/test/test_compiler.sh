###
 # @Author: flwfdd
 # @Date: 2024-04-16 20:51:34
 # @LastEditTime: 2024-04-17 00:11:08
 # @Description: 
 # _(:з」∠)_
### 
script_dir=$(dirname "$0")
cd $script_dir/../build
cmake ..
make
cd ../test
../build/Compilerlab2 src.txt