<?php 

require_once 'model/Game.php';

// Connect to the database.
try {
	$db = new PDO("mysql:host=localhost;dbname=arcap", "admin", "admin");
	$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
} catch (PDOException $e) {
	die("Error: {$e->getMessage()}");
}

// Check for game ID.
if (!isset($_GET['g'])) {
	// If no game ID, create a new game.
	$game = Game::addGameTo($db);
	header("Location: /arcap/play.php?g={$game->id}");
} else {
	// Get the game.
	$game = Game::selectById($_GET['g'], $db);
	// Check player code.
	if (!isset($_GET['p'])) {
		// If no player code, create a new player.
		$player = $game->addPlayer();
		header("Location: /arcap/play.php?g={$game->id}&p={$player->code}");
	} else {
		// Get the player.
		$player = Party::selectByCode($_GET['p'], $db);
		// Print the results.
		?><pre><?=print_r($game);?></pre><?php 
		?><pre><?=print_r($player);?></pre><?php
	}
}

?>