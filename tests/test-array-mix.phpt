--TEST--
Check for array
--SKIPIF--
--FILE--
<?php
if(!extension_loaded('binpack')) {
    dl('binpack.' . PHP_SHLIB_SUFFIX);
}

function test($title, $value, $dump = true) {
    $buf = bin_encode($value);
    $data = bin_decode($buf);

    if ($dump)
    {
        echo $title, PHP_EOL;
        echo 'buffer : ' . bin2hex($buf),  PHP_EOL;
        echo hex_dump($buf), PHP_EOL;
        var_dump($value);
        var_dump($data);
    }
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
$data['map'] = array('name' => 'srain', 'gender' => 1);
$data['list'] = array(1, 2, 3);
test('array', $data, false);

$data = array();

$data[-1] = -2;
$data[] = -1;
$data[] = 0;
$data[] = 1;

$data[] = null;
$data[] = true;
$data[] = false;
$data['num'] = 1111.23423;
$data['map'] = array('name' => 'srain', 'gender' => 1);
$data['list'] = array(1, 2, 3);
test('array', $data, true);
?>
--EXPECT--
OK
array
buffer : 036162406141404241430f44044505236e756d0640915cefd9fd36f8236d617003246e616d6525737261696e2667656e6465724101246c697374024142430101
     0 : 03 61 62 40 61 41 40 42 41 43 0f 44 04 45 05 23 [.ab@aA@BAC.D.E.#]
    10 : 6e 75 6d 06 40 91 5c ef d9 fd 36 f8 23 6d 61 70 [num.@.\...6.#map]
    20 : 03 24 6e 61 6d 65 25 73 72 61 69 6e 26 67 65 6e [.$name%srain&gen]
    30 : 64 65 72 41 01 24 6c 69 73 74 02 41 42 43 01 01 [derA.$list.ABC..]

array(10) {
  [-1]=>
  int(-2)
  [0]=>
  int(-1)
  [1]=>
  int(0)
  [2]=>
  int(1)
  [3]=>
  NULL
  [4]=>
  bool(true)
  [5]=>
  bool(false)
  ["num"]=>
  float(1111.23423)
  ["map"]=>
  array(2) {
    ["name"]=>
    string(5) "srain"
    ["gender"]=>
    int(1)
  }
  ["list"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
array(10) {
  [-1]=>
  int(-2)
  [0]=>
  int(-1)
  [1]=>
  int(0)
  [2]=>
  int(1)
  [3]=>
  NULL
  [4]=>
  bool(true)
  [5]=>
  bool(false)
  ["num"]=>
  float(1111.23423)
  ["map"]=>
  array(2) {
    ["name"]=>
    string(5) "srain"
    ["gender"]=>
    int(1)
  }
  ["list"]=>
  array(3) {
    [0]=>
    int(1)
    [1]=>
    int(2)
    [2]=>
    int(3)
  }
}
OK
