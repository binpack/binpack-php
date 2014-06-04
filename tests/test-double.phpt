--TEST--
Check for double
--SKIPIF--
--FILE--
<?php
if(!extension_loaded('binpack')) {
    dl('binpack.' . PHP_SHLIB_SUFFIX);
}

function test($title, $value) {
    $buf = bin_encode($value);
    $data = bin_decode($buf);

    echo $title, PHP_EOL;
    echo bin2hex($buf), PHP_EOL;
    var_dump($data);
    echo $data === $value ? 'OK' : 'ERROR', PHP_EOL;
}

test('double', -3.14);
test('double', 3655.198);
?>
--EXPECT--
double
06c0091eb851eb851f
float(-3.14)
OK
double
0640ac8e6560418937
float(3655.198)
OK
