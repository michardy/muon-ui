class Queue {
	constructor() {
		this.mq = [];
	}
	add_messages(messages) {
		for (m in messages.split('\r\n')) {
			this.mq.push();
		}
	}
	get_message() {
		return this.mq.pop();
	}
}

q= new Queue();

var ws = new WebSocket("ws://"+window.location.host+"/socket");
ws.onopen = function() {
	ws.send("Hello Server");
};
ws.onmessage = function (evt) {
	q.add_messages(evt.data);
};
