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

$data = array();

$data[-1] = -2;
$data[] = -1;
$data[] = 0;
$data[] = 1;
$data[] = PHP_INT_MAX;
$data[] = PHP_INT_MAX + 1;
$data[] = -PHP_INT_MAX;
$data[] = (PHP_INT_MAX * -1) - 1;
$data[] = (PHP_INT_MAX * -1) - 2;

$data[] = null;
$data[] = true;
$data[] = false;
$data['num'] = 1111.23423;
$data['user'] = array('name' => 'srain', 'gender' => 1);
$data['arr'] = range(1, 100);

$str = bin_encode($data);
$data1 = bin_decode($str);

var_dump($data); echo "\n";
var_dump($data1); echo "\n";
var_export($data1 == $data);echo "\n";
