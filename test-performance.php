<?php
if ($_SERVER['argc'] < 2) {
    echo 'Usage: php ' , $_SERVER['argv'][0] , ' 1 | 2', "\n";
    exit;
}

if(!extension_loaded('binpack')) {
    dl('binpack.' . PHP_SHLIB_SUFFIX);
}

if(!extension_loaded('msgpack')) {
    dl('msgpack.' . PHP_SHLIB_SUFFIX);
}

$data = require 'test-get-test-data.php';

$total_t = 100001;
$type = $_SERVER['argv'][1];

$t1 = microtime(true);
if ($type == 1)
{
    for ($i = 0; $i < $total_t; $i++)
    {
        $str = bin_encode($data);
        $data1 = array();
        $data1['str'] = $str;
        $str = bin_encode($data1);
        $data1 = bin_decode($str);
        $data1 = bin_decode($data1['str']);
    }
    $t2 = microtime(true);
    echo "binpack\t",  $t2 - $t1, "\t" . 'data length: ' . strlen($str) . "\n";
}
else if ($type == 2)
{
    for ($i = 0; $i < $total_t; $i++)
    {
        $str = msgpack_serialize($data);
        $data1 = array();
        $data1['str'] = $str;
        $str = msgpack_serialize($data1);
        $data1 = msgpack_unserialize($str);
        $data1 = msgpack_unserialize($data1['str']);
    }
    $t2 = microtime(true);
    echo "msgpack\t",  $t2 - $t1, "\t" . 'data length: ' . strlen($str) . "\n";
}
