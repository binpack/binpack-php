function exe_cmd()
{
    echo $1
    eval $1
}
exe_cmd "make clean"
exe_cmd "make"
exe_cmd "sudo cp modules/binpack.so /usr/local/php/extensions"
exe_cmd "php test-dev-test.php"

