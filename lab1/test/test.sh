###
 # @Author: flwfdd
 # @Date: 2024-03-19 15:52:03
 # @LastEditTime: 2024-03-19 20:24:45
 # @Description: 
 # _(:з」∠)_
### 

script_dir=$(dirname "$0")
cd $script_dir
g++ ../compilerlab1.cpp -o compilerlab1.out
./compilerlab1.out src.txt > out.txt
sed -e '/##REPLACEME##/{
    r out.txt
    d
}' template.s > result.s
gcc -m32 -no-pie result.s -o result.out
./result.out