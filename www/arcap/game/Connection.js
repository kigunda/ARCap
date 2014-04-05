function Connection(controller) {
	this.controller = controller;
	var address = $('#server-address').text().trim();
	var port = $('#server-port').text().trim();
	this.uri = 'ws://' + address + ':' + port + '/arcap/player';
	this.websocket = new WebSocket(this.uri);
	this.id = $('#player-id').text().trim();
	this.connected = false;
	var connection = this;
	
	this.websocket.onopen = function(e) {
		console.log('[WebSocket] Connected');
		// Send player details.
		connection.websocket.send('+p' + connection.id);
		// Send a test shot.
		connection.websocket.send('is');
	};

	this.websocket.onmessage = function(e) {
		// Ignore the final newline.
		var message = e.data.substring(0, e.data.length - 1);
		console.log('[WebSocket] ' + message);
		// Check message tokens.
		var tokens = message.split(' ');
		switch (tokens[0]) {
		case ':ok':
			connection.controller.ready(true);
			break;
		case ':err':
			connection.controller.ready(false);
			break;
		case 'ih':
			connection.controller.hit(tokens[1]);
			break;
		}
	};
	
	this.websocket.onerror = function(e) {
		console.log('[WebSocket] error: ' + e.data);
	};
	
	this.websocket.onclose = function(e) {
		console.log('[WebSocket] disconnected');
	};
}

Connection.prototype.send = function(message) {
	if (!this.controller.ready) return false;
	this.websocket.send(message + '\n');
	console.log('[WebSocket] ' + message);
	return true;
};