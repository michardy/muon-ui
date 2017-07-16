function graph(pos, value) {
	var ga = document.getElementById("grapharea");
	gc = ga.getContext('2d');
}

var ws = new WebSocket("ws://"+window.location.hostname+":8888/socket");
ws.onopen = function() {
	ws.send("Hello Server");
};

var queue_push = Module.cwrap('queue_push', 'null', ['string']);

ws.onmessage = function (evt) {
	queue_push(evt.data);
};
