<?php

// Command codes.
define("ARCAP_COMMAND_PREFIX", ":");
define("ARCAP_CONNECT_PREFIX", "+");
define("ARCAP_OK", 	ARCAP_COMMAND_PREFIX . "ok");
define("ARCAP_ERR", ARCAP_COMMAND_PREFIX . "err");

// Socket details.
//define("ARCAP_SERVER_ADDRESS", '127.0.0.1');
define("ARCAP_SERVER_ADDRESS", '192.168.0.100');
define("ARCAP_SERVER_PORT", 10000);
define("ARCAP_SERVER_BACKLOG", 5);
define("ARCAP_SERVER_MAX_READ_LENGTH", 128);

// Workaround for missing flags for socket_recv.
if (!defined('MSG_DONTWAIT')) {
	define('MSG_DONTWAIT', 0x40);
}