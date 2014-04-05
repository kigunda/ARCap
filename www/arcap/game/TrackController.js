var controller;

$(document).ready(function() {
	controller = TrackController.create();
});

function TrackController() {
	this.connection = new Connection(this);
	this.draw = SVG('controller').size(Controller.width, Controller.height);
	this.shape = this.draw.rect(Controller.width, Controller.height).fill('#f5f5f5');
	this.status = {
			ready: false,
			marker: this.draw
			.rect(Controller.status.marker.width, Controller.status.marker.height)
			.move(Controller.status.marker.x, Controller.status.marker.y)
			.fill("red"),
	};
	// Create tracks for move commands.
	// Directions are switched.
	this.move = {};
	this.move.left = Track.create(this, {
		x: TrackButton.width / 2,
		y: Controller.height * (1/2),
	}, "move move-left", "r");
	this.move.right = Track.create(this, {
		x: Controller.width - (TrackButton.width / 2),
		y: Controller.height * (1/2),
	}, "move move-right", "l");
	this.move.both = {};
	this.move.both.left = Track.create(this, {
		x: TrackButton.width * (3 / 2),
		y: Controller.height * (1/2),
		width: TrackButton.width / 2,
	}, "move move-both-left", "b");
	this.move.both.left = Track.create(this, {
		x: Controller.width - (TrackButton.width + TrackButton.outerSize),
		y: Controller.height * (1/2),
		width: TrackButton.width / 2
	}, "move move-both-right", "b");
	// Create simple buttons for the shoot command.
	this.shoot = SimpleButton.create(this, {
		x: Controller.width * (3/16),
		y: Controller.height - SimpleButton.size,
	}, "shoot", "is");
	this.shoot = SimpleButton.create(this, {
		x: Controller.width * (13/16),
		y: Controller.height - SimpleButton.size,
	}, "shoot", "is");
	console.log(this.move);
}

TrackController.create = function() {
	return new TrackController();
};

Controller = {};
Controller.width = $('#content').width();
Controller.height = $('#content').height();

Controller.status = {
		marker: {
			width: Controller.width,
			height: 10,
		},
};
Controller.status.marker.x = Controller.width * (1/2) - Controller.status.marker.width / 2;
Controller.status.marker.y = Controller.height - Controller.status.marker.height;

TrackController.prototype.ready = function(ready) {
	if (typeof(ready) !== 'undefined') {
		// If value provided, set and update the status marker.
		this.status.ready = ready;
		if (ready) {
			this.status.marker.fill('green');
		} else {
			this.status.marker.fill('red');
			// TODO Stop all tracks.
		}
		return this;
	} else {
		// If no value provided, return the current status.
		console.log(this.status.ready);
		return this.status.ready;
	}
};

TrackController.prototype.perform = function(action) {
	this.connection.send(action);
};

TrackController.prototype.hit = function() {
	this.hitBlink(0);
};

TrackController.prototype.hitBlink = function(count) {
	var controller = this;
	if (count < 3) {
		this.shape.animate(250).fill('#da532c').after(function() {
			this.animate(250).fill('#f5f5f5').after(function() {
				controller.hitBlink(count + 1);
			});
		});
	}
};

TrackController.prototype.group = function(group, button) {
	if (!(group in this.move)) {
		this.move[group] = [];
	}
	this.move[group].push(button);
};


