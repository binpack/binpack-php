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
array
buffer : 03616240614140424143ffffffffffffffffff40440643e000000000000045ffffffffffffffffff6046808080808080808080614706c3e0000000000000480f49044a05236e756d0640915cefd9fd36f8236d617003246e616d6525737261696e2667656e6465724101246c697374024142430101
     0 : 03 61 62 40 61 41 40 42 41 43 ff ff ff ff ff ff [.ab@aA@BAC......]
    10 : ff ff ff 40 44 06 43 e0 00 00 00 00 00 00 45 ff [...@D.C.......E.]
    20 : ff ff ff ff ff ff ff ff 60 46 80 80 80 80 80 80 [........`F......]
    30 : 80 80 80 61 47 06 c3 e0 00 00 00 00 00 00 48 0f [...aG.........H.]
    40 : 49 04 4a 05 23 6e 75 6d 06 40 91 5c ef d9 fd 36 [I.J.#num.@.\...6]
    50 : f8 23 6d 61 70 03 24 6e 61 6d 65 25 73 72 61 69 [.#map.$name%srai]
    60 : 6e 26 67 65 6e 64 65 72 41 01 24 6c 69 73 74 02 [n&genderA.$list.]
    70 : 41 42 43 01 01 [ABC..]

array(15) {
  [-1]=>
  int(-2)
  [0]=>
  int(-1)
  [1]=>
  int(0)
  [2]=>
  int(1)
  [3]=>
  int(9223372036854775807)
  [4]=>
  float(9.2233720368548E+18)
  [5]=>
  int(-9223372036854775807)
  [6]=>
  int(-9223372036854775808)
  [7]=>
  float(-9.2233720368548E+18)
  [8]=>
  NULL
  [9]=>
  bool(true)
  [10]=>
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
