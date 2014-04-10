<?php
$br = (php_sapi_name() == "cli")? "":"<br>";
if(!extension_loaded('binpack')) {
    dl('binpack.' . PHP_SHLIB_SUFFIX);
}

$module = 'binpack';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:$br\n";
foreach($functions as $func) {
    echo $func."$br\n";
}
echo "$br\n";

$data = require 'test-get-test-data.php';

$str = bin_encode($data);
$data1 = bin_decode($str);

var_dump($data); echo "\n";
var_dump($data1); echo "\n";
var_export($data1 == $data);echo "\n";
