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

$item = array('hi');
$arr = array(&$item, &$item);
test('array-ref', $arr);
?>
--EXPECT--
array-ref
buffer : 020222686901022268690101
     0 : 02 02 22 68 69 01 02 22 68 69 01 01 [.."hi.."hi..]

array(2) {
  [0]=>
  array(1) {
    [0]=>
    string(2) "hi"
  }
  [1]=>
  array(1) {
    [0]=>
    string(2) "hi"
  }
}
OK
