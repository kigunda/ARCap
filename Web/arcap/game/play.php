<?php 
$version = 16;
$mode = 'dpad';
$hasPlayerId = isset($_GET['p']);
$playerId = $hasPlayerId? $_GET['p'] : "";

require_once '../socket/config.php';
?>
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8" />
<link rel="stylesheet" href="play.css?v=<?=$version?>" />
</head>
<body>
	<div class="info" id="server-address">
		<?=ARCAP_SERVER_ADDRESS?>
	</div>
	<div class="info" id="server-port">
		<?=ARCAP_SERVER_PORT?>
	</div>
	<div class="info" id="player-id">
		<?=$playerId?>
	</div>
	<header>
		<a id="header" href="play.php"> <span class="title">ARCap</span> <span
			class="subtitle"> <?php 
			switch($playerId) {
				case "1": echo "one"; break;
				case "2": echo "two"; break;
				default: echo $playerId; break;
			}?>
		</span>
		</a>
	</header>
	<?php
	if (!$hasPlayerId) {
	?>
	<section id="content">
		<div id="player-select">
			<a href="play.php?p=1"><span class="player-option" data-id="1">Rover
					1</span> </a> <a href="play.php?p=2"><span class="player-option"
				data-id="2">Rover 2</span> </a>
			<!-- <a href="play.php?p=viewer"><span
				class="player-option" data-id="viewer">Viewer</span> </a>-->
		</div>
	</section>
	<?php
	} else {
	?>
	<section id="content">
		<div id="controller"></div>
	</section>
	<script src="/jquery-2.1.0.min.js"></script>
	<script src="/svg.min.js"></script>
	<script src="/svg.parser.min.js"></script>
	<script src="/svg.import.min.js"></script>
	<script src="Connection.js?v=<?=$version?>"></script>
	<?php if ($mode == "track") {?>
	<script src="TrackController.js?v=<?=$version?>"></script>
	<script src="Track.js?v=<?=$version?>"></script>
	<script src="TrackButton.js?v=<?=$version?>"></script>
	<?php } else if ($mode == "dpad") {?>
	<script src="DpadController.js?v=<?=$version?>"></script>
	<script src="Dpad.js?v=<?=$version?>"></script>
	<script src="DpadButton.js?v=<?=$version?>"></script>
	<?php }?>
	<script src="SimpleButton.js?v=<?=$version?>"></script>
	<script src="StatusMarker.js?v=<?=$version?>"></script>
	<?php
	}
	?>
</body>
</html>
