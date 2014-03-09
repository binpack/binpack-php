<?php
if ($_SERVER['argc'] < 2) {
    echo 'Usage: php ' . $_SERVER['argv'][0] . ' 1 | 2';
    exit;
}

if(!extension_loaded('binpack')) {
    dl('binpack.' . PHP_SHLIB_SUFFIX);
}

if(!extension_loaded('msgpack')) {
    dl('msgpack.' . PHP_SHLIB_SUFFIX);
}
$data = array();
$data[] = -1111;
$data['user'] = array('name' => 'srain', 'gender' => 1);
$data['num'] = 1111.23423;

$str = file_get_contents('src.json');
$data = json_decode($str, true);

$t1 = microtime(true);
$total_t = 10000;
$type = $_SERVER['argv'][1];
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
    echo "bin\t",  $t2 - $t1, "\n";
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
    echo "msgpack\t",  $t2 - $t1, "\n";
}
