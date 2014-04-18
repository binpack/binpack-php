binpack-php
===========

The php implementation for binpack.


### installation

```
phpize
./configure
make & make install
```

### usage

```php
<?php
$data = array('name' => 'srain');
$str = bin_encode($data);
$data1 = bin_decode($str);
```
