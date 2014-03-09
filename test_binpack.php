<?php
$str = file_get_contents('src.json');
$src_data = json_decode($str, true);
function hex_dump($data, $newline="\n")
{
    static $from = '';
    static $to = '';

    static $width = 16; # number of bytes per line

    static $pad = '.'; # padding for non-visible characters

    if ($from==='')
    {
        for ($i=0; $i<=0xFF; $i++)
        {
            $from .= chr($i);
            $to .= ($i >= 0x20 && $i <= 0x7E) ? chr($i) : $pad;
        }
    }

    $hex = str_split(bin2hex($data), $width*2);
    $chars = str_split(strtr($data, $from, $to), $width);

    $offset = 0;
    foreach ($hex as $i => $line)
    {
        echo sprintf('%6X',$offset).' : '.implode(' ', str_split($line,2)) . ' [' . $chars[$i] . ']' . $newline;
        $offset += $width;
    }
}

$br = (php_sapi_name() == "cli")? "":"<br>";
if(!extension_loaded('binpack')) {
    dl('binpack.' . PHP_SHLIB_SUFFIX);
}

if(!extension_loaded('vbs')) {
    dl('vbs.' . PHP_SHLIB_SUFFIX);
}

$module = 'binpack';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:$br\n";
foreach($functions as $func) {
    echo $func."$br\n";
}
echo "$br\n";

$data = array();

$data[-1] = -2;
$data[] = -1;
$data[] = 0;
$data[] = 1;
$data[] = PHP_INT_MAX;
$data[] = PHP_INT_MAX + 1;
$data[] = -PHP_INT_MAX;
// $data[] = (PHP_INT_MAX * -1) - 1;
// $data[] = (PHP_INT_MAX * -1) - 2;

$data[] = true;
$data[] = false;

$data['num'] = 1111.23423;
$data['user'] = array('name' => 'srain', 'gender' => 1);

// $data = $src_data;
$data = (PHP_INT_MAX * -1) - 1;
echo $data;
$str = bin_encode($data);
hex_dump($str);
$data1 = bin_decode($str);

var_export($data1);
echo "\n", PHP_INT_MAX, "\n";
