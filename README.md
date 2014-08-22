binpack-php
===========

The php implementation for binpack.


### installation

*  by complie

    ```
    phpize
    ./configure
    make & make install
    ```

*  install from PECL

    BINPACK is an PECL extension, thus you can simply install it by:
    
    pecl install binpack


*   brew for Mac

    Binpack has been added to homebrew-php, so you can:
    
    ```
    brew search binpack
    ```
    
    Choose a version to install:
    
    ```
    brew install homebrew/php/php54-binpack
    ```

### usage

```php
<?php
$data = array('name' => 'srain');
$str = bin_encode($data);
$data = bin_decode($str);
```
