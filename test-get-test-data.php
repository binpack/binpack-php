<?php
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
$data['user'] = array('name' => 'srain', 'gender' => 1);
$data['user'] = array(1, 2, 3);
return $data;
