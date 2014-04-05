function Track(controller, placement, cssClass, motor) {
	this.controller = controller;
	this.motor = motor;
	this.position = {
			x: placement.x - (TrackButton.width / 2),
			y: placement.y - (TrackButton.height / 2),
	};
	this.position = $.extend({
		width: TrackButton.innerWidth,
		height: TrackButton.innerHeight,
	}, placement, this.position);
	this.buttons = {};
	this.buttons.selected = null;
	this.createMoveButtons(-1, "f");
	this.createMoveButtons(1, "b");
	this.createStopButton();
}

Track.prototype.createMoveButtons = function(sign, direction) {
	TrackButton.create(this, {
		origin: this.position,
		offset: {x: 0, y: sign * 1},
	}, this.cssClass, {
		motor: this.motor,
		direction: direction,
		speed: 70,
	});
	TrackButton.create(this, {
		origin: this.position,
		offset: {x: 0, y: sign * 2},
	}, this.cssClass, {
		motor: this.motor,
		direction: direction,
		speed: 85,
	});
	TrackButton.create(this, {
		origin: this.position,
		offset: {x: 0, y: sign * 3},
	}, this.cssClass, {
		motor: this.motor,
		direction: direction,
		speed: 100,
	});
};

Track.prototype.createStopButton = function() {
	var stop = TrackButton.create(this, {
		origin: this.position,
		offset: {x: 0, y: 0},
	}, this.cssClass, {
		motor: this.motor,
		direction: 'f',
		speed: 0,
	}, {
		attr: {
			deselected: {
				opacity: 0.8,
			},
		},
	});
	stop.select();
};

Track.create = function(controller, placement, cssClass, motor) {
	return new Track(controller, placement, cssClass, motor);
};