<?php
$loop = 10000;
$retry = 10;
$value_display = false;

$types = array(
    'integer',
    'float',
    'string',
    'array',
    'hash',
    'mix',
);

function genData($type)
{
    switch ($type)
    {
    case 'integer':
        $value = rand();
        break;
    case 'float':
        $value = log(rand());
        break;
    case 'string':
        $value = md5(rand());
        break;
    case 'array':
        $value = array(md5(rand()),
            md5(rand()),
            md5(rand()),
            md5(rand()),
            md5(rand()));
        break;
    case 'hash':
        $value = array(md5(rand()) => md5(rand()),
            md5(rand()) => md5(rand()),
            md5(rand()) => md5(rand()),
            md5(rand()) => md5(rand()),
            md5(rand()) => md5(rand()));
        break;
    case 'mix':
        $element_types = array(
            'integer',
            'float',
            'string',
            'array',
            'hash',
        );
        $value = array();
        foreach ($element_types as $element)
        {
            $value[] = genData($t);
        }
        break;
    default:
        $value = null;
    }
    return $value;
}

foreach ($types as $type)
{
    $value = genData($type);

    if (!is_numeric($retry) || empty($retry))
    {
        $retry = 1;
    }

    $serialize_pack = 0;
    $serialize_unpack = 0;
    $serialize_size = 0;
    $serialize_status = '*NG*';
    $bin_pack = 0;
    $bin_unpack = 0;
    $bin_size = 0;
    $bin_status = '*NG*';
    $msgpack_pack = 0;
    $msgpack_unpack = 0;
    $msgpack_size = 0;
    $msgpack_status = '*NG*';

    for ($c = 0; $c < $retry; $c++)
    {
        //default (serialize)
        $pack = null;
        $unpack = null;

        $start = microtime(true);
        for ($i = 0; $i < $loop; $i++)
        {
            $pack = serialize($value);
        }
        $end = microtime(true);
        $serialize_pack += ($end - $start);

        $start = microtime(true);
        for ($i = 0; $i < $loop; $i++)
        {
            $unpack = unserialize($pack);
        }
        $end = microtime(true);
        $serialize_unpack += ($end - $start);

        $serialize_size += strlen($pack);
        if ($unpack === $value ||
            (is_object($value) && $unpack == $value))
        {
            $serialize_status = 'OK';
        }

        //bin
        $pack = null;
        $unpack = null;
        $opt = false;
        if (is_array($value))
        {
            $opt = true;
        }
        $start = microtime(true);
        for ($i = 0; $i < $loop; $i++)
        {
            $pack = bin_encode($value);
        }
        $end = microtime(true);
        $bin_pack += ($end - $start);

        $start = microtime(true);
        for ($i = 0; $i < $loop; $i++)
        {
            $unpack = bin_decode($pack, $opt);
        }
        $end = microtime(true);
        $bin_unpack += ($end - $start);

        $bin_size += strlen($pack);
        if ($unpack === $value ||
            (is_object($value) && $unpack == $value) ||
            (is_float($value) &&
            number_format($value, 10, '.', '') ===
            number_format($unpack, 10, '.', '')))
        {
            $bin_status = 'OK';
        }

        //msgpack
        $pack = null;
        $unpack = null;
        $start = microtime(true);
        for ($i = 0; $i < $loop; $i++)
        {
            $pack = msgpack_serialize($value);
        }
        $end = microtime(true);
        $msgpack_pack += ($end - $start);

        $start = microtime(true);
        for ($i = 0; $i < $loop; $i++)
        {
            $unpack = msgpack_unserialize($pack);
        }
        $end = microtime(true);
        $msgpack_unpack += ($end - $start);

        $msgpack_size += strlen($pack);
        if ($unpack === $value ||
            (is_object($value) && $unpack == $value))
        {
            $msgpack_status = 'OK';
        }
    }

    $serialize_pack /= $retry;
    $serialize_unpack /= $retry;
    $serialize_size /= $retry;
    $bin_pack /= $retry;
    $bin_unpack /= $retry;
    $bin_size /= $retry;
    $msgpack_pack /= $retry;
    $msgpack_unpack /= $retry;
    $msgpack_size /= $retry;

    printf("[%-10s] %13s %13s %13s\n",
        $type, 'default', 'binpack', 'msgpack');
    printf("status     : %12s  %12s  %12s\n",
        $serialize_status, $bin_status, $msgpack_status);
    printf("serialize  : %.4f (100%%) %.4f (%3d%%) %.4f (%3d%%)\n",
        $serialize_pack,
        $bin_pack, ($bin_pack / $serialize_pack * 100),
        $msgpack_pack, ($msgpack_pack / $serialize_pack * 100));
    printf("unserialize: %.4f (100%%) %.4f (%3d%%) %.4f (%3d%%)\n",
        $serialize_unpack,
        $bin_unpack, ($bin_unpack / $serialize_unpack * 100),
        $msgpack_unpack, ($msgpack_unpack / $serialize_unpack * 100));
    printf("size       : %6d (100%%) %6d (%3d%%) %6d (%3d%%)\n\n",
        $serialize_size,
        $bin_size, ($bin_size / $serialize_size * 100),
        $msgpack_size, ($msgpack_size / $serialize_size * 100));
    if ($value_display === true)
    {
        var_dump($value);
        echo PHP_EOL;
    }
}
