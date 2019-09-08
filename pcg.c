/**
 * PCG-64 for PHP 7.3.9
 *
 * Author: Dracovian (https://github.com/Dracovian)
 * Date  : September 8, 2019
 */

// Import our config header if the HAVE_CONFIG_H flag is set.
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// Include our necessary headers.
#include "php.h"
#include "ext/standard/info.h"
#include "php_pcg.h"

// For compatibility with older PHP versions
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
        ZEND_PARSE_PARAMETERS_START(0, 0) \
        ZEND_PARSE_PARAMETERS_END()
#endif

// Return the current PCG extension version (current PHP version).
PHP_FUNCTION(pcg_version) {
    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_STRING(PHP_PCG_VERSION);
}

// Return a random value that has a bounded min and max range.
PHP_FUNCTION(pcg_random) {

    // Create our seeds array.
    qword seeds[2];

    // Make use of our PCG structure.
    struct pcg_t pcg;

    // Fill our seeds array.
    entropy((void *) seeds, sizeof(seeds));

    // Run our seeded random function to initialize our PCG structure.
    pcg_srandom(&pcg, seeds[0], seeds[1]);

    // Store some long int values from the PHP function parameters.
    qword min, max;

    // Parse our PHP function with 2 minimum and 2 maximum arguments.
    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_LONG(min)
        Z_PARAM_LONG(max)
    ZEND_PARSE_PARAMETERS_END();

    // Return the bound results of our random function.
    RETURN_LONG(pcg_bind(&pcg, max) + min);
}

// Create our RINIT function.
PHP_RINIT_FUNCTION(pcg) {
#if defined(ZTS) && defined(COMPILE_DL_PCG)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    return SUCCESS;
}

// Create our MINFO function.
PHP_MINFO_FUNCTION(pcg) {
    php_info_print_table_start();
    php_info_print_table_header(2, "pcg support", "enabled");
    php_info_print_table_end();
}

// Create our argument information data.
ZEND_BEGIN_ARG_INFO(arginfo_pcg_version, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_pcg_random, 0)
    ZEND_ARG_INFO(0, min)
    ZEND_ARG_INFO(0, max)
ZEND_END_ARG_INFO()

// Set our PCG functions.
static const zend_function_entry pcg_functions[] = {
    PHP_FE(pcg_version, arginfo_pcg_version)
    PHP_FE(pcg_random,  arginfo_pcg_random)
    PHP_FE_END
};

// Set our module entry information.
zend_module_entry pcg_module_entry = {
    STANDARD_MODULE_HEADER,
    "pcg",                      // Extension name
    pcg_functions,              // Our functions
    NULL,                       // PHP_MINIT - Module Initialization
    NULL,                       // PHP_MSHUTDOWN - Module Shutdown
    PHP_RINIT(pcg),             // PHP_RINIT - Request Initialization
    NULL,                       // PHP_RSHUTDOWN - Request Shutdown
    PHP_MINFO(pcg),             // PHP_MINFO - Module Information
    PHP_PCG_VERSION,            // Extension version
    STANDARD_MODULE_PROPERTIES
};

// More thread safe options
#ifdef COMPILE_DL_PCG
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(pcg)
#endif