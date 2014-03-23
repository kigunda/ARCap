<?php

require_once 'Party.php';

for ($i = 0; $i < 1000; $i++) {
	?>
	<div>
		<?=bin2hex(openssl_random_pseudo_bytes(Party::$CODE_BIT_COUNT));?>
	</div>
	<?php 
}

?>