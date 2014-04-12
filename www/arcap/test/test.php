<?php $version = 1; ?>
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>Socket Test</title>
<link rel="stylesheet" href="test.css?v=<?=$version?>" />
</head>
<body>
	<div id="content">
		<form id="send">
			<input id="send-data" 
				type="text" placeholder="Enter your message here" value=""
				size="64" 
			/>
		</form>
		<div id="log"></div>
	</div>
	<script src="/jquery-2.1.0.min.js"></script>
	<script src="test.js?v=<?=$version?>"></script>
</body>
</html>