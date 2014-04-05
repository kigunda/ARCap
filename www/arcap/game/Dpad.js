function Dpad(controller, placement, cssClass) {
	this.controller = controller;
	this.position = {
			x: placement.x - (DpadButton.width / 2),
			y: placement.y - (DpadButton.height / 2),
	};
	this.buttons = {};
	this.buttons.selected = null;
	this.buttons.stop = this.createButton({x: 0, y: 0}, ['mbf0'], "stop").select();
	this.buttons.up = this.createButton({x: 0, y: -1,}, ['mbf100'], "forward");
	this.buttons.down = this.createButton({x: 0, y: 1}, ['mbb100'], "backward");
	this.buttons.left = this.createButton({x: -1, y: 0}, ['mlf80', 'mrf60', 'mrb60'], "left");
	this.buttons.right = this.createButton({x: 1, y: 0}, ['mrf80', 'mlf60', 'mlb60'], "right");
}

Dpad.prototype.createButton = function(offset, actions, name) {
	return DpadButton.create(this, {
		origin: this.position,
		offset: offset,
	}, this.cssClass, actions, name);
};

Dpad.create = function(controller, placement, cssClass) {
	return new Dpad(controller, placement, cssClass);
};