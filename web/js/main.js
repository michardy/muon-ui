var ws = new WebSocket("ws://"+window.location.host+"/socket");
ws.onopen = function() {
	ws.send("Hello Server");
};

var queue_push = Module.cwrap('queue_push', 'null', ['string']);

ws.onmessage = function (evt) {
	queue_push(evt.data);
};
