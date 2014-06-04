--TEST--
Check for blob
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
    echo 'buffer befer encode:     ' . bin2hex($value), PHP_EOL;
    echo 'buffer after decode:     ' . bin2hex($data), PHP_EOL;
    echo 'the encode result:   ' . bin2hex($buf), PHP_EOL;
    echo $data === $value ? 'OK' : 'ERROR', PHP_EOL;
}

$buffer = bin_encode('If I could tell you.');
test('blob', $buffer);
?>
--EXPECT--
blob
buffer befer encode:     94204966204920636f756c642074656c6c20796f752e
buffer after decode:     94204966204920636f756c642074656c6c20796f752e
the encode result:   962094204966204920636f756c642074656c6c20796f752e
OK
