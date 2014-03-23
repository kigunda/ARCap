<!DOCTYPE html>
<html>
<head>
</head>
<body>
<div class="error">
<?php
// Connect to the database.
$mysqli = new mysqli("localhost", "admin", "admin", "arcap");
if ($mysqli->connect_errno) {
    echo "Failed to connect to MySQL: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
}

// Update infrared event count.
$mysqli->query("UPDATE event SET count = count + 1 WHERE id = 1");?>
</div>
<div>
<?php
	$result = $mysqli->query("SELECT name, count FROM event");
	$result->data_seek(0);?>
	<table><?php
	while ($event = $result->fetch_assoc()) {?>
		<tr class="event-entry">
			<td class="event-name"><?=$event['name']?></td>
			<td class="event-count"><?=$event['count']?></td>
		</tr><?php } ?>
	</table>
</div>
</body>
</html>