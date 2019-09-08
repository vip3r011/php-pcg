# php-pcg
PCG-64 PRNG support in PHP 7.3

# requirements
64-bit version of PHP 7.3
64-bit operating system with a CPU that has access to the RDTSC opcode (x86_64 and AMD64 only)

# functions
```php
// Return the module version which should match the PHP version it was written for (7.3.9 in this case).
pcg_version() : string;

// Return a random value between $min and $max (accepting up to 64-bit values).
pcg_random(int $min, int $max) : int;

// I can't guarantee the accuracy of the pcg_random function when it comes to negative values.
// It's best not to generate negative values, just send it a positive value and subract that from whatever you desire.
```
