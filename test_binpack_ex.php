<?php

if(!extension_loaded('binpack')) {
    dl('binpack.' . PHP_SHLIB_SUFFIX);
}

$data = array();

$data[-1] = -2;
$data[] = -1;
$data[] = 0;
$data[] = 1;
// $data[] = PHP_INT_MAX;
// $data[] = PHP_INT_MAX + 1;
// $data[] = -PHP_INT_MAX;
// $data[] = (PHP_INT_MAX * -1) - 1;
// $data[] = (PHP_INT_MAX * -1) - 2;

$data[] = true;
$data[] = false;

$data['num'] = 1111.23423;
$data['user'] = array('name' => 'srain', 'gender' => 1);

$str = file_get_contents('src.json');
$data = json_decode($str, true);

$data = array();
$str = bin_encode($data);
$data1 = bin_decode($str);

$t1 = microtime(true);
if (empty($_SERVER['argv'][1]))
{
    for ($i = 0; $i < 1000000; $i++)
    {
        bin_encode($data);
    }
    $t2 = microtime(true);
    echo $t2 - $t1, "\n";
}
else
{
    for ($i = 0; $i < 1000000; $i++)
    {
        bin_encode($data);
    }
    $t2 = microtime(true);
    echo 'decode_ex ', $t2 - $t1, "\n";
}
?>
