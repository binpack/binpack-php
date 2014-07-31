--TEST--
Check for integer
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

test('integer', -2);
test('integer', -1);
test('integer', 0);
test('integer', 1);
test('integer', PHP_INT_MAX, false);
test('integer', -PHP_INT_MAX, false);
test('integer', -PHP_INT_MAX - 1, false);
test('integer', PHP_INT_MAX + 1, false);
test('integer', -PHP_INT_MAX - 2, false);
?>
--EXPECT--
integer
buffer : 62
     0 : 62 [b]

int(-2)
OK
integer
buffer : 61
     0 : 61 [a]

int(-1)
OK
integer
buffer : 40
     0 : 40 [@]

int(0)
OK
integer
buffer : 41
     0 : 41 [A]

int(1)
OK
OK
OK
OK
OK
OK
