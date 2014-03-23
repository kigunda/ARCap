<?php

require_once 'Game.php';

/**
 * A class that represents a party in a {@link Game}.
 * @author Kenan Kigunda
 *
 */
class Party {
	/**
	 * The party ID.
	 * @var Integer
	 */
	public $id;
	
	/**
	 * The number of bits used to generate the code.
	 * The resulting code will be twice as long.
	 * @var Integer
	 */
	public static $CODE_BIT_COUNT = 4;
	
	/**
	 * The code used to verify the party.
	 * @var String
	 */
	public $code;
	
	/**
	 * The type of party.
	 * One of "player" or "viewer".
	 * @var String
	 */
	public $type;
	
	/**
	 * The ID of the parent game.
	 * @var $Integer
	 */
	public $game;
	
	/**
	 * The name of the party.
	 * @var String
	 */
	public $name = "";
	
	/**
	 * The ID of the rover that this party controls.
	 * Will be null if the party type is "viewer".
	 * @var Integer
	 */
	public $rover = null;
	
	/**
	 * Whether this party is online.
	 * @var Boolean
	 */
	public $online = true;
	
	/**
	 * The amount of energy available to this party.
	 * Not used if the party type is "viewer".
	 * @var Integer
	 */
	public $energy = 100;
	
	/**
	 * The party's current score.
	 * Not used if the party type is "viewer".
	 * @var Integer
	 */
	public $score = 0;
	
	/**
	 * The database handler.
	 * @var PDO
	 */
	public $db;
	
	// CONSTRUCTION
	
	/**
	 * Sets the parent game and type of this party.
	 * @param Game $game the parent game
	 * @param String $type the type, either "player" or "viewer"
	 * @return Party this party
	 */
	public function inGameWithType($game, $type) {
		$this->game = $game->id;
		$this->type = $type;
		return $this;
	}
	
	// DATABASE

	/**
	 * Adds a new party of the given type to the given game.
	 * @param Game $game the parent game of the party
	 * @param String $type the type of party, either "player" or "viewer"
	 * @return Party this party
	 */
	public static function addPartyTo(Game $game, $type) {
		return (new Party())
		->inGameWithType($game, $type)
		->withDatabase($game->db)
		->insert();
	}
	
	/**
	 * Retrieves the party with the given ID from the database.
	 * @param Integer $id the ID of the desired party
	 * @param PDO $db the database handler
	 * @return Party the desired party
	 */
	public static function selectById($id, PDO $db) {
		$stmt = $db->prepare("SELECT " .
				"id, code, type, game, name, rover, online, energy, score " .
				"FROM party WHERE id = :id;"
		);
		$stmt->execute(array(":id" => $id));
		$stmt->setFetchMode(PDO::FETCH_CLASS, "Party");
		return $stmt->fetch();
	}
	
	/**
	 * Retrieves the party with the given code from the database.
	 * @param String $code the code identifying the desired party
	 * @param PDO $db the database handler
	 * @return Party the desired party
	 */
	public static function selectByCode($code, PDO $db) {
		$stmt = $db->prepare("SELECT " .
				"id, code, type, game, name, rover, online, energy, score " .
				"FROM party WHERE code = :code;"
		);
		$stmt->execute(array(":code" => $code));
		$stmt->setFetchMode(PDO::FETCH_CLASS, "Party");
		return $stmt->fetch();
	}
	
	/**
	 * Sets the database handler used by this party.
	 * @param PDO $db the database handler to set
	 * @return Party this party
	 */
	public function withDatabase(PDO $db) {
		$this->db = $db;
		return $this;
	}
	
	/**
	 * Inserts this party into the database.
	 * @param PDO $db
	 * @return Party this party
	 */
	public function insert() {
		// Generate the code.
		$this->code = bin2hex(openssl_random_pseudo_bytes(Party::$CODE_BIT_COUNT));
		// Insert the party.
		$stmt = $this->db->prepare("INSERT INTO party " .
				"(code, type, name, game, rover, online, energy, score) " .
				"VALUES " .
				"(:code, :type, :name, :game, :rover, :online, :energy, :score);"
				);
		$stmt->execute(array(
				":code" => $this->code,
				":type" => $this->type,
				":name" => $this->name,
				":game" => $this->game,
				":rover" => $this->rover,
				":online" => $this->online,
				":energy" => $this->energy,
				":score" => $this->score,
		));
		// Get the ID.
		$this->id = $this->db->lastInsertId();
		return $this;
	}
}