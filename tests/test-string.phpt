--TEST--
Check for string
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

test('string', 'If I could tell you.');
?>
--EXPECT--
string
buffer : 94204966204920636f756c642074656c6c20796f752e
     0 : 94 20 49 66 20 49 20 63 6f 75 6c 64 20 74 65 6c [. If I could tel]
    10 : 6c 20 79 6f 75 2e [l you.]

string(20) "If I could tell you."
OK
