$(document).ready(createTestSocket);

function createTestSocket() {
	websocket = new WebSocket("ws://192.168.0.100:10000/arcap/player");
	
	websocket.onopen = function(e) {
		console.log("connected");
		$('#log').append('<div class="status">Connected</div>');
	};

	websocket.onmessage = function(e) {
		console.log(e.data);
		$('#log').append('<div class="incoming">' + e.data + '</div>');
	};
	
	websocket.onerror = function(e) {
		console.log(e.data);
		$('#log').append('<div class="error">' + e.data + '</div>');
	};
	
	websocket.onclose = function(e) {
		console.log("disconnected");
		$('#log').append('<div class="status">Disconnected</div>');
	};
	
	$('#send').submit(function(e) {
		e.preventDefault();
		var input = $('#send-data');
		var data = input.val().trim();
		console.log(data);
		// Ignore empty strings.
		if (data.length > 0) {
			// Check type.
			if (data.charAt(0) == '%') {
				// Command.
				switch (data) {
				case "%c":
				case "%clear":	
					$('#log').text('');
					break;
				}
			} else {
				// Message.
				websocket.send(data);
				$('#log').append('<div class="outgoing">' + data + '</div>');
			}
			// Reset the input.
			input.val('');
		}
	});
	
	$('#send-data').trigger('focus');
}