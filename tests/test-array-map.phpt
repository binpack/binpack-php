--TEST--
Check for array(as map)
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
    echo 'buffer : ' . bin2hex($buf),  PHP_EOL;
    echo hex_dump($buf), PHP_EOL;
    var_dump($data);
    echo $data === $value ? 'OK' : 'ERROR', PHP_EOL;
}

function hex_dump($data, $newline="\n")
{
    static $from = '';
    static $to = '';

    static $width = 16; # number of bytes per line

    static $pad = '.'; # padding for non-visible characters

    if ($from === '')
    {
        for ($i=0; $i <= 0xFF; $i++)
        {
            $from .= chr($i);
            $to .= ($i >= 0x20 && $i <= 0x7E) ? chr($i) : $pad;
        }
    }

    $hex = str_split(bin2hex($data), $width * 2);
    $chars = str_split(strtr($data, $from, $to), $width);

    $offset = 0;
    $ret = '';
    foreach ($hex as $i => $line)
    {
        $ret .= sprintf('%6X',$offset).' : '.implode(' ', str_split($line,2)) . ' [' . $chars[$i] . ']' . $newline;
        $offset += $width;
    }
    return $ret;
}

$arr = array('name' => 'srain', 'location' => 'Beijing');
test('array-map', $arr);
?>
--EXPECT--
array-map
buffer : 03246e616d6525737261696e286c6f636174696f6e274265696a696e6701
     0 : 03 24 6e 61 6d 65 25 73 72 61 69 6e 28 6c 6f 63 [.$name%srain(loc]
    10 : 61 74 69 6f 6e 27 42 65 69 6a 69 6e 67 01 [ation'Beijing.]

array(2) {
  ["name"]=>
  string(5) "srain"
  ["location"]=>
  string(7) "Beijing"
}
OK
