--TEST--
Check for array(as list)
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

$arr = array(1, 2, 3, 10);
test('array-list', $arr);
?>
--EXPECT--
array-list
buffer : 024142434a01
     0 : 02 41 42 43 4a 01 [.ABCJ.]

array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(10)
}
OK
