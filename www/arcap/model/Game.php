<?php

require_once 'Party.php';

/**
 * A class that represents an ARCap game.
 * @author Kenan Kigunda
 *
 */
class Game {
	/**
	 * The game ID.
	 * @var Integer
	 */
	public $id;
	/**
	 * The time when the game was created.
	 * @var DateTime
	 */
	public $datecreated;

	/**
	 * The number of players who can participate in the game.
	 * @var Integer
	 */
	public $maxplayers = 2;

	/**
	 * The current number of players in the game.
	 * @var Integer
	 */
	public $numplayers = 0;

	/**
	 * The current status of the game.
	 * One of "open" or "closed".
	 * @var String
	 */
	public $status = "open";

	/**
	 * The database handler.
	 * @var PDO
	 */
	public $db;

	// DATABASE

	/**
	 * Adds a new game to the given database.
	 * @param PDO $db the database handler
	 * @return Game the new game
	 */
	public static function addGameTo($db) {
		return (new Game())->withDatabase($db)->insert();
	}

	/**
	 * Retrieves the game with the given ID from the database.
	 * @param Integer $id the ID of the desired game
	 * @param PDO $db the database handler
	 * @return Game the desired game
	 */
	public static function selectById($id, $db) {
		$stmt = $db->prepare("SELECT " .
				"id, datecreated, maxplayers, numplayers, status " .
				"FROM game WHERE id = :id;");
		$stmt->execute(array(":id" => $id));
		$stmt->setFetchMode(PDO::FETCH_CLASS, "Game");
		$game = $stmt->fetch();
		if ($game == null) {
			return $game;
		} else {
			return $game->withDatabase($db);
		}
	}

	/**
	 * Sets the database handler used by this game.
	 * @param PDO $db
	 * @return Game this game
	 */
	public function withDatabase($db) {
		$this->db = $db;
		return $this;
	}

	/**
	 * Inserts this game into the database.
	 * @return Game this game
	 */
	public function insert() {
		// Insert the game.
		$stmt = $this->db->prepare("INSERT INTO game " .
				"(maxplayers, numplayers, status) " .
				"VALUES " .
				"(:maxplayers, :numplayers, :status);"
		);
		$stmt->execute(array(
				":maxplayers" => $this->maxplayers,
				":numplayers" => $this->numplayers,
				":status" => $this->status,
		));
		// Get the ID.
		$this->id = $this->db->lastInsertId();
		return $this;
	}

	// ADDING PLAYERS

	/**
	 * Adds a new party to this game.
	 * If this game is under capacity, then the party will be added as a player.
	 * If this game is at capacity, then the party will be added as a viewer.
	 * @return Party the new party
	 */
	public function addPlayer() {
		if ($this->numplayers < $this->maxplayers) {
			$party = Party::addPartyTo($this, "player");
			$this->incrementPlayerCount();
			return $party;
		} else {
			return Party::addPartyTo($this, "viewer");
		}
	}

	/**
	 * Increments the player count.
	 */
	private function incrementPlayerCount() {
		$stmt = $this->db->prepare("UPDATE game SET " .
				"numplayers = numplayers + 1 " .
				"WHERE id = :id;"
		);
		$stmt->execute(array(":id" => $this->id));
		$this->numplayers++;
	}
}